#ifndef LRT_RREGISTRY_OPERATIONS_HPP
#define LRT_RREGISTRY_OPERATIONS_HPP

#include "Datatype.hpp"

namespace lrt {
namespace rregistry {
class Operation
{
  public:
  enum Properties
  {
    ExecuteMovement,
    Request,

    Request_Category,
    Request_Property,

    __COUNT
  };

  Datatype TypesImpl[(size_t)Properties::__COUNT] = {
    Datatype::INT_8,// Operation::ExecuteMovement
    Datatype::INT_8,// Operation::Request

    Datatype::UINT_16,// Operation::Request_Category
    Datatype::UINT_16,// Operation::Request_Property
  };

  static Datatype* Types;
};
}
}

#endif
