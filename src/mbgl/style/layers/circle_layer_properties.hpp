// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/programs/attributes.hpp>

namespace mbgl {
namespace style {

struct CircleRadius : PaintProperty<float> {
    static float defaultValue() { return 5; }
    using Attribute = attributes::a_radius;
};

struct CircleColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
    using Attribute = attributes::a_color;
};

struct CircleBlur : PaintProperty<float> {
    static float defaultValue() { return 0; }
    using Attribute = attributes::a_blur;
};

struct CircleOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
    using Attribute = attributes::a_opacity;
};

struct CircleTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct CircleTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct CirclePitchScale : PaintProperty<CirclePitchScaleType> {
    static CirclePitchScaleType defaultValue() { return CirclePitchScaleType::Map; }
};

class CirclePaintProperties : public PaintProperties<
    CircleRadius,
    CircleColor,
    CircleBlur,
    CircleOpacity,
    CircleTranslate,
    CircleTranslateAnchor,
    CirclePitchScale
> {};

} // namespace style
} // namespace mbgl
