#include <RRegistry/Detail.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <memory>

#include "../include/RSupport/PipeAdapter.hpp"
#include "../include/RSupport/RSupport.h"

const char* RSupportStatusNames[RSupportStatus__Count] = {
  "Okay",
  "Could not open FIFOs",
  "Connection Failed",
  "Updates Received",
  "IO-Error",
  "FIFO not open for reading"
  "Other Error"
};

const char* RSupportDefaultFifoPath = "/tmp/lrt_pipe_path.pipe";;

const char*
rsupport_status_msg(RSupportStatus status)
{
  return RSupportStatusNames[static_cast<std::size_t>(status)];
}

RSupportHandle*
rsupport_handle_create()
{
  RSupportHandle* handle = new RSupportHandle();
  handle->registry = std::make_shared<lrt::rregistry::Registry>();
  handle->adapter =
    std::make_shared<lrt::rsupport::PipeAdapter>(handle->registry);

  handle->registry->registerAdapter(handle->adapter);
  return handle;
}

RSupportStatus
rsupport_handle_free(RSupportHandle* handle)
{
  handle->registry->removeAdapter(handle->adapter);
  delete handle;
}

RSupportStatus
rsupport_handle_service(RSupportHandle* handle)
{
  handle->adapter->service();
}

const char* getPipePath(const char *pipe) {
  if(pipe == 0) {
    // Receive pipe path.
    const char* env_path = std::getenv("LRT_PIPE_PATH");
    if(env_path == 0) {
      env_path = "/tmp/lrt_pipe_path.pipe";
    }
  }
  return pipe;
}

RSupportStatus
rsupport_handle_connect(RSupportHandle* handle, const char* pipe)
{
  pipe = getPipePath(pipe);
  handle->adapter->connect(pipe);
}

RSupportStatus
rsupport_handle_connect_create(RSupportHandle* handle, const char* pipe)
{
  pipe = getPipePath(pipe);
  handle->adapter->create(pipe);
}

RSupportStatus
rsupport_handle_disconnect(RSupportHandle* handle)
{
  handle->adapter->disconnect();
}

#define GETTER(CLASS)                                                        \
  typename lrt::rregistry::GetValueTypeOfEntryClass<lrt::rregistry::CLASS>:: \
    type rsupport_handle_get_##CLASS(RSupportHandle* handle,                 \
                                     uint16_t property)                      \
  {                                                                          \
    return handle->registry->get(                                            \
      static_cast<lrt::rregistry::CLASS>(property));                         \
  }

#define SETTER(CLASS)                                                   \
  void rsupport_handle_set_##CLASS(                                     \
    RSupportHandle* handle,                                             \
    uint16_t property,                                                  \
    typename lrt::rregistry::GetValueTypeOfEntryClass<                  \
      lrt::rregistry::CLASS>::type value)                               \
  {                                                                     \
    handle->registry->set(static_cast<lrt::rregistry::CLASS>(property), \
                          value);                                       \
  }

LRT_RREGISTRY_CPPTYPELIST_HELPER(GETTER)
LRT_RREGISTRY_CPPTYPELIST_HELPER(SETTER)

void
rsupport_handle_subscribe(RSupportHandle* handle,
                          uint16_t type,
                          uint16_t property)
{
  handle->adapter->subscribeByTypeVal(static_cast<lrt::rregistry::Type>(type),
                                      property);
}
void
rsupport_handle_unsubscribe(RSupportHandle* handle,
                            uint16_t type,
                            uint16_t property)
{
  handle->adapter->unsubscribeByTypeVal(static_cast<lrt::rregistry::Type>(type),
                                        property);
}
void
rsupport_handle_request(RSupportHandle* handle,
                        uint16_t type,
                        uint16_t property)
{
  handle->adapter->requestByTypeVal(static_cast<lrt::rregistry::Type>(type),
                                    property);
}

std::shared_ptr<lrt::rregistry::Registry>
rsupport_handle_get_registry(RSupportHandle* handle)
{
  return handle->registry;
}
