#ifndef LRT_RREGISTRY_SYSTEM_HPP
#define LRT_RREGISTRY_SYSTEM_HPP

#include "Datatype.hpp"

namespace lrt {
namespace rregistry {
class System
{
  public:
  enum Properties
  {
    TemperatureCore,

    __COUNT
  };

  Datatype TypesImpl[(size_t)Properties::__COUNT] = {
    Datatype::INT_16// TemperatureCore
  };

  static Datatype* Types;
};
}
}

#endif
