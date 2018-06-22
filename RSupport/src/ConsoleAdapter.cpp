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

  unsigned int writePaddChars = (3 - msg.length() % 3) % 3;

  // Output the new data to the console directly.
  cout << "!:";
  std::copy(base64Iterator(msg.buf.begin()),
            base64Iterator(msg.buf.begin() + msg.length()),
            std::ostreambuf_iterator<char>(cout));

  while(writePaddChars > 0) {
    cout << "=";
    writePaddChars--;
  }

  cout << endl;
}
void
ConsoleAdapter::read()
{
  using namespace boost::archive::iterators;
  typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>
    binaryIt;

  std::getline(cin, m_inputBuffer);
  // Parse incoming messages.

  if(m_inputBuffer.length() > 2) {
    if(m_inputBuffer[0] == '!' && m_inputBuffer[1] == ':') {
      m_inputBuffer.erase(0, 2);

      std::replace(m_inputBuffer.begin(), m_inputBuffer.end(), '=', 'A');

      std::copy(binaryIt(m_inputBuffer.begin()),
                binaryIt(m_inputBuffer.end()),
                m_inputMessage.buf.begin());
    }

    parseMessage(m_inputMessage);
  }
}
}
}
