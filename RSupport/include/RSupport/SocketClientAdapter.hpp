#ifndef LRT_RSUPPORT_SOCKETCLIENTADAPTER_HPP
#define LRT_RSUPPORT_SOCKETCLIENTADAPTER_HPP

#include "RSupport.hpp"
#include <RCore/defaults.h>
#include <RCore/util.hpp>
#include <RRegistry/Registry.hpp>
#include <functional>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>

namespace lrt {
namespace rsupport {
class SocketClientAdapter : public rregistry::Registry::Adapter
{
  public:
  SocketClientAdapter(std::shared_ptr<rregistry::Registry> registry,
                      bool subscribedToAll = false);
  virtual ~SocketClientAdapter();

  virtual lrt_rcore_event_t send(
    lrt_rcore_transmit_buffer_entry_t* entry,
    rcomm::Reliability reliability = rcomm::DefaultReliability) override;

  RSupportStatus connect(const std::string& socket);
  RSupportStatus disconnect();
  RSupportStatus service();

  int getSocketFd() { return m_fd; }

  protected:
  RCore::RCommHandlePtr m_rcomm_handle;
  struct sockaddr_un m_addr;
  std::array<uint8_t, 100> m_buffer;
  int m_fd = 0, m_rc = 0;
};
}
}

#endif
