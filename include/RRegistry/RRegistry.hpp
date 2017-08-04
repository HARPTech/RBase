#ifndef LRT_RREGISTRY_RREGISTRY_HPP
#define LRT_RREGISTRY_RREGISTRY_HPP

#include "Categories.hpp"
#include "Datatype.hpp"
#include "Movement.hpp"
#include "Operation.hpp"
#include "System.hpp"

// The definition below has to be inserted ONCE into a source file. This file
// defines all global RRegistry classes to provide static members, without
// needing additional internal source files and while seperating each category
// in their own header files.
#define LRT_RREGISTRY_DEFINE_STATICS                           \
  lrt::rregistry::System lrt_rregistry_system_impl;            \
  lrt::rregistry::Datatype* lrt::rregistry::System::Types =    \
    lrt_rregistry_system_impl.TypesImpl;                       \
  lrt::rregistry::Movement lrt_rregistry_movement_impl;        \
  lrt::rregistry::Datatype* lrt::rregistry::Movement::Types =  \
    lrt_rregistry_movement_impl.TypesImpl;                     \
  lrt::rregistry::Operation lrt_rregistry_operation_impl;      \
  lrt::rregistry::Datatype* lrt::rregistry::Operation::Types = \
    lrt_rregistry_operation_impl.TypesImpl;

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
