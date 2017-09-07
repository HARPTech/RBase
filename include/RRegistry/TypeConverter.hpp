#ifndef LRT_RREGISTRY_TYPE_CONVERTER_HPP
#define LRT_RREGISTRY_TYPE_CONVERTER_HPP

#include "Entries.hpp"
#include <stdint.h>

#ifndef LRT_STRING_TYPE
#include <string>
#define LRT_STRING_TYPE std::string
#define LRT_STRING_TYPE_STD
#endif

namespace lrt {
namespace rregistry {
template<typename T>
struct GetValueTypeOfEntryClass;

template<typename TypeCategory>
constexpr Type
GetEnumTypeOfEntryClass(TypeCategory enumType);

#define LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(CLASS, TYPE) \
  template<>                                             \
  struct GetValueTypeOfEntryClass<CLASS>                 \
  {                                                      \
    typedef TYPE type;                                   \
  };                                                     \
  template<>                                             \
  constexpr Type GetEnumTypeOfEntryClass(CLASS)          \
  {                                                      \
    return Type::CLASS;                                  \
  }

LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Int8, int8_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Int16, int16_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Int32, int32_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Int64, int64_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Uint8, uint8_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Uint16, uint16_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Uint32, uint32_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Uint64, uint64_t)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Float, float)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Double, double)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Bool, bool)
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(String, LRT_STRING_TYPE)
}
}

#endif
