#ifndef LRT_RSUPPORT_CONSOLEADAPTER_HPP
#define LRT_RSUPPORT_CONSOLEADAPTER_HPP

#include <RCore/defaults.h>
#include <RCore/util.hpp>
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

  virtual lrt_rcore_event_t send(
    lrt_rcore_transmit_buffer_entry_t* entry,
    rcomm::Reliability reliability = rcomm::DefaultReliability) override;

  void addCallback(WriteCallback cb) { m_callbacks.push_back(cb); }

  void setMode(Mode mode) { m_mode = mode; }

  lrt_rcore_event_t read();
  lrt_rcore_event_t parseLine(std::string line);
  lrt_rcore_event_t parseBase64(std::string base64);

  std::string extractBinaryStreamFromLine(std::string line);
  std::string binaryStreamToBitRepresentation(std::string binary);

  inline size_t calculateCorrectDataLength(size_t length)
  {
    return (length / 8) * 8;
  }

  private:
  std::vector<WriteCallback> m_callbacks;
  Mode m_mode = STDOUT;
  int m_readTimeout = 0;

  RCore::RCommHandlePtr m_rcomm_handle;
};
}
}

#endif
