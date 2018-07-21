#ifndef LRT_RSUPPORT_CONSOLEADAPTER_HPP
#define LRT_RSUPPORT_CONSOLEADAPTER_HPP

#include <RRegistry/Registry.hpp>
#include <functional>
#include <vector>

namespace lrt {
namespace rsupport {
class ConsoleAdapter : public rregistry::Registry::Adapter
{
  public:
  enum Mode
  {
    STDOUT = 0b00000001,
    CALLBACK = 0b00000010,
    MIXED = 0b00000011
  };

  typedef std::function<void(std::string)> WriteCallback;

  ConsoleAdapter(std::shared_ptr<rregistry::Registry> registry,
                 bool subscribedToAll = false);
  virtual ~ConsoleAdapter();

  virtual void send(
    const rregistry::Registry::Adapter::Message& msg,
    rcomm::Reliability reliability = rcomm::DefaultReliability) override;

  void addCallback(WriteCallback cb) { m_callbacks.push_back(cb); }

  void setMode(Mode mode) { m_mode = mode; }

  void read();
  void parseLine(std::string line);
  void parseBase64(std::string base64);

  private:
  Message m_inputMessage;
  std::vector<WriteCallback> m_callbacks;
  Mode m_mode = STDOUT;
  int m_readTimeout = 0;
};
}
}

#endif
