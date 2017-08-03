#ifndef LRT_RREGISTRY_MOVEMENT_HPP
#define LRT_RREGISTRY_MOVEMENT_HPP

#include "Datatype.hpp"

namespace lrt
{
namespace rregistry
{
class Movement
{
public:
    enum class Properties {
	Speed,
	TurnAngle,
	TurnLength,

	__COUNT
    };

    Datatype Types[static_cast<property_type_t>(Properties::__COUNT) + 1] = {
	Datatype::FLOAT,   // Movement::Speed
	Datatype::DOUBLE,  // Movement::TurnAngle,
	Datatype::FLOAT,   // Movement::TurnLength

	Datatype::VOID     // __COUNT
    };
};
}
}

#endif
