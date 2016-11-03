#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/programs/attributes.hpp>

#include <cassert>

namespace mbgl {

enum class ProgramDefines : bool {
    None = false,
    Overdraw = true,
};

template <class S, class P, class L, class U, class Ps = style::PaintProperties<>>
class Program;

template <class Shaders,
          class Primitive,
          class LayoutAttrs,
          class Uniforms,
          class... Ps>
class Program<Shaders, Primitive, LayoutAttrs, Uniforms, style::PaintProperties<Ps...>> {
public:
    using LayoutAttributes = LayoutAttrs;
    using PaintAttributes = gl::Attributes<typename Ps::Attribute...>;
    using Attributes = gl::ConcatenateAttributes<LayoutAttributes, PaintAttributes>;

    using LayoutVertex = typename LayoutAttributes::Vertex;
    using PaintValues = typename PaintAttributes::Values;

    using ProgramType = gl::Program<Primitive, Attributes, Uniforms>;
    using UniformValues = typename ProgramType::UniformValues;

    ProgramType program;

    Program(gl::Context& context, ProgramDefines defines)
        : program(context, Shaders::vertexSource, fragmentSource(defines))
        {}

    static std::string fragmentSource(ProgramDefines defines) {
        std::string fragment = Shaders::fragmentSource;
        if (defines == ProgramDefines::Overdraw) {
            assert(fragment.find("#ifdef OVERDRAW_INSPECTOR") != std::string::npos);
            fragment.replace(fragment.find_first_of('\n'), 1, "\n#define OVERDRAW_INSPECTOR\n");
        }
        return fragment;
    }

    template <class AllProperties>
    static PaintValues paintPropertyValues(const AllProperties& properties) {
        return {
            Ps::Attribute::constantValue(properties.template get<Ps>())...
        };
    }

    template <class DrawMode, class AllPaintProperties>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              UniformValues&& uniformValues,
              const gl::VertexBuffer<LayoutVertex>& vertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const std::vector<gl::Segment>& segments,
              const AllPaintProperties& properties) {
        program.draw(
            context,
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            std::move(uniformValues),
            LayoutAttributes::allVariableValues()
                .concat(paintPropertyValues(properties)),
            vertexBuffer.buffer,
            indexBuffer.buffer,
            segments
        );
    }

    template <class DrawMode, class AllPaintProperties>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              UniformValues&& uniformValues,
              const gl::VertexBuffer<LayoutVertex, DrawMode>& vertexBuffer,
              const AllPaintProperties& properties) {
        program.draw(
            context,
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            std::move(uniformValues),
            LayoutAttributes::allVariableValues()
                .concat(paintPropertyValues(properties)),
            vertexBuffer.buffer,
            0,
            {{ 0, 0, vertexBuffer.vertexCount, 0 }}
        );
    }
};

} // namespace mbgl
