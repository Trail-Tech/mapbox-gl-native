#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/function/zoom_function.hpp>
#include <mbgl/style/function/property_function.hpp>

namespace mbgl {
namespace style {

class Undefined {};

inline bool operator==(const Undefined&, const Undefined&) { return true; }
inline bool operator!=(const Undefined&, const Undefined&) { return false; }

template <class T>
class PropertyValue {
private:
    using Value = variant<Undefined, T, ZoomFunction<T>>;

    Value value;

    template <class S> friend bool operator==(const PropertyValue<S>&, const PropertyValue<S>&);

public:
    using EvaluatedType = T;

    PropertyValue()                             : value()         {}
    PropertyValue(             T  constant) : value(constant) {}
    PropertyValue(ZoomFunction<T> function) : value(function) {}

    bool isUndefined()         const { return value.which() == 0; }
    bool isConstant()          const { return value.which() == 1; }
    bool isZoomFunction()      const { return value.which() == 2; }

    const              T &     asConstant() const { return value.template get<             T >(); }
    const ZoomFunction<T>& asZoomFunction() const { return value.template get<ZoomFunction<T>>(); }

    explicit operator bool() const { return !isUndefined(); };

    template <typename Evaluator>
    auto evaluate(const Evaluator& evaluator) const {
        return Value::visit(value, evaluator);
    }
};

template <class T>
bool operator==(const PropertyValue<T>& lhs, const PropertyValue<T>& rhs) {
    return lhs.value == rhs.value;
}

template <class T>
bool operator!=(const PropertyValue<T>& lhs, const PropertyValue<T>& rhs) {
    return !(lhs == rhs);
}

template <class T>
class DataDrivenPropertyValue {
private:
    using Value = variant<
        Undefined,
        T,
        ZoomFunction<T>,
        PropertyFunction<T>>;

    Value value;

    template <class S> friend bool operator==(const DataDrivenPropertyValue<S>&, const DataDrivenPropertyValue<S>&);

public:
    using EvaluatedType = T;

    DataDrivenPropertyValue()                             : value()         {}
    DataDrivenPropertyValue(                 T  constant) : value(constant) {}
    DataDrivenPropertyValue(    ZoomFunction<T> function) : value(function) {}
    DataDrivenPropertyValue(PropertyFunction<T> function) : value(function) {}

    bool isUndefined()         const { return value.which() == 0; }
    bool isConstant()          const { return value.which() == 1; }
    bool isZoomFunction()      const { return value.which() == 2; }
    bool isPropertyFunction()  const { return value.which() == 3; }

    const                  T &         asConstant() const { return value.template get<                 T >(); }
    const     ZoomFunction<T>&     asZoomFunction() const { return value.template get<    ZoomFunction<T>>(); }
    const PropertyFunction<T>& asPropertyFunction() const { return value.template get<PropertyFunction<T>>(); }

    explicit operator bool() const { return !isUndefined(); };

    template <typename Evaluator>
    auto evaluate(const Evaluator& evaluator) const {
        return Value::visit(value, evaluator);
    }
};

template <class T>
bool operator==(const DataDrivenPropertyValue<T>& lhs, const DataDrivenPropertyValue<T>& rhs) {
    return lhs.value == rhs.value;
}

template <class T>
bool operator!=(const DataDrivenPropertyValue<T>& lhs, const DataDrivenPropertyValue<T>& rhs) {
    return !(lhs == rhs);
}

} // namespace style
} // namespace mbgl
