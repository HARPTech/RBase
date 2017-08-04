#ifndef LRT_RREGISTRY_PROPERTY_HPP
#define LRT_RREGISTRY_PROPERTY_HPP

#include "Datatype.hpp"

#define LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(TYPE, INTERNAL, PROPERTY) \
  bool get(TYPE* v)                                                   \
  {                                                                   \
    if(type == Datatype::INTERNAL) {                                  \
      *v = value.PROPERTY;                                            \
      return true;                                                    \
    }                                                                 \
    return false;                                                     \
  }

namespace lrt {
namespace rregistry {
class Property
{
  public:
  Property() {}
  ~Property() {}

  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(int8_t, INT_8, Int8)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(int16_t, INT_16, Int16)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(int32_t, INT_32, Int32)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(int64_t, INT_64, Int64)

  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(uint8_t, UINT_8, UInt8)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(uint16_t, UINT_16, UInt16)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(uint32_t, UINT_32, UInt32)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(uint64_t, UINT_64, UInt64)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(float, FLOAT, Float)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(double, DOUBLE, Double)
  LRT_RREGISTRY_PROPERTY_GET_OVERLOAD(bool, BOOLEAN, Bool)

  property_value_t value;
  Datatype type;
};
}
}

#endif
