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
    Speed,    ///< The Speed modifier multiplies every movement action. Default
              /// Value: 1.
    TurnAngle,///< The TurnAngle is the angle of the arc the rover should follow
              /// in degrees.
    TurnLength,   ///< The TurnLength is the length of the arc the rover should
                  /// follow in meters.
    Direct_X_Axis,///< This X-Axis represents the X-component of the direct
                  /// movement vector of the rover. It could be mapped to a
                  /// joystick. This value is on a 1-1000 scale.
    Direct_Y_Axis,///< This Y-Axis represents the Y-component of the direct
                  /// movement vector of the rover. It could be mapped to a
                  /// joystick. This value is on a 1-1000 scale.
    __COUNT       ///< The number of properties in this category.
  };

  Datatype TypesImpl[(size_t)Properties::__COUNT] = {
    Datatype::FLOAT, // Movement::Speed
    Datatype::DOUBLE,// Movement::TurnAngle,
    Datatype::FLOAT, // Movement::TurnLength

    Datatype::INT_8,// Movement::Direct_X_Axis
    Datatype::INT_8,// Movement::Direct_Y_Axis
  };

  static Datatype* Types;
};
}
}

#endif
