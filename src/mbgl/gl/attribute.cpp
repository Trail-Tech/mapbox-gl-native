#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/normalization.hpp>

namespace mbgl {
namespace gl {

static_assert(offsetof(Normalized<uint8_t>, value) == 0, "unexpected normalized offset");

AttributeLocation bindAttributeLocation(ProgramID id, AttributeLocation location, const char* name) {
    MBGL_CHECK_ERROR(glBindAttribLocation(id, location, name));
    return location;
}

template <class T> DataType DataTypeOf    = static_cast<DataType>(0);
template <> DataType DataTypeOf< int8_t>  = DataType::Byte;
template <> DataType DataTypeOf<uint8_t>  = DataType::UnsignedByte;
template <> DataType DataTypeOf< int16_t> = DataType::Short;
template <> DataType DataTypeOf<uint16_t> = DataType::UnsignedShort;
template <> DataType DataTypeOf< int32_t> = DataType::Integer;
template <> DataType DataTypeOf<uint32_t> = DataType::UnsignedInteger;
template <> DataType DataTypeOf<float>    = DataType::Float;

template <class T> bool IsNormalized                  = false;
template <class T> bool IsNormalized<Normalized<T>>   = true;
template <class T> DataType DataTypeOf<Normalized<T>> = DataTypeOf<T>;

template <class T, std::size_t N>
void VariableAttributeValue<T, N>::bind(AttributeLocation location, std::size_t vertexOffset) const {
    MBGL_CHECK_ERROR(glEnableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttribPointer(
        location,
        static_cast<GLint>(N),
        static_cast<GLenum>(DataTypeOf<T>),
        static_cast<GLboolean>(IsNormalized<T>),
        static_cast<GLsizei>(vertexSize),
        reinterpret_cast<GLvoid*>(attributeOffset + (vertexSize * vertexOffset))));
}

template class VariableAttributeValue<uint8_t, 1>;
template class VariableAttributeValue<uint8_t, 2>;
template class VariableAttributeValue<uint8_t, 3>;
template class VariableAttributeValue<uint8_t, 4>;

template class VariableAttributeValue<Normalized<uint8_t>, 1>;
template class VariableAttributeValue<Normalized<uint8_t>, 2>;
template class VariableAttributeValue<Normalized<uint8_t>, 3>;
template class VariableAttributeValue<Normalized<uint8_t>, 4>;

template class VariableAttributeValue<uint16_t, 1>;
template class VariableAttributeValue<uint16_t, 2>;
template class VariableAttributeValue<uint16_t, 3>;
template class VariableAttributeValue<uint16_t, 4>;

template class VariableAttributeValue<int16_t, 1>;
template class VariableAttributeValue<int16_t, 2>;
template class VariableAttributeValue<int16_t, 3>;
template class VariableAttributeValue<int16_t, 4>;

template class VariableAttributeValue<float, 1>;
template class VariableAttributeValue<float, 2>;
template class VariableAttributeValue<float, 3>;
template class VariableAttributeValue<float, 4>;

template <>
void ConstantAttributeValue<uint8_t, 1>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1s(location, value[0]));
}

template <>
void ConstantAttributeValue<uint8_t, 2>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2s(location, value[0], value[1]));
}

template <>
void ConstantAttributeValue<uint8_t, 3>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3s(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeValue<uint8_t, 4>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4s(location, value[0], value[1], value[2], value[3]));
}


template <>
void ConstantAttributeValue<Normalized<uint8_t>, 1>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4Nub(location, value[0].value, 0, 0, 0));
}

template <>
void ConstantAttributeValue<Normalized<uint8_t>, 2>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4Nub(location, value[0].value, value[1].value, 0, 0));
}

template <>
void ConstantAttributeValue<Normalized<uint8_t>, 3>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4Nub(location, value[0].value, value[1].value, value[2].value, 0));
}

template <>
void ConstantAttributeValue<Normalized<uint8_t>, 4>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4Nub(location, value[0].value, value[1].value, value[2].value, value[3].value));
}


template <>
void ConstantAttributeValue<uint16_t, 1>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1f(location, value[0]));
}

template <>
void ConstantAttributeValue<uint16_t, 2>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2f(location, value[0], value[1]));
}

template <>
void ConstantAttributeValue<uint16_t, 3>::bind(AttributeLocation location, std::size_t) const {
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3f(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeValue<uint16_t, 4>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4f(location, value[0], value[1], value[2], value[3]));
}


template <>
void ConstantAttributeValue<int16_t, 1>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1f(location, value[0]));
}

template <>
void ConstantAttributeValue<int16_t, 2>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2f(location, value[0], value[1]));
}

template <>
void ConstantAttributeValue<int16_t, 3>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3f(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeValue<int16_t, 4>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4f(location, value[0], value[1], value[2], value[3]));
}


template <>
void ConstantAttributeValue<float, 1>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib1f(location, value[0]));
}

template <>
void ConstantAttributeValue<float, 2>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib2f(location, value[0], value[1]));
}

template <>
void ConstantAttributeValue<float, 3>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib3f(location, value[0], value[1], value[2]));
}

template <>
void ConstantAttributeValue<float, 4>::bind(AttributeLocation location, std::size_t) const {
    assert(location != 0);
    MBGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    MBGL_CHECK_ERROR(glVertexAttrib4f(location, value[0], value[1], value[2], value[3]));
}

} // namespace gl
} // namespace mbgl
