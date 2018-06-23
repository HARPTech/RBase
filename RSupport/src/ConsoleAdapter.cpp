#include "../include/RSupport/ConsoleAdapter.hpp"
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
{
}
ConsoleAdapter::~ConsoleAdapter() {}

void
ConsoleAdapter::send(const rregistry::Registry::Adapter::Message& msg,
                     rcomm::Reliability reliability)
{
  using namespace boost::archive::iterators;
  typedef base64_from_binary<
    transform_width<Message::Buffer::const_iterator, 6, 8>>
    base64Iterator;

  int writePaddChars = (3 - msg.length() % 3) % 3 + 1;

  if(m_mode & STDOUT) {
    // Output the new data to the console directly.
    cout << "!:";
    std::copy(base64Iterator(msg.buf.begin()),
              base64Iterator(msg.buf.begin() + msg.length()),
              std::ostreambuf_iterator<char>(cout));

    while(writePaddChars >= 0) {
      cout << "=";
      writePaddChars--;
    }
    cout << endl;
  }
  if(m_mode & CALLBACK) {
    // Output the data to all callbacks.
    std::string outStr = "";
    outStr.reserve(20);
    outStr = std::string(base64Iterator(msg.buf.begin()),
                         base64Iterator(msg.buf.begin() + msg.length()));
    outStr = "!:" + outStr;
    while(writePaddChars >= 0) {
      outStr.append("=");
      writePaddChars--;
    }
    outStr.append("\n");
    for(auto cb : m_callbacks) {
      cb(outStr);
    }
  }
}
void
ConsoleAdapter::read()
{
  if(m_readTimeout < 10) {
    m_readTimeout ++;
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

    std::copy(binaryIt(base64.begin()),
              binaryIt(base64.end()),
              m_inputMessage.buf.begin());
    parseMessage(m_inputMessage);
  } catch(dataflow_exception& e) {
    clog << "Invalid base64: \"" << base64 << "\"" << endl
         << "Error: " << e.what();
  }
}
}
}
