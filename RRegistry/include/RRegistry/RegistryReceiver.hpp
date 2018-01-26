#ifndef LRT_RREGISTRY_REGISTRYRECEIVER_HPP
#define LRT_RREGISTRY_REGISTRYRECEIVER_HPP

#include "Entries.hpp"
#include <cstdint>

namespace lrt {
namespace rregistry {
class RegistryReceiver
{
  public:
  virtual void onUpdate(Type type, uint32_t property) = 0;
};
}
}

#endif
