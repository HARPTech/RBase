#ifndef LRT_RREGISTRY_MOVEMENT_HPP
#define LRT_RREGISTRY_MOVEMENT_HPP

#include "Datatype.hpp"

namespace lrt {
namespace rregistry {
class Movement
{
  public:
  enum Properties
  {
    Speed,
    TurnAngle,
    TurnLength,

    Direct_X_Axis,
    Direct_Y_Axis,

    __COUNT
  };

  Datatype TypesImpl[(size_t)Properties::__COUNT] = {
    Datatype::FLOAT, // Movement::Speed
    Datatype::DOUBLE,// Movement::TurnAngle,
    Datatype::FLOAT, // Movement::TurnLength

    Datatype::INT_32, // Movement::Direct_X_Axis
    Datatype::INT_32, // Movement::Direct_Y_Axis
  };

  static Datatype* Types;
};
}
}

#endif
