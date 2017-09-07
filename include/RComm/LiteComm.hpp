#ifndef LRT_RCOMM_LITECOMM_HPP
#define LRT_RCOMM_LITECOMM_HPP

#include <stdint.h>

#include <RRegistry/TypeConverter.hpp>

namespace lrt {
namespace rcomm {

/**
 * @page LiteComm Lite Communication Framework
 * \tableofcontents
 *
 * The LiteComm Framework serves as a replacement of the standardized,
 * message-based communication format that serves between main components in
 * more constrained environments. It uses an Union to encompass all possible
 * types, which is transmitted directly over the communication channel using the
 * byte[] member.
 *
 * \section format Format
 *
 * The format of LiteComm-Messages is defined on this page. It defines the byte
 * order of the sent values.
 *
 * The first transmitted byte defines the transmission type. All possible
 * transmission types are listed below.
 *
 * \subsection transmission_type_update Update
 *
 * This type serves to update a value in the registry.
 *
 * | Byte-Number | Datatype     | Purpose                                   |
 * |-------------|--------------|-------------------------------------------|
 * | 0-1 | lrt::rcomm::LiteCommType | Transmission Type = LiteCommType::Update |
 * | 1-2 | lrt::rregistry::Type         | The registry type. |
 * | 2-6 | LiteCommProp | The property to change.                   |
 * | 6-14| LiteCommData | The new registry value.                   |
 *
 * \subsection transmission_type_request Request
 *
 * This type serves to request a value in the registry to be sent to the caller.
 *
 * | Byte-Number | Datatype     | Purpose                                   |
 * |-------------|--------------|-------------------------------------------|
 * | 0-1 | lrt::rcomm::LiteCommType | Transmission Type = LiteCommType::Request|
 * | 1-2 | lrt::rregistry::Type         | The registry type. |
 * | 2-6 | LiteCommProp | The requested property.                   |
 *
 * \subsection transmission_type_subscribe  Subscribe
 *
 * This type serves to subscribe to all changes of a value.
 *
 * | Byte-Number | Datatype     | Purpose                                   |
 * |-------------|--------------|-------------------------------------------|
 * | 0-1|lrt::rcomm::LiteCommType|Transmission Type = LiteCommType::Subscribe|
 * | 1-2 | lrt::rregistry::Type         | The registry type. |
 * | 2-6 | LiteCommProp | The property to subscribe to.                   |
 *
 * \subsection transmission_type_unsibscribe  Unsibscribe
 *
 * This type serves to unsubscribe to all changes of a value.
 *
 * | Byte-Number | Datatype     | Purpose                                   |
 * |-------------|--------------|-------------------------------------------|
 * | 0-1|lrt::rcomm::LiteCommType|Transmission Type = LiteCommType::Unsibscribe|
 * | 1-2 | lrt::rregistry::Type         | The registry type. |
 * | 2-6 | LiteCommProp | The property to unsubscribe from.                |
 *
 */
union LiteCommData
{
  char byte[8];
  int8_t Int8;
  int16_t Int16;
  int32_t Int32;
  int64_t Int64;

  uint8_t Uint8;
  uint16_t Uint16;
  uint32_t Uint32;
  uint64_t Uint64;

  float Float;
  double Double;

  bool Bool;

  template<typename TypeCategory>
  inline static void fromType(LiteCommData& data, TypeCategory value);
};

#define LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(TYPE)                          \
  template<>                                                                   \
  inline void LiteCommData::fromType(                                          \
    LiteCommData& data,                                                        \
    typename rregistry::GetValueTypeOfEntryClass<rregistry::TYPE>::type value) \
  {                                                                            \
    data.TYPE = value;                                                         \
  }

LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Int8)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Int16)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Int32)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Int64)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Uint8)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Uint16)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Uint32)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Uint64)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Float)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Double)
LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(Bool)

template<>
inline void
LiteCommData::fromType(
  LiteCommData& data,
  typename rregistry::GetValueTypeOfEntryClass<rregistry::String>::type value)
{
  data.Int32 = value.length();
}

enum class LiteCommType
{
  Update,
  Request,
  Subscribe,
  Unsubscribe,
};

union LiteCommProp
{
  uint32_t property;
  char byte[sizeof(property)];
};
}
}

#endif
