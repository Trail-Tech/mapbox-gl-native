#pragma once

#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/normalization.hpp>
#include <mbgl/style/paint_property.hpp>

#include <cstdint>

namespace mbgl {
namespace attributes {

// Attributes common to several shaders.

MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_pos);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_offset);
MBGL_DEFINE_ATTRIBUTE(int16_t, 2, a_extrude);
MBGL_DEFINE_ATTRIBUTE(uint16_t, 2, a_texture_pos);

template <std::size_t N>
struct a_data : gl::Attribute<a_data<N>, uint8_t, N> {
    static constexpr auto name = "a_data";
};

// Paint attributes

struct a_color : gl::Attribute<a_color, gl::Normalized<uint8_t>, 4> {
    static constexpr auto name = "a_color";

    static ConstantValue constantValue(const Color& color) {
        return {{
            gl::Normalized<uint8_t>(color.r),
            gl::Normalized<uint8_t>(color.g),
            gl::Normalized<uint8_t>(color.b),
            gl::Normalized<uint8_t>(color.a)
        }};
    }
};

struct a_outline_color : gl::Attribute<a_outline_color, gl::Normalized<uint8_t>, 4> {
    static constexpr auto name = "a_outline_color";

    static ConstantValue constantValue(const Color& color) {
        return {{
            gl::Normalized<uint8_t>(color.r),
            gl::Normalized<uint8_t>(color.g),
            gl::Normalized<uint8_t>(color.b),
            gl::Normalized<uint8_t>(color.a)
        }};
    }
};

struct a_opacity : gl::Attribute<a_opacity, gl::Normalized<uint8_t>, 1> {
    static constexpr auto name = "a_opacity";

    static ConstantValue constantValue(float opacity) {
        return {{ gl::Normalized<uint8_t>(opacity) }};
    }
};

struct a_blur : gl::Attribute<a_blur, float, 1> {
    static constexpr auto name = "a_blur";

    static ConstantValue constantValue(float blur) {
        return {{ blur }};
    }
};

struct a_radius : gl::Attribute<a_radius, float, 1> {
    static constexpr auto name = "a_radius";

    static ConstantValue constantValue(float radius) {
        return {{ radius }};
    }
};

} // namespace attributes
} // namespace mbgl
