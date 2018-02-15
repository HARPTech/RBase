#ifndef LRT_RSUPPORT_PIPEADAPTER_HPP
#define LRT_RSUPPORT_PIPEADAPTER_HPP

#include "RSupport.hpp"
#include <RRegistry/Registry.hpp>

namespace lrt {
namespace rsupport {
class PipeAdapter : public rregistry::Registry::Adapter
{
  public:
  PipeAdapter(std::shared_ptr<rregistry::Registry> registry);
  virtual ~PipeAdapter();

  virtual void send(const rregistry::Registry::Adapter::Message& msg) override;

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

  private:
  rregistry::Registry::Adapter::Message m_message;
  rregistry::Registry::Adapter::Message::Buffer::iterator m_messageIt;
  int m_in_fd = 0, m_out_fd = 0;
  std::string m_inFifo;
  std::string m_outFifo;

  enum Mode
  {
    Server,
    Client
  };

  Mode m_mode = Client;
};
}
}

#endif
