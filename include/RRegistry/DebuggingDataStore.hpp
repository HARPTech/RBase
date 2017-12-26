#ifndef LRT_RREGISTRY_DEBUGGINGDATASTORE_HPP
#define LRT_RREGISTRY_DEBUGGINGDATASTORE_HPP

#include "Entries.hpp"
#include <RComm/LiteComm.hpp>

namespace lrt {
namespace rregistry {

/** @brief This class serves as an example on how to implement a receiver for
 * debugging notifications.
 *
 * This is the base class passed as a pointer to the Registry
 * class and the used function signatures can be copied at will.
 */
class DebuggingDataStore
{
  public:
  virtual void setOp(const char* adapter,
                     rregistry::Type type,
                     uint16_t property,
                     const rcomm::LiteCommData& lData,
                     int32_t posId) = 0;

  virtual void getOp(const char* adapter,
                     rregistry::Type type,
                     uint16_t property,
                     int32_t posId) = 0;
};
}
}

#endif
