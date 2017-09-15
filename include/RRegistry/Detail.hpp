#ifndef LRT_RREGISTRY_DETAIL_HPP
#define LRT_RREGISTRY_DETAIL_HPP

#include "Entries.hpp"
#include "TypeConverter.hpp"
#include <string.h>

namespace lrt {
namespace rregistry {
enum class TypeTraits
{
  None = 0,
  Numeric = 1 << 0,
  Signed = 1 << 2,
  Unsigned = 1 << 3,
  Boolean = 1 << 4,
  FloatingPoint = 1 << 5,
  String = 1 << 6
};

constexpr inline TypeTraits
operator|(TypeTraits a, TypeTraits b)
{
  return TypeTraits(int(a) | int(b));
}
constexpr inline bool operator&(TypeTraits a, TypeTraits b)
{
  return bool(int(a) & int(b));
}

struct TypeDetail
{
  const char* name;
  const char* description;
  TypeTraits traits;
  Type type;
};

template<typename TypeCategory>
struct EntryDetail
{
  const char* name;
  const char* description;
  typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type def;
  typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type min;
  typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type max;
};
struct EntryDetailLite
{
  const char* name;
  const char* description;
};

static constexpr TypeDetail TypeDetail[static_cast<size_t>(Type::_COUNT)] = {
  { "Int8",
    "8 bit wide signed integer.",
    TypeTraits::Numeric | TypeTraits::Signed,
    Type::Int8 },
  { "Int16",
    "16 bit wide signed integer.",
    TypeTraits::Numeric | TypeTraits::Signed,
    Type::Int16 },
  { "Int32",
    "32 bit wide signed integer.",
    TypeTraits::Numeric | TypeTraits::Signed,
    Type::Int32 },
  { "Int64",
    "64 bit wide signed integer.",
    TypeTraits::Numeric | TypeTraits::Signed,
    Type::Int64 },
  { "Uint8",
    "8 bit wide unsigned integer.",
    TypeTraits::Numeric | TypeTraits::Unsigned,
    Type::Uint8 },
  { "Uint16",
    "16 bit wide unsigned integer.",
    TypeTraits::Numeric | TypeTraits::Unsigned,
    Type::Uint16 },
  { "Uint32",
    "32 bit wide unsigned integer.",
    TypeTraits::Numeric | TypeTraits::Unsigned,
    Type::Uint32 },
  { "Uint64",
    "64 bit wide unsigned integer.",
    TypeTraits::Numeric | TypeTraits::Unsigned,
    Type::Uint64 },
  { "Float",
    "32 bit wide floating point number.",
    TypeTraits::Numeric | TypeTraits::FloatingPoint | TypeTraits::Signed,
    Type::Float },
  { "Double",
    "64 bit wide floating point number.",
    TypeTraits::Numeric | TypeTraits::FloatingPoint | TypeTraits::Signed,
    Type::Double },
  { "Bool",
    "Binary true/false value.",
    TypeTraits::Numeric | TypeTraits::Boolean,
    Type::Bool },
  { "String",
    "String containing multiple letters.",
    TypeTraits::String,
    Type::String },
};

constexpr static const struct TypeDetail&
GetTypeDetail(Type type)
{
  return TypeDetail[static_cast<size_t>(type)];
}

#define LRT_RREGISTRY_ENTRYDETAIL_ARR(CLASS) \
  static constexpr EntryDetail<CLASS>        \
    CLASS##Detail[static_cast<size_t>(CLASS::_COUNT)]

LRT_RREGISTRY_ENTRYDETAIL_ARR(Int8) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Int16) = {
  { "MVMT_STEER_DIRECTION", "Direct steering direction.", 0, -32767, 32767 },
  { "MVMT_FORWARD_VELOCITY", "Direct movement velocity.", 0, -32767, 32767 },
};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Int32) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Int64) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint8) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint16) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint32) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint64) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Float) = {
  { "MVMT_SPEED_MULTIPLIER", "Factor for general movement speed.", 1, 0, 100 }
};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Double) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Bool) = {
  { "TEST_RBREAKOUT_COMMUNICATION",
    "Test the communication channel between RMaster and RBrreakout.",
    false,
    false,
    true },
  { "REDY_RBREAKOUT",
    "Mark RBreakout as ready, should be received from RBreakout.",
    false,
    false,
    true }
};
static constexpr EntryDetailLite
  StringDetail[static_cast<size_t>(String::_COUNT)] = {

  };

#define LRT_RREGISTRY_NAMECONVERTER_GENERATOR(CLASS)                 \
  CLASS static GetValueOfName##CLASS(const char* name)               \
  {                                                                  \
    for(size_t i = 0; i < static_cast<size_t>(CLASS::_COUNT); ++i) { \
      if(strcmp(name, CLASS##Detail[i].name) == 0)                   \
        return static_cast<CLASS>(i);                                \
    }                                                                \
    return CLASS::_COUNT;                                            \
  }

LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
  LRT_RREGISTRY_NAMECONVERTER_GENERATOR)

#define LRT_RREGISTRY_NAMECONVERTER_CASE(CLASS)                \
  case Type::CLASS:                                            \
    return static_cast<uint32_t>(GetValueOfName##CLASS(name)); \
    break;

int32_t static GetValueOfName(Type type, const char* name)
{
  switch(type) {
    LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
      LRT_RREGISTRY_NAMECONVERTER_CASE)
    default:
      return 0;
  }
}

#define LRT_RREGISTRY_GETNAMEOFENTRYCLASS_CASE(CLASS) \
  case Type::CLASS:                                   \
    return CLASS##Detail[property].name;

const char*
GetNameOfEntryClass(Type type, uint32_t property)
{
  switch(type) {
    LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
      LRT_RREGISTRY_GETNAMEOFENTRYCLASS_CASE)
    default:
      return "";
  }
}

template<typename TypeCategory>
constexpr const EntryDetail<TypeCategory>&
GetEntryDetail(TypeCategory property);

#define LRT_RREGISTRY_GETENTRYDETAIL_SPECIAL(CLASS)                   \
  template<>                                                          \
  constexpr const EntryDetail<lrt::rregistry::CLASS>& GetEntryDetail( \
    lrt::rregistry::CLASS property)                                   \
  {                                                                   \
    return CLASS##Detail[static_cast<size_t>(property)];              \
  }

LRT_RREGISTRY_CPPTYPELIST_HELPER(LRT_RREGISTRY_GETENTRYDETAIL_SPECIAL)
}
}

#endif
