#include <mbgl/gl/upload_pass.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/enum.hpp>
#include <mbgl/gl/defines.hpp>
#include <mbgl/gl/command_encoder.hpp>
#include <mbgl/gl/vertex_buffer_resource.hpp>
#include <mbgl/gl/index_buffer_resource.hpp>
#include <mbgl/gl/texture_resource.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace gl {

using namespace platform;

UploadPass::UploadPass(gl::CommandEncoder& commandEncoder_, const char* name)
    : commandEncoder(commandEncoder_), debugGroup(commandEncoder.createDebugGroup(name)) {
}

std::unique_ptr<gfx::VertexBufferResource> UploadPass::createVertexBufferResource(
    const void* data, std::size_t size, const gfx::BufferUsageType usage) {
    BufferID id = 0;
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
    commandEncoder.context.renderingStats().numBuffers++;
    commandEncoder.context.renderingStats().memVertexBuffers += size;
    // NOLINTNEXTLINE(performance-move-const-arg)
    UniqueBuffer result{ std::move(id), { commandEncoder.context } };
    commandEncoder.context.vertexBuffer = result;

    // If this might be a float buffer with three components, overallocate the VBO by half
    // to mitigate a buffer overrun in libGAL's computeWLimit (as of imx-gpu-viv 6.2.4.p4.0)
    if (!(size % 12) && usage == gfx::BufferUsageType::StreamDraw) {
        static const bool enabled = !getenv("MAPBOX_NO_WLIMIT_WORKAROUND");
        static const bool logEnabled = getenv("MAPBOX_LOG_WLIMIT_WORKAROUND");
        std::size_t paddedSize = size + (size / 2);

        if (logEnabled) {
            if (enabled) {
                Log::Info(Event::OpenGL, "Overallocating VBO %d (%d -> %d) for computeWLimit overrun",
                          int(id), int(size), int(paddedSize));
            } else {
                Log::Info(Event::OpenGL, "Would overallocate VBO %d (%d -> %d) for computeWLimit "
                          "overrun, but MAPBOX_NO_WLIMIT_WORKAROUND is set", int(id), int(size),
                          int(paddedSize));
            }
        }
        if (!enabled)
            paddedSize = size;

        MBGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, paddedSize, nullptr,
                                      Enum<gfx::BufferUsageType>::to(usage)));
        MBGL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
    } else {
        MBGL_CHECK_ERROR(
            glBufferData(GL_ARRAY_BUFFER, size, data, Enum<gfx::BufferUsageType>::to(usage)));
    }
    return std::make_unique<gl::VertexBufferResource>(std::move(result), size);
}

void UploadPass::updateVertexBufferResource(gfx::VertexBufferResource& resource,
                                            const void* data,
                                            std::size_t size) {
    commandEncoder.context.vertexBuffer = static_cast<gl::VertexBufferResource&>(resource).buffer;
    MBGL_CHECK_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

std::unique_ptr<gfx::IndexBufferResource> UploadPass::createIndexBufferResource(
    const void* data, std::size_t size, const gfx::BufferUsageType usage) {
    BufferID id = 0;
    MBGL_CHECK_ERROR(glGenBuffers(1, &id));
    commandEncoder.context.renderingStats().numBuffers++;
    commandEncoder.context.renderingStats().memIndexBuffers += size;
    // NOLINTNEXTLINE(performance-move-const-arg)
    UniqueBuffer result{ std::move(id), { commandEncoder.context } };
    commandEncoder.context.bindVertexArray = 0;
    commandEncoder.context.globalVertexArrayState.indexBuffer = result;
    MBGL_CHECK_ERROR(
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, Enum<gfx::BufferUsageType>::to(usage)));
    return std::make_unique<gl::IndexBufferResource>(std::move(result), size);
}

void UploadPass::updateIndexBufferResource(gfx::IndexBufferResource& resource,
                                           const void* data,
                                           std::size_t size) {
    // Be sure to unbind any existing vertex array object before binding the index buffer
    // so that we don't mess up another VAO
    commandEncoder.context.bindVertexArray = 0;
    commandEncoder.context.globalVertexArrayState.indexBuffer =
        static_cast<gl::IndexBufferResource&>(resource).buffer;
    MBGL_CHECK_ERROR(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data));
}

std::unique_ptr<gfx::TextureResource>
UploadPass::createTextureResource(const Size size,
                                  const void* data,
                                  gfx::TexturePixelType format,
                                  gfx::TextureChannelDataType type) {
    auto obj = commandEncoder.context.createUniqueTexture();
    int textureByteSize = gl::TextureResource::getStorageSize(size, format, type);
    commandEncoder.context.renderingStats().memTextures += textureByteSize;
    std::unique_ptr<gfx::TextureResource> resource =
        std::make_unique<gl::TextureResource>(std::move(obj), textureByteSize);
    commandEncoder.context.pixelStoreUnpack = { 1 };
    updateTextureResource(*resource, size, data, format, type);
    // We are using clamp to edge here since OpenGL ES doesn't allow GL_REPEAT on NPOT textures.
    // We use those when the pixelRatio isn't a power of two, e.g. on iPhone 6 Plus.
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    return resource;
}

void UploadPass::updateTextureResource(gfx::TextureResource& resource,
                                       const Size size,
                                       const void* data,
                                       gfx::TexturePixelType format,
                                       gfx::TextureChannelDataType type) {
    // Always use texture unit 0 for manipulating it.
    commandEncoder.context.activeTextureUnit = 0;
    commandEncoder.context.texture[0] = static_cast<gl::TextureResource&>(resource).texture;
    MBGL_CHECK_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, Enum<gfx::TexturePixelType>::to(format),
                                  size.width, size.height, 0,
                                  Enum<gfx::TexturePixelType>::to(format),
                                  Enum<gfx::TextureChannelDataType>::to(type), data));
}

void UploadPass::updateTextureResourceSub(gfx::TextureResource& resource,
                                          const uint16_t xOffset,
                                          const uint16_t yOffset,
                                          const Size size,
                                          const void* data,
                                          gfx::TexturePixelType format,
                                          gfx::TextureChannelDataType type) {
    // Always use texture unit 0 for manipulating it.
    commandEncoder.context.activeTextureUnit = 0;
    commandEncoder.context.texture[0] = static_cast<const gl::TextureResource&>(resource).texture;
    MBGL_CHECK_ERROR(glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, size.width, size.height,
                                     Enum<gfx::TexturePixelType>::to(format),
                                     Enum<gfx::TextureChannelDataType>::to(type), data));
}

void UploadPass::pushDebugGroup(const char* name) {
    commandEncoder.pushDebugGroup(name);
}

void UploadPass::popDebugGroup() {
    commandEncoder.popDebugGroup();
}

} // namespace gl
} // namespace mbgl
