#include <RRegistry/Detail.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <memory>
#include <thread>

#include "../include/RSupport/ConsoleAdapter.hpp"
#include "../include/RSupport/PipeAdapter.hpp"
#include "../include/RSupport/RSupport.hpp"
#include <RRegistry/Entries.hpp>

const char* RSupportStatusNames[RSupportStatus__Count] = {
  "Okay",
  "Could not open FIFOs",
  "Connection Failed",
  "Updates Received",
  "IO-Error",
  "FIFO not open for reading",
  "Other Error",
  "Invalid Option"
};
const char* RSupportOptionNames[RSupportOption__Count] = {
  "Automatic Frequency Adjustment"
  "Automatic Movement Updates"
};

const char* RSupportDefaultFifoPath = "/tmp/lrt_pipe_path.pipe";

const char*
rsupport_status_msg(RSupportStatus status)
{
  return RSupportStatusNames[static_cast<std::size_t>(status)];
}
const char*
rsupport_option_msg(RSupportOption option)
{
  return RSupportOptionNames[static_cast<std::size_t>(option)];
}

RSupportHandle*
rsupport_handle_create(bool subscribedToAll)
{
  RSupportHandle* handle = new RSupportHandle();

  // Initiate other fields.
  handle->registry = std::make_shared<lrt::rregistry::Registry>();

  // Check the environment variables for special modes of operation.
  const char* consoleOnlyMode = std::getenv("RSUPPORT_CONSOLE_ONLY");
  if(consoleOnlyMode) {
    // Activate console only mode. This means, that changes will be printed to
    // the console and each frame reads a single line as LiteComm input from
    // cin.
    handle->consoleAdapter = std::make_shared<lrt::rsupport::ConsoleAdapter>(
      handle->registry, subscribedToAll);
    handle->registry->registerAdapter(handle->consoleAdapter);
  } else {
    handle->pipeAdapter = std::make_shared<lrt::rsupport::PipeAdapter>(
      handle->registry, subscribedToAll);
    handle->registry->registerAdapter(handle->pipeAdapter);
  }

  // Set default options.
  rsupport_handle_set_option(handle, RSupportOption_AutoFrequency, true);
  rsupport_handle_set_option(handle, RSupportOption_AutoMovementBurst, true);

  return handle;
}

RSupportStatus
rsupport_handle_free(RSupportHandle* handle)
{
  if(handle->pipeAdapter)
    handle->registry->removeAdapter(handle->pipeAdapter);
  if(handle->consoleAdapter)
    handle->registry->removeAdapter(handle->consoleAdapter);
  delete handle;
  return RSupportStatus_Ok;
}

RSupportStatus
rsupport_handle_service(RSupportHandle* handle)
{
  // If auto burst is active; at the end of the frame (before delaying for
  // frequency adjustments), the movement state has to be synced.
  if(rsupport_handle_get_option(handle, RSupportOption_AutoMovementBurst))
    handle->registry->set(lrt::rregistry::Bool::BRST_RBREAKOUT_MOVEMENT, true);

  // Delay after burst has been sent and before requesting updates from the
  // adaptor.
  if(rsupport_handle_get_option(handle, RSupportOption_AutoFrequency)) {
    using namespace std::literals::chrono_literals;
    // The end point of the current frame.
    std::chrono::system_clock::time_point frameEnd =
      std::chrono::system_clock::now();

    // The difference between the last time point and the current frame is the
    // time it took for this frame to compute.
    auto duration = frameEnd - handle->frameStart;

    // Set the frame time field for debugging purposes.
    handle->frameTime =
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

    // The difference between Uint8::REGULATION_KERNEL_FREQUENCY
    auto waitDuration =
      1s / ((double)handle->registry->get(
             lrt::rregistry::Uint8::REGULATION_KERNEL_FREQUENCY)) -
      duration;

    // If the console adapter is initialised, it will be used for pacing. If the
    // PipeAdapter is used instead, a clock will be used.
    if(handle->consoleAdapter) {
      handle->consoleAdapter->read();
    } else {
      std::this_thread::sleep_for(waitDuration);
    }

    // Get the start time of the next frame.
    handle->frameStart = std::chrono::system_clock::now();
  }

  RSupportStatus status = RSupportStatus_Ok;

  // Service at the end to get the newest updates.
  if(handle->pipeAdapter)
    status = handle->pipeAdapter->service();

  return status;
}

const char*
getPipePath(const char* pipe)
{
  if(pipe == nullptr || pipe == 0 || strcmp(pipe, "") == 0) {
    // Receive pipe path.
    const char* env_path = std::getenv("LRT_PIPE_PATH");
    if(env_path == 0) {
      env_path = "/tmp/lrt_pipe_path.pipe";
    }
  }
  return pipe;
}

RSupportStatus
rsupport_handle_set_option(RSupportHandle* handle,
                           RSupportOption option,
                           bool state)
{
  if(option >= RSupportOption__Count || handle == nullptr)
    return RSupportStatus_InvalidOption;
  handle->options.set(option, state);
  return RSupportStatus_Ok;
}
bool
rsupport_handle_get_option(RSupportHandle* handle, RSupportOption option)
{

  if(option >= RSupportOption__Count || handle == nullptr)
    return false;
  return handle->options.test(option);
}

RSupportStatus
rsupport_handle_connect(RSupportHandle* handle, const char* pipe)
{
  pipe = getPipePath(pipe);
  RSupportStatus status = RSupportStatus_Ok;
  if(handle->pipeAdapter) {
    status = handle->pipeAdapter->connect(pipe);

    if(status != RSupportStatus_Ok)
      return status;

    // Check if there are any options that need to be sent to the other side.
    if(rsupport_handle_get_option(handle, RSupportOption_AutoFrequency))
      handle->pipeAdapter->subscribe(
        lrt::rregistry::Uint8::REGULATION_KERNEL_FREQUENCY);
  }

  return status;
}

RSupportStatus
rsupport_handle_connect_create(RSupportHandle* handle, const char* pipe)
{
  pipe = getPipePath(pipe);
  RSupportStatus status = RSupportStatus_Ok;
  if(handle->pipeAdapter)
    status = handle->pipeAdapter->create(pipe);
  return status;
}

RSupportStatus
rsupport_handle_disconnect(RSupportHandle* handle)
{
  RSupportStatus status = RSupportStatus_Ok;
  if(handle->pipeAdapter)
    status = handle->pipeAdapter->disconnect();
  return status;
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
  if(handle->pipeAdapter)
    handle->pipeAdapter->subscribeByTypeVal(
      static_cast<lrt::rregistry::Type>(type), property);
  if(handle->consoleAdapter)
    handle->consoleAdapter->subscribeByTypeVal(
      static_cast<lrt::rregistry::Type>(type), property);
}
void
rsupport_handle_unsubscribe(RSupportHandle* handle,
                            uint16_t type,
                            uint16_t property)
{
  if(handle->pipeAdapter)
    handle->pipeAdapter->unsubscribeByTypeVal(
      static_cast<lrt::rregistry::Type>(type), property);
  if(handle->consoleAdapter)
    handle->consoleAdapter->unsubscribeByTypeVal(
      static_cast<lrt::rregistry::Type>(type), property);
}
void
rsupport_handle_request(RSupportHandle* handle,
                        uint16_t type,
                        uint16_t property)
{
  if(handle->pipeAdapter)
    handle->pipeAdapter->requestByTypeVal(
      static_cast<lrt::rregistry::Type>(type), property);
  if(handle->consoleAdapter)
    handle->consoleAdapter->requestByTypeVal(
      static_cast<lrt::rregistry::Type>(type), property);
}

std::shared_ptr<lrt::rregistry::Registry>
rsupport_handle_get_registry(RSupportHandle* handle)
{
  return handle->registry;
}

uint32_t
rsupport_handle_get_frame_time(RSupportHandle* handle)
{
  return handle->frameTime;
}
