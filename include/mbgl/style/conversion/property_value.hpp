#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<PropertyValue<T>> {
    template <class V>
    Result<PropertyValue<T>> operator()(const V& value) const {
        if (isUndefined(value)) {
            return {};
        } else if (!isObject(value)) {
            Result<T> constant = convert<T>(value);
            if (!constant) {
                return constant.error();
            }
            return *constant;
        } else {
            Result<ZoomFunction<T>> function = convert<ZoomFunction<T>>(value);
            if (!function) {
                return function.error();
            }
            return *function;
        }
    }
};

template <class T>
struct Converter<DataDrivenPropertyValue<T>> {
    template <class V>
    Result<DataDrivenPropertyValue<T>> operator()(const V& value) const {
        if (isUndefined(value)) {
            return {};
        } else if (!isObject(value)) {
            Result<T> constant = convert<T>(value);
            if (!constant) {
                return constant.error();
            }
            return *constant;
        } else if (!objectMember(value, "property")) {
            Result<ZoomFunction<T>> function = convert<ZoomFunction<T>>(value);
            if (!function) {
                return function.error();
            }
            return *function;
        } else {
            Result<PropertyFunction<T>> function = convert<PropertyFunction<T>>(value);
            if (!function) {
                return function.error();
            }
            return *function;
        }
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
