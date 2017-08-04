#ifndef LRT_RREGISTRY_OPERATIONS_HPP
#define LRT_RREGISTRY_OPERATIONS_HPP

#include "Datatype.hpp"

namespace lrt {
namespace rregistry {
class Operation
{
  public:
  enum class Properties
  {
    ExecuteMovement,

    __COUNT
  };

  static constexpr Datatype
    Types[static_cast<property_type_t>(Properties::__COUNT) + 1] = {
      Datatype::VOID,// Operation::ExecuteMovement

      Datatype::VOID// __COUNT
    };
};
}
}

#endif
