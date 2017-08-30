#ifndef LRT_RREGISTRY_TYPE_CONVERTER_HPP
#define LRT_RREGISTRY_TYPE_CONVERTER_HPP

#include "Entries.hpp"
#include <stdint.h>
#include <string>

namespace lrt {
namespace rregistry {
template<typename T>
struct GetValueTypeOfEntryClass;

#define LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(Class, Type) \
  template<>                                             \
  struct GetValueTypeOfEntryClass<Class>                 \
  {                                                      \
    typedef Type type;                                   \
  };

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
LRT_RREGISTRY_TYPE_CONVERTER_STRUCT(String, std::string)
}
}

#endif
