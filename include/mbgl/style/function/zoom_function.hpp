#pragma once

#include <vector>
#include <utility>

namespace mbgl {
namespace style {

template <typename T>
class ZoomFunction {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    ZoomFunction(Stops stops_, float base_)
        : base(base_), stops(std::move(stops_)) {}

    float getBase() const { return base; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

private:
    float base = 1;
    std::vector<std::pair<float, T>> stops;

    template <class S> friend bool operator==(const ZoomFunction<S>&, const ZoomFunction<S>&);
};

template <class T>
bool operator==(const ZoomFunction<T>& lhs, const ZoomFunction<T>& rhs) {
    return lhs.base == rhs.base && lhs.stops == rhs.stops;
}

template <class T>
bool operator!=(const ZoomFunction<T>& lhs, const ZoomFunction<T>& rhs) {
    return !(lhs == rhs);
}

} // namespace style
} // namespace mbgl
