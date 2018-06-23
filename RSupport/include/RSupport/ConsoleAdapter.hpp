#ifndef LRT_RSUPPORT_CONSOLEADAPTER_HPP
#define LRT_RSUPPORT_CONSOLEADAPTER_HPP

#include <RRegistry/Registry.hpp>

namespace lrt {
namespace rsupport {
class ConsoleAdapter : public rregistry::Registry::Adapter
{
  public:
  public:
  ConsoleAdapter(std::shared_ptr<rregistry::Registry> registry,
                 bool subscribedToAll = false);
  virtual ~ConsoleAdapter();

  virtual void send(
    const rregistry::Registry::Adapter::Message& msg,
    rcomm::Reliability reliability = rcomm::DefaultReliability) override;

  void read();
  void parseBase64(const std::string& base64);

  private:
  std::string m_inputBuffer;
  Message m_inputMessage;
};
}
}

#endif
