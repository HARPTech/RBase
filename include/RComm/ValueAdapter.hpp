#ifndef LRT_RCOMM_VALUEADAPTOR_HPP
#define LRT_RCOMM_VALUEADAPTOR_HPP

#include <stddef.h>

#include <value.capnp.h>

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
}
}

#endif
