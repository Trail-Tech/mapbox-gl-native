#pragma once

#include <vector>
#include <utility>

namespace mbgl {
namespace style {

template <typename T>
class PropertyFunction {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    PropertyFunction(std::string property_, Stops stops_, float base_)
        : property(std::move(property_)), base(base_), stops(std::move(stops_)) {}

    const std::string& getProperty() const { return property; }
    float getBase() const { return base; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

private:
    std::string property;
    float base = 1;
    std::vector<std::pair<float, T>> stops;

    template <class S> friend bool operator==(const PropertyFunction<S>&, const PropertyFunction<S>&);
};

template <class T>
bool operator==(const PropertyFunction<T>& lhs, const PropertyFunction<T>& rhs) {
    return lhs.property == rhs.property && lhs.base == rhs.base && lhs.stops == rhs.stops;
}

template <class T>
bool operator!=(const PropertyFunction<T>& lhs, const PropertyFunction<T>& rhs) {
    return !(lhs == rhs);
}

} // namespace style
} // namespace mbgl
