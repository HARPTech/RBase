#include "../include/RSupport/ConsoleAdapter.hpp"
#include <RCore/transmit_buffer.h>
#include <RRegistry/Detail.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using std::cin;
using std::clog;
using std::cout;
using std::endl;

namespace lrt {
namespace rsupport {
ConsoleAdapter::ConsoleAdapter(std::shared_ptr<rregistry::Registry> registry,
                               bool subscribedToAll)
  : rregistry::Registry::Adapter(registry, subscribedToAll)
  , m_rcomm_handle(RCore::CreateRCommHandlePtr())
{
  rcomm_set_transmit_cb(
    m_rcomm_handle.get(),
    [](const uint8_t* data, void* userdata, size_t bytes) {
      ConsoleAdapter* adapter = static_cast<ConsoleAdapter*>(userdata);
      using namespace boost::archive::iterators;
      typedef base64_from_binary<transform_width<const uint8_t*, 6, 8>>
        base64Iterator;

      assert(data != NULL);
      assert(bytes > 0);

      const uint8_t* dataEnd = data + bytes;

      int writePaddChars = (3 - (bytes) % 3) % 3 + 1;

      if(adapter->m_mode & STDOUT) {
        // Output the new data to the console directly.
        cout << "!:";
        std::copy(base64Iterator(data),
                  base64Iterator(dataEnd),
                  std::ostreambuf_iterator<char>(cout));

        while(writePaddChars >= 0) {
          cout << "=";
          writePaddChars--;
        }
        cout << endl;
      }
      if(adapter->m_mode & CALLBACK) {
        // Output the data to all callbacks.
        std::string outStr =
          std::string(base64Iterator(data), base64Iterator(dataEnd));
        outStr = "!:" + outStr;
        while(writePaddChars >= 0) {
          outStr.append("=");
          writePaddChars--;
        }
        outStr.append("\n");
        for(auto cb : adapter->m_callbacks) {
          cb(outStr);
        }
      }
      return LRT_RCORE_OK;
    },
    this);
  rcomm_set_accept_cb(
    m_rcomm_handle.get(),
    [](lrt_rbp_message_t* message, void* userdata) {
      ConsoleAdapter* adapter = static_cast<ConsoleAdapter*>(userdata);

      rcomm_transfer_message_to_tb(adapter->m_rcomm_handle.get(),
                                   message,
                                   adapter->m_transmit_buffer.get());

      return LRT_RCORE_OK;
    },
    this);
}
ConsoleAdapter::~ConsoleAdapter() {}

void
ConsoleAdapter::send(lrt_rcore_transmit_buffer_entry_t* entry,
                     rcomm::Reliability reliability)
{
  rcomm_send_tb_entry(m_rcomm_handle.get(), entry);
}
void
ConsoleAdapter::read()
{
  if(m_readTimeout < 10) {
    m_readTimeout++;
    return;
  }
  std::string line;
  std::getline(cin, line);
  parseLine(line);
}
void
ConsoleAdapter::parseLine(std::string line)
{
  if(line.length() > 2) {
    if(line[0] == '!' && line[1] == ':') {
      line.erase(0, 2);
      parseBase64(line);
    }
  }
}

void
ConsoleAdapter::parseBase64(std::string base64)
{
  using namespace boost::archive::iterators;
  typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>
    binaryIt;

  try {
    base64 = base64.substr(0, base64.find('\n'));
    std::replace(base64.begin(), base64.end(), '=', 'A');

    std::string binaryStr =
      std::string(binaryIt(base64.begin()), binaryIt(base64.end()));

    rcomm_parse_bytes(m_rcomm_handle.get(),
                      reinterpret_cast<const uint8_t*>(binaryStr.data()),
                      binaryStr.length());

  } catch(dataflow_exception& e) {
    clog << "Invalid base64: \"" << base64 << "\"" << endl
         << "Error: " << e.what() << endl;
  }
}
}
}
