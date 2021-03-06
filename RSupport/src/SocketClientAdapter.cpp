#include "../include/RSupport/SocketClientAdapter.hpp"
#include <RCore/rcomm.h>
#include <RCore/util.hpp>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Link with correct target glibc.
// https://stackoverflow.com/questions/2856438/how-can-i-link-to-a-specific-glibc-version
__asm__(".symver realpath,realpath@GLIBC_3.4.21");

using std::cout;
using std::endl;

namespace lrt {
namespace rsupport {
SocketClientAdapter::SocketClientAdapter(
  std::shared_ptr<rregistry::Registry> registry,
  bool subscribedToAll)
  : rregistry::Registry::Adapter(registry, subscribedToAll)
  , m_rcomm_handle(RCore::CreateRCommHandlePtr())
{
  rcomm_set_transmit_cb(m_rcomm_handle.get(),
                        [](const uint8_t* data, void* userdata, size_t bytes) {
                          SocketClientAdapter* adapter =
                            static_cast<SocketClientAdapter*>(userdata);

                          if(adapter->m_fd > 0) {
                            if(write(adapter->m_fd, data, bytes) != bytes) {
                              return LRT_RCORE_TRANSMIT_ERROR;
                            }
                          }

                          return LRT_RCORE_OK;
                        },
                        this);
  rcomm_set_accept_cb(m_rcomm_handle.get(),
                      [](lrt_rbp_message_t* message, void* userdata) {
                        SocketClientAdapter* adapter =
                          static_cast<SocketClientAdapter*>(userdata);

                        rcomm_transfer_message_to_tb(
                          adapter->m_rcomm_handle.get(),
                          message,
                          adapter->m_transmit_buffer.get());

                        return LRT_RCORE_OK;
                      },
                      this);
}
SocketClientAdapter::~SocketClientAdapter() {}
lrt_rcore_event_t
SocketClientAdapter::send(lrt_rcore_transmit_buffer_entry_t* entry,
                          rcomm::Reliability reliability)
{
  return rcomm_send_tb_entry(m_rcomm_handle.get(), entry);
}

RSupportStatus
SocketClientAdapter::connect(const std::string& socket)
{
  cout << "[RSupport] Connecting to socket " << socket << endl;
  memset(&m_addr, 0, sizeof(m_addr));
  m_addr.sun_family = AF_UNIX;
  strncpy(m_addr.sun_path, socket.c_str(), sizeof(m_addr.sun_path));

  // Open the socket.
  if((m_fd = ::socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    m_fd = 0;
    cout << "[RSupport] Opening the socket failed! Error: " << strerror(errno)
         << endl;
    return RSupportStatus_CouldNotOpenSocket;
  }

  // Connect to the socket.
  if(::connect(m_fd, (struct sockaddr*)&m_addr, sizeof(m_addr)) == -1) {
    m_fd = 0;
    cout << "[RSupport] Socket connection failed! Error: " << strerror(errno)
         << endl;
    return RSupportStatus_ConnectionFailed;
  }

  // Set non-blocking
  int flags;
  if(-1 == (flags = fcntl(m_fd, F_GETFL, 0)))
    flags = 0;
  fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);

  return RSupportStatus_Ok;
}
RSupportStatus
SocketClientAdapter::disconnect()
{
  close(m_fd);
  return RSupportStatus_Ok;
}
RSupportStatus
SocketClientAdapter::service()
{
  m_rc = ::read(m_fd, &m_buffer, m_buffer.size());
  if(m_rc > 0) {
    // Data ready to be read by RComm.
    rcomm_parse_bytes(m_rcomm_handle.get(), m_buffer.data(), m_rc);
    return RSupportStatus_Updates;
  }
  if(m_rc == -1) {
    switch(errno) {
      case EAGAIN:
        return RSupportStatus_Ok;
      case EIO:
        return RSupportStatus_IOError;
      case EBADF:
        return RSupportStatus_SocketNotOpenForReading;
      default:
        return RSupportStatus_OtherError;
    }
  }
  return RSupportStatus_Ok;
}
}
}
