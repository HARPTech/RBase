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
    RBreakout_Version,

    __COUNT
  };

  Datatype TypesImpl[(size_t)Properties::__COUNT] = {
    Datatype::STRING // System::RBreakout_Version
  };

  static Datatype* Types;
};
}
}

#endif
