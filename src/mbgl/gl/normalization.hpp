#pragma once

#include <mbgl/math/clamp.hpp>

#include <cassert>
#include <limits>

namespace mbgl {
namespace gl {

template <class T>
class Normalized {
public:
    T value;

    explicit Normalized(float f)
        : value(static_cast<uint8_t>(std::numeric_limits<uint8_t>::max() * util::clamp(f, 0.0f, 1.0f))) {
        assert(f >= 0.0f);
        assert(f <= 1.0f);
    }
};

} // namespace gl
} // namespace mbgl
