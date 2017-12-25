#ifndef LRT_RCOMM_VALUEADAPTOR_HPP
#define LRT_RCOMM_VALUEADAPTOR_HPP

#include <stddef.h>

#include <position.capnp.h>
#include <value.capnp.h>

#include <RRegistry/TypeConverter.hpp>

namespace lrt {
namespace rcomm {
template<typename ValueType>
void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, ValueType value);

template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, int8_t value)
{
  container.setInt8(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, int16_t value)
{
  container.setInt16(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, int32_t value)
{
  container.setInt32(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, int64_t value)
{
  container.setInt64(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, uint8_t value)
{
  container.setUint8(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, uint16_t value)
{
  container.setUint16(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, uint32_t value)
{
  container.setUint32(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, uint64_t value)
{
  container.setUint64(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, float value)
{
  container.setFloat(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, double value)
{
  container.setDouble(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container, bool value)
{
  container.setBool(value);
}
template<>
inline void
SetRegistryValue(rcomm::RegistryValue::Builder&& container,
                 const std::string& value)
{
  container.setString(value);
}

template<typename TypeCategory>
inline void
SetRegistryPosition(rcomm::RegistryPosition::Builder& container,
                    TypeCategory property);

#define LRT_RREGISTRY_VALUEADAPTER_POSITION(CLASS)                             \
  template<>                                                                   \
  inline void SetRegistryPosition(rcomm::RegistryPosition::Builder& container, \
                                  rregistry::CLASS property)                   \
  {                                                                            \
    container.getType().set##CLASS();                                          \
    container.setProperty(static_cast<uint32_t>(property));                    \
  }

LRT_RREGISTRY_VALUEADAPTER_POSITION(Int8)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Int16)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Int32)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Int64)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Uint8)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Uint16)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Uint32)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Uint64)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Float)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Double)
LRT_RREGISTRY_VALUEADAPTER_POSITION(Bool)
LRT_RREGISTRY_VALUEADAPTER_POSITION(String)
}
}

#endif
