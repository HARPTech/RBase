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

#define LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                           \
  CLASS, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA)                      \
  case lrt::rregistry::Type::CLASS:                                            \
    TEMPLATE_FUNC(static_cast<lrt::rregistry::CLASS>(PROPERTY_VAR), USERDATA); \
    break;

#define LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER(                 \
  TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA)              \
  switch(TYPE_VAR) {                                            \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Int8, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA);   \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Int16, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA);  \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Int32, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA);  \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Int64, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA);  \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Uint8, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA);  \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Uint16, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA); \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Uint32, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA); \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Uint64, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA); \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Float, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA);  \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Double, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA); \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      Bool, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA);   \
    LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER_CASE(                \
      String, TYPE_VAR, PROPERTY_VAR, TEMPLATE_FUNC, USERDATA); \
    default:                                                    \
      break;                                                    \
  }

#define LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(CPP_FUNC) \
  CPP_FUNC(Int8)                                   \
  CPP_FUNC(Int16)                                  \
  CPP_FUNC(Int32)                                  \
  CPP_FUNC(Int64)                                  \
  CPP_FUNC(Uint8)                                  \
  CPP_FUNC(Uint16)                                 \
  CPP_FUNC(Uint32)                                 \
  CPP_FUNC(Uint64)                                 \
  CPP_FUNC(Float)                                  \
  CPP_FUNC(Double)                                 \
  CPP_FUNC(Bool)                                   \
  CPP_FUNC(String)

#define LRT_RREGISTRY_CPPTYPELIST_HELPER(CPP_FUNC) \
  CPP_FUNC(Int8)                                   \
  CPP_FUNC(Int16)                                  \
  CPP_FUNC(Int32)                                  \
  CPP_FUNC(Int64)                                  \
  CPP_FUNC(Uint8)                                  \
  CPP_FUNC(Uint16)                                 \
  CPP_FUNC(Uint32)                                 \
  CPP_FUNC(Uint64)                                 \
  CPP_FUNC(Float)                                  \
  CPP_FUNC(Double)                                 \
  CPP_FUNC(Bool)
}
}

#endif
