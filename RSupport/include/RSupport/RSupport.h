#ifndef LRT_RCOMM_RCOMM_H
#define LRT_RCOMM_RCOMM_H

#ifdef __cplusplus

// If C++ is used to compile the support library, the whole interface can be
// exposed securely.

namespace lrt {
namespace rsupport {
class PipeAdapter;
}
}

#include <RRegistry/Registry.hpp>
struct RSupportHandle
{
  std::shared_ptr<lrt::rregistry::Registry> registry;
  std::shared_ptr<lrt::rsupport::PipeAdapter> adapter;
};

extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct RSupportHandle RSupportHandle;

typedef enum RSupportStatus {
  RSupportStatus_Ok,
  RSupportStatus_CouldNotOpenFIFOs,
  RSupportStatus_ConnectionFailed,
  RSupportStatus_Updates,
  RSupportStatus_IOError,
  RSupportStatus_FIFONotOpenForReading,
  RSupportStatus_OtherError,
  RSupportStatus__Count
} RSupportStatus;

const char*
rsupport_status_msg(RSupportStatus status);

RSupportHandle*
rsupport_handle_create();

RSupportStatus
rsupport_handle_free(RSupportHandle* handle);

RSupportStatus
rsupport_handle_connect(RSupportHandle* handle, const char* pipe);

RSupportStatus
rsupport_handle_connect_create(RSupportHandle* handle, const char* pipe);

RSupportStatus
rsupport_handle_disconnect(RSupportHandle* handle);

RSupportStatus
rsupport_handle_service(RSupportHandle* handle);

// Registry Interface
// ==================

void
rsupport_handle_subscribe(RSupportHandle* handle,
                          uint16_t type,
                          uint16_t property);
void
rsupport_handle_unsubscribe(RSupportHandle* handle,
                            uint16_t type,
                            uint16_t property);
void
rsupport_handle_request(RSupportHandle* handle,
                        uint16_t type,
                        uint16_t property);

int8_t
rsupport_handle_get_Int8(RSupportHandle* handle, uint16_t property);
int16_t
rsupport_handle_get_Int16(RSupportHandle* handle, uint16_t property);
int32_t
rsupport_handle_get_Int32(RSupportHandle* handle, uint16_t property);
int64_t
rsupport_handle_get_Int64(RSupportHandle* handle, uint16_t property);
uint8_t
rsupport_handle_get_Uint8(RSupportHandle* handle, uint16_t property);
uint16_t
rsupport_handle_get_Uint16(RSupportHandle* handle, uint16_t property);
uint32_t
rsupport_handle_get_Uint32(RSupportHandle* handle, uint16_t property);
uint64_t
rsupport_handle_get_Uint64(RSupportHandle* handle, uint16_t property);
float
rsupport_handle_get_Float(RSupportHandle* handle, uint16_t property);
double
rsupport_handle_get_Double(RSupportHandle* handle, uint16_t property);
bool
rsupport_handle_get_Bool(RSupportHandle* handle, uint16_t property);

void
rsupport_handle_set_Int8(RSupportHandle* handle,
                         uint16_t property,
                         int8_t value);
void
rsupport_handle_set_Int16(RSupportHandle* handle,
                          uint16_t property,
                          int16_t value);
void
rsupport_handle_set_Int32(RSupportHandle* handle,
                          uint16_t property,
                          int32_t value);
void
rsupport_handle_set_Int64(RSupportHandle* handle,
                          uint16_t property,
                          int64_t value);
void
rsupport_handle_set_Uint8(RSupportHandle* handle,
                          uint16_t property,
                          uint8_t value);
void
rsupport_handle_set_Uint16(RSupportHandle* handle,
                           uint16_t property,
                           uint16_t value);
void
rsupport_handle_set_Uint32(RSupportHandle* handle,
                           uint16_t property,
                           uint32_t value);
void
rsupport_handle_set_Uint64(RSupportHandle* handle,
                           uint16_t property,
                           uint64_t value);
void
rsupport_handle_set_Float(RSupportHandle* handle,
                          uint16_t property,
                          float value);
void
rsupport_handle_set_Double(RSupportHandle* handle,
                           uint16_t property,
                           double value);
void
rsupport_handle_set_Bool(RSupportHandle* handle, uint16_t property, bool value);

#ifdef __cplusplus
}
#include <RRegistry/Registry.hpp>
#include <memory>

std::shared_ptr<lrt::rregistry::Registry>
rsupport_handle_get_registry(RSupportHandle* handle);

/**
 * @brief Utility binding if SWIG is used.
 *
 * Just a wrapper around the C API with safe memory deletion, which makes better
 * APIs through SWIG possible.
 */
class RSupport
{
  public:
  typedef std::unique_ptr<RSupportHandle, RSupportStatus (*)(RSupportHandle*)>
    RSupportHandlePtr;

  RSupport()
    : m_handle(rsupport_handle_create(), &rsupport_handle_free)
  {
  }
  ~RSupport() {}

  inline RSupportHandle* handle() { return m_handle.get(); }
  lrt::rregistry::Registry* registry() { return m_handle->registry.get(); }

  inline RSupportStatus connect(const char* path)
  {
    return rsupport_handle_connect(handle(), path);
  }
  inline RSupportStatus create(const char* path)
  {
    return rsupport_handle_connect_create(handle(), path);
  }
  inline RSupportStatus disconnect()
  {
    return rsupport_handle_disconnect(handle());
  }

  inline void request(uint16_t type, uint16_t property)
  {
    rsupport_handle_request(handle(), type, property);
  }
  inline void subscribe(uint16_t type, uint16_t property)
  {
    rsupport_handle_subscribe(handle(), type, property);
  }
  inline void unsubscribe(uint16_t type, uint16_t property)
  {
    rsupport_handle_unsubscribe(handle(), type, property);
  }
  inline RSupportStatus service() { return rsupport_handle_service(handle()); }

  private:
  RSupportHandlePtr m_handle;
};

#endif

#endif
