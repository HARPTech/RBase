#ifndef LRT_RREGISTRY_RREGISTRY_HPP
#define LRT_RREGISTRY_RREGISTRY_HPP

#include "Categories.hpp"
#include "Datatype.hpp"
#include "Movement.hpp"
#include "Operation.hpp"
#include "System.hpp"

namespace lrt {
namespace rregistry {
inline Datatype
GetDatatypeForProperty(category_type_t category, property_type_t property)
{
  switch(static_cast<Categories>(category)) {
    case Categories::System:
      if(property > static_cast<size_t>(System::Properties::__COUNT))
        return Datatype::VOID;
      return System::Types[property];
    case Categories::Movement:
      if(property > static_cast<size_t>(Movement::Properties::__COUNT))
        return Datatype::VOID;
      return Movement::Types[property];
    case Categories::Operation:
      if(property > static_cast<size_t>(Operation::Properties::__COUNT))
        return Datatype::VOID;
      return Operation::Types[property];
  }
  return Datatype::VOID;
}
}
}

#endif
