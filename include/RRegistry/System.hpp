#ifndef LRT_RREGISTRY_SYSTEM_HPP
#define LRT_RREGISTRY_SYSTEM_HPP

#include "Datatype.hpp"

namespace lrt
{
namespace rregistry
{
class System
{
public:
    enum class Properties {
	__COUNT
    };

    Datatype Types[static_cast<property_type_t>(Properties::__COUNT) + 1] = {
	Datatype::VOID     // __COUNT
    };
};
}
}

#endif
