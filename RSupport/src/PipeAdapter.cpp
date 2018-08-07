#include "../include/RSupport/PipeAdapter.hpp"
#include <RCore/defaults.h>
#include <RCore/rcomm.h>
#include <algorithm>
#include <array>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

LRT_RCOMM_UNIVERSAL_DEFINITIONS()

using std::cout;
using std::endl;

namespace lrt {
namespace rsupport {
PipeAdapter::PipeAdapter(std::shared_ptr<rregistry::Registry> registry,
                         bool subscribedToAll)
  : LiteCommAdapter(registry, subscribedToAll, "PipeAdapter", 2)
{
  m_rcomm_handle = rcomm_create();
  rcomm_set_transmit_cb(
    m_rcomm_handle,
    [](const uint8_t* data, void* userdata, size_t bytes) {
      PipeAdapter* adapter = static_cast<PipeAdapter*>(userdata);
      write(adapter->m_out_fd, static_cast<const void*>(data), bytes);
      return LRT_RCORE_OK;
    },
    this);
  rcomm_set_accept_cb(
    m_rcomm_handle,
    [](rcomm_block_t* block, void* userdata) {
      PipeAdapter* adapter = static_cast<PipeAdapter*>(userdata);

      rcomm_transfer_block_to_tb(
        adapter->m_rcomm_handle, block, adapter->m_transmit_buffer.get());

      return LRT_RCORE_OK;
    },
    this);
}
PipeAdapter::~PipeAdapter()
{
  rcomm_free(m_rcomm_handle);
}

void
PipeAdapter::send(lrt_rcore_transmit_buffer_entry_t* entry,
                  rcomm::Reliability reliability)
{
  rcomm_send_tb_entry(m_rcomm_handle, entry);
}
RSupportStatus
PipeAdapter::connect(const char* pipe)
{
  m_mode = Client;
  // "in" means into the master!
  m_inFifo = std::string(pipe) + "-in";
  m_outFifo = std::string(pipe) + "-out";

  // Switched, because this is the client side.
  m_in_fd = open(m_outFifo.c_str(), O_RDONLY | O_NONBLOCK);
  m_out_fd = open(m_inFifo.c_str(), O_WRONLY | O_NONBLOCK);

  if(m_in_fd <= 0 || m_out_fd <= 0) {
    cout << "Error opening fifos: " << strerror(errno) << endl;
    return RSupportStatus_CouldNotOpenFIFOs;
  }

  int flags = fcntl(m_in_fd, F_GETFL, 0);
  fcntl(m_in_fd, F_SETFL, flags | O_NONBLOCK);

  request(rregistry::Bool::TEST_PIPE_COMMUNICATION);

  return RSupportStatus_Ok;
}
RSupportStatus
PipeAdapter::create(const char* pipe)
{
  m_mode = Server;
  // "in" means into the master!
  m_inFifo = std::string(pipe) + "-in";
  m_outFifo = std::string(pipe) + "-out";

  mkfifo(m_inFifo.c_str(), 0666);
  mkfifo(m_outFifo.c_str(), 0666);

  m_in_fd = open(m_inFifo.c_str(), O_RDONLY | O_NONBLOCK);

  if(m_in_fd == -1) {
    cout << "Error opening fifos: " << strerror(errno) << endl;
    return RSupportStatus_CouldNotOpenFIFOs;
  }

  int flags = fcntl(m_in_fd, F_GETFL, 0);
  fcntl(m_in_fd, F_SETFL, flags | O_NONBLOCK);

  return RSupportStatus_Ok;
}
RSupportStatus
PipeAdapter::disconnect()
{
  close(m_in_fd);
  close(m_out_fd);
  return RSupportStatus_Ok;
}
RSupportStatus
PipeAdapter::service()
{
  const std::size_t bufferSize = 200;

  std::array<uint8_t, bufferSize> buffer;

  ssize_t ssize = read(m_in_fd, &buffer, bufferSize);
  if(ssize > 0) {
    if(m_mode == Server && m_out_fd <= 0) {
      m_out_fd = open(m_outFifo.c_str(), O_WRONLY);
    }
    if(m_out_fd == -1) {
      cout << "Error opening fifos: " << strerror(errno) << endl;
      return RSupportStatus_CouldNotOpenFIFOs;
    }

    size_t size = ssize;
    // Handle the arrived bytes.
    using namespace rcomm;
    // Copy the received bytes into the internal buffer and wait as long as it
    // takes for the specified message length to arrive.

    rcomm_parse_bytes(
      m_rcomm_handle, static_cast<const uint8_t*>(buffer.data()), size);

    return RSupportStatus_Updates;
  }

  // Error handling
  if(ssize == EBADF)
    return RSupportStatus_FIFONotOpenForReading;
  else if(ssize == 0 || ssize == EAGAIN || errno == EAGAIN)
    return RSupportStatus_Ok;
  else if(ssize == EIO)
    return RSupportStatus_IOError;
  cout << "Error reading fifo: " << strerror(errno) << endl;
  return RSupportStatus_OtherError;
}
}
}
