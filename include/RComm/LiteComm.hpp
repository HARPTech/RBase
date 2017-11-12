#ifndef LRT_RCOMM_LITECOMM_HPP
#define LRT_RCOMM_LITECOMM_HPP

#include <stdint.h>
#include <string.h>

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
  char byte[8] = { 0 };
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
  inline static void fromType(LiteCommData& data,
                              TypeCategory value,
                              std::size_t&& i = 0);

#define LRT_RCOMM_LITECOMMDATA_FROMREGISTRYENTRY_CASE(TYPE)             \
  case rregistry::Type::TYPE:                                           \
    fromType(data, registry->get(static_cast<rregistry::TYPE>(entry))); \
    break;

  template<typename Registry>
  inline static void fromRegistryEntry(LiteCommData& data,
                                       rregistry::Type type,
                                       uint32_t entry,
                                       Registry registry)
  {
    switch(type) {
      LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
        LRT_RCOMM_LITECOMMDATA_FROMREGISTRYENTRY_CASE)
      default:
        break;
    }
  }
};

#define LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL(TYPE)                          \
  template<>                                                                   \
  inline void LiteCommData::fromType(                                          \
    LiteCommData& data,                                                        \
    typename rregistry::GetValueTypeOfEntryClass<rregistry::TYPE>::type value, \
    std::size_t&& i)                                                           \
  {                                                                            \
    data.TYPE = value;                                                         \
  }

LRT_RREGISTRY_CPPTYPELIST_HELPER(LRT_RCOMM_LITECOMMDATA_FROMTYPE_SPECIAL)

template<>
inline void
LiteCommData::fromType(
  LiteCommData& data,
  typename rregistry::GetValueTypeOfEntryClass<rregistry::String>::type value,
  std::size_t&& i)
{
  std::size_t strlength = 0;
#ifdef LRT_STRING_TYPE_STD
  strlength = value.length();
#else
  strlength = strlen(value);
#endif
  if(i == 0) {
    data.Int32 = strlength;
  } else {
    // The string should be written directly into the binary blob.
    // The (i - 1) is needed, because i = 0 stands for the size of the string to
    // be written into data.Int32.
    for(std::size_t n = 0; n < sizeof(LiteCommData); ++n, ++i)
      data.byte[n] = value[(i - 1)];
  }
}

#define LRT_RCOMM_SETLITECOMMDATATOREGISTRY_HELPER(CLASS)               \
  case rregistry::Type::CLASS:                                          \
    registry->set(static_cast<rregistry::CLASS>(property), data.CLASS); \
    break;

template<class Registry>
inline void
SetLiteCommDataToRegistry(rregistry::Type type,
                          uint32_t property,
                          const LiteCommData& data,
                          Registry registry)
{
  switch(type) {
    LRT_RREGISTRY_CPPTYPELIST_HELPER(LRT_RCOMM_SETLITECOMMDATATOREGISTRY_HELPER)
    case rregistry::Type::String:
      // This only gets called on the first run and does not append data. This
      // is why the string type is only its length in Int32.
      {
        auto str =
          registry->getPtrFromArray(static_cast<rregistry::String>(property));

#ifdef LRT_STRING_TYPE_STD
        str->assign(data.Int32, '\0');
#else
        if(str != nullptr)
          std::free(str);
        str = std::calloc(data.Int32, char);
#endif
      }
      break;
    default:
      break;
  }
}

enum class LiteCommType
{
  Update,
  Request,
  Subscribe,
  Unsubscribe,
  Append,
};

union LiteCommProp
{
  uint32_t property;
  char byte[sizeof(property)];
};
}
}

#endif
