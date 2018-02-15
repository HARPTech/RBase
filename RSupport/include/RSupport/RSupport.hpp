#ifndef LRT_RCOMM_RCOMM_H
#define LRT_RCOMM_RCOMM_H

/**
 * @file RSupport.hpp
 * @brief Functions and Classes for integrating a client Regulation Kernel into
 * RMaster as separate process.
 *
 * The C-API is usable by all languages, while the C++ handle is internal only
 * and also exposed with a SWIG bindings generator.
 */

typedef enum RSupportStatus {
  RSupportStatus_Ok,
  RSupportStatus_CouldNotOpenFIFOs,
  RSupportStatus_ConnectionFailed,
  RSupportStatus_Updates,
  RSupportStatus_IOError,
  RSupportStatus_FIFONotOpenForReading,
  RSupportStatus_OtherError,
  RSupportStatus_InvalidOption,
  RSupportStatus__Count
} RSupportStatus;

typedef enum RSupportOption {
  RSupportOption_AutoFrequency,
  RSupportOption_AutoMovementBurst,
  RSupportOption__Count
} RSupportOption;

#ifdef __cplusplus

// If C++ is used to compile the support library, the whole interface can be
// exposed securely.

namespace lrt {
namespace rsupport {
class PipeAdapter;
}
}

#include <RRegistry/Registry.hpp>
#include <bitset>
#include <chrono>
#include <memory>

/**
 * @brief The handle for a client or a server using the RSupport library.
 *
 * This handle holds all needed variables and can be used opaque to C++-isms
 * using the provided C-API.
 */
struct RSupportHandle
{
  std::shared_ptr<lrt::rregistry::Registry> registry;
  std::shared_ptr<lrt::rsupport::PipeAdapter> adapter;
  std::bitset<RSupportOption__Count> options;
  std::chrono::system_clock::time_point frameStart =
    std::chrono::system_clock::now();
  uint64_t frameTime = 0;
};

extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct RSupportHandle RSupportHandle;

/**
 * @brief The default fifo path to use. Should not be overridden by clients.
 */
extern const char* RSupportDefaultFifoPath;

/**
 * @brief Converts the given status enum value to a string representation.
 *
 * @param status The status to convert.
 * @return The name/description of the given status.
 */
const char*
rsupport_status_msg(RSupportStatus status);

/**
 * @brief Converts the given option enum value to a string representation.
 *
 * @param option The option to convert.
 * @return The name/description of the given option.
 */
const char*
rsupport_option_msg(RSupportOption option);

/**
 * @brief Creates the handle and initializes all internal fields.
 *
 * @return The created handle. Caller is responsible for freeing the pointer
 * using rsupport_handle_free().
 */
RSupportHandle*
rsupport_handle_create();

/**
 * @brief Frees the handle created with rsupport_handle_create().
 *
 * Internally also sets default options: RSupportOption_AutoFrequency and
 * RSupportOption_AutoMovementBurst are both set to true.
 *
 * @param handle The handle to free.
 * @return RSupportStatus_Ok on success.
 */
RSupportStatus
rsupport_handle_free(RSupportHandle* handle);

/**
 * @brief Connect the handle to the given pipe-path.
 *
 * @param handle The handle to initiate the connection in.
 * @param pipe The pipe to use. Can be an empty string or nullptr and will be
 * assigned to a default location.
 * @return RSupportStatus_Ok on success.
 */
RSupportStatus
rsupport_handle_connect(RSupportHandle* handle, const char* pipe);

/**
 * @brief Create the host (initial fifo creation).
 *
 * @param handle The handle to use in the creation of the host.
 * @param pipe The pipe to use. Can be an empty string or nullptr and will be
 * assigned to a default location.
 * @return RSupportStatus_Ok on success.
 */
RSupportStatus
rsupport_handle_connect_create(RSupportHandle* handle, const char* pipe);

/**
 * @brief Disconnect from the server in a clean and orderly way.
 *
 * @param handle The handle to use while disconnecting.
 * @return RSupportStatus_Ok on success.
 */
RSupportStatus
rsupport_handle_disconnect(RSupportHandle* handle);

/**
 * @brief Service the handle, get new updates from the host and apply activated
 * options.
 *
 * This function internally also detects the options
 * RSupportOption_AutoMovementBurst and RSupportOption_AutoFrequency and applies
 * the necessary actions, if they are active.
 *
 * @param handle The handle to service.
 * @return RSupportStatus_Ok or RSupportStatus_Updates on success.
 */
RSupportStatus
rsupport_handle_service(RSupportHandle* handle);

/**
 * @brief Sets the specified option applying to the behaviour of the RSupport
 * library to the given value.
 *
 * These options mainly apply to the rsupport_handle_service() function, which
 * changes according to active options on the given handle. Options can also be
 * changed during run-time.
 *
 * @param handle The handle to set the option on.
 * @param option The option to set.
 * @param state The value for the option to be set to.
 * @return RSupportStatus_Ok on success.
 */
RSupportStatus
rsupport_handle_set_option(RSupportHandle* handle,
                           RSupportOption option,
                           bool state);

/**
 * @brief Get the value of a given option in the specified handle.
 *
 * @param handle The handle to query.
 * @param option The option to check.
 * @return True if the option is active, false otherwise.
 */
bool
rsupport_handle_get_option(RSupportHandle* handle, RSupportOption option);

/**
 * @brief Get the time it took for the last frame to complete.
 *
 * @param handle The handle to query.
 * @return The time it took for the last frame to complete in nanoseconds.
 */
uint32_t
rsupport_handle_get_frame_time(RSupportHandle* handle);

// Registry Interface
// ==================

/**
 * @brief Subscribe to a registry value.
 *
 * @param handle The handle to use.
 * @param type The type of registry variable to subscribe.
 * @param property The property to subscribe to.
 */
void
rsupport_handle_subscribe(RSupportHandle* handle,
                          uint16_t type,
                          uint16_t property);

/**
 * @brief Unsubscribe from a registry value.
 *
 * @param handle The handle to use.
 * @param type The type of registry variable to unsubscribe from.
 * @param property The property to unsubscribe from.
 */
void
rsupport_handle_unsubscribe(RSupportHandle* handle,
                            uint16_t type,
                            uint16_t property);

/**
 * @brief Request a registry value.
 *
 * The requested variable will be in the local copy of the registry after the
 * next call to rsupport_handle_service().
 *
 * @param handle The handle to use.
 * @param type The type of registry variable to request.
 * @param property The property to request.
 */
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

namespace lrt {
namespace rsupport {

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

  /**
   * @brief Get the internal reference to the used RSupportHandle.
   * @return RSupportHandle The internal handle.
   */
  inline RSupportHandle* handle() { return m_handle.get(); }
  lrt::rregistry::Registry* registry() { return m_handle->registry.get(); }

  /**
   * @brief Forwards to rsupport_handle_connect().
   */
  inline RSupportStatus connect(const char* path)
  {
    return rsupport_handle_connect(handle(), path);
  }
  /**
   * @brief Forwards to rsupport_handle_create().
   */
  inline RSupportStatus create(const char* path)
  {
    return rsupport_handle_connect_create(handle(), path);
  }
  /**
   * @brief Forwards to rsupport_handle_disconnect().
   */
  inline RSupportStatus disconnect()
  {
    return rsupport_handle_disconnect(handle());
  }

  /**
   * @brief Forwards to rsupport_handle_request().
   */
  inline void request(uint16_t type, uint16_t property)
  {
    rsupport_handle_request(handle(), type, property);
  }
  /**
   * @brief Forwards to rsupport_handle_subscribe().
   */
  inline void subscribe(uint16_t type, uint16_t property)
  {
    rsupport_handle_subscribe(handle(), type, property);
  }
  /**
   * @brief Forwards to rsupport_handle_subscribe().
   */
  inline void unsubscribe(uint16_t type, uint16_t property)
  {
    rsupport_handle_unsubscribe(handle(), type, property);
  }
  /**
   * @brief Forwards to rsupport_handle_set_option().
   */
  inline RSupportStatus setOption(RSupportOption option, bool value)
  {
    return rsupport_handle_set_option(handle(), option, value);
  }
  /**
   * @brief Forwards to rsupport_handle_get_option().
   */
  inline bool getOption(RSupportOption option)
  {
    return rsupport_handle_get_option(handle(), option);
  }
  /**
   * @brief Forwards to rsupport_handle_get_frame_time().
   */
  inline uint32_t getFrameTime()
  {
    return rsupport_handle_get_frame_time(handle());
  }
  /**
   * @brief Forwards to rsupport_handle_service().
   */
  inline RSupportStatus service() { return rsupport_handle_service(handle()); }

  private:
  RSupportHandlePtr m_handle;
};
}
}

#endif

#endif
