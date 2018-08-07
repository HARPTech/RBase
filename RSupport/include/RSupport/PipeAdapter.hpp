#ifndef LRT_RSUPPORT_PIPEADAPTER_HPP
#define LRT_RSUPPORT_PIPEADAPTER_HPP

#include "RSupport.hpp"
#include <RCore/transmit_buffer.h>
#include <RRegistry/Registry.hpp>
#include <RCore/defaults.h>

namespace lrt {
namespace rsupport {
class PipeAdapter : public rregistry::Registry::Adapter
{
  public:
  PipeAdapter(std::shared_ptr<rregistry::Registry> registry,
              bool subscribedToAll = false);
  virtual ~PipeAdapter();

  virtual void send(
    lrt_rcore_transmit_buffer_entry_t* entry,
    rcomm::Reliability reliability = rcomm::DefaultReliability) override;

  /**
   * @brief Connects to the two fifos at the provided path.
   *
   * These pipes are created by RMaster and serve as an entry point for add-on
   * applications on the same host.
   */
  RSupportStatus connect(const char* pipe);
  RSupportStatus create(const char* pipe);
  RSupportStatus disconnect();
  RSupportStatus service();

  int inFd() { return m_in_fd; }
  int outFd() { return m_out_fd; }
  void setInFd(int inFd) { m_in_fd = inFd; }
  void setOutFd(int outFd) { m_out_fd = outFd; }

  const std::string& inFifo() { return m_inFifo; }
  const std::string& outFifo() { return m_outFifo; }

  private:
  int m_in_fd = 0, m_out_fd = 0;
  std::string m_inFifo;
  std::string m_outFifo;

  enum Mode
  {
    Server,
    Client
  };

  Mode m_mode = Client;
  rcomm_handle_t *m_rcomm_handle;
};
}
}

#endif
