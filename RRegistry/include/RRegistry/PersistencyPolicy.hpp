#ifndef LRT_RREGISTRY_CONSISTENCYPOLICY_HPP
#define LRT_RREGISTRY_CONSISTENCYPOLICY_HPP

#include "Entries.hpp"
#include <RComm/LiteComm.hpp>
#include <cstdint>
#include <memory>

namespace lrt {
namespace rregistry {

class PersistencyPolicy
{
  public:
  PersistencyPolicy() {}
  virtual ~PersistencyPolicy() {}

  virtual void push(uint16_t clientId,
                    rregistry::Type type,
                    uint16_t property,
                    rcomm::LiteCommData data)
  {
  }

  virtual void enable(const std::string &dbFile) {}
  virtual void disable() {}
};

using PersistencyPolicyPtr = std::unique_ptr<PersistencyPolicy>;
}
}

#endif
