#include "../include/RSupport/PipeAdapter.hpp"
#include <algorithm>
#include <array>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using std::cout;
using std::endl;

namespace lrt {
namespace rsupport {
PipeAdapter::PipeAdapter(std::shared_ptr<rregistry::Registry> registry)
  : LiteCommAdapter(registry)
{
}
PipeAdapter::~PipeAdapter() {}

void
PipeAdapter::send(const rregistry::Registry::Adapter::Message& msg)
{
  write(m_out_fd, static_cast<const void*>(&msg.buf[0]), msg.length());
}
RSupportStatus
PipeAdapter::connect(const char* pipe)
{
  m_mode = Client;
  // "in" means into the master!
  m_inFifo = std::string(pipe) + "-in";
  m_outFifo = std::string(pipe) + "-out";

  // Switched, because this is the client side.
  m_in_fd = open(m_outFifo.c_str(), O_RDONLY | O_NONBLOCK);
  m_out_fd = open(m_inFifo.c_str(), O_WRONLY | O_NONBLOCK);

  if(m_in_fd <= 0 || m_out_fd <= 0) {
    cout << "Error opening fifos: " << strerror(errno) << endl;
    return RSupportStatus_CouldNotOpenFIFOs;
  }

  int flags = fcntl(m_in_fd, F_GETFL, 0);
  fcntl(m_in_fd, F_SETFL, flags | O_NONBLOCK);

  request(rregistry::Bool::TEST_PIPE_COMMUNICATION);

  return RSupportStatus_Ok;
}
RSupportStatus
PipeAdapter::create(const char* pipe)
{
  m_mode = Server;
  // "in" means into the master!
  m_inFifo = std::string(pipe) + "-in";
  m_outFifo = std::string(pipe) + "-out";

  mkfifo(m_inFifo.c_str(), 0666);
  mkfifo(m_outFifo.c_str(), 0666);

  m_in_fd = open(m_inFifo.c_str(), O_RDONLY | O_NONBLOCK);

  if(m_in_fd == -1) {
    cout << "Error opening fifos: " << strerror(errno) << endl;
    return RSupportStatus_CouldNotOpenFIFOs;
  }

  int flags = fcntl(m_in_fd, F_GETFL, 0);
  fcntl(m_in_fd, F_SETFL, flags | O_NONBLOCK);

  return RSupportStatus_Ok;
}
RSupportStatus
PipeAdapter::disconnect()
{
  close(m_in_fd);
  close(m_out_fd);
}
RSupportStatus
PipeAdapter::service()
{
  const std::size_t bufferSize = 100;

  std::array<char, bufferSize> buffer;

  ssize_t ssize = read(m_in_fd, &buffer, bufferSize);
  if(ssize > 0) {
    if(m_mode == Server && m_out_fd <= 0) {
      m_out_fd = open(m_outFifo.c_str(), O_WRONLY);
    }
    if(m_out_fd == -1) {
      cout << "Error opening fifos: " << strerror(errno) << endl;
      return RSupportStatus_CouldNotOpenFIFOs;
    }

    size_t size = ssize;
    // Handle the arrived bytes.
    using namespace rcomm;
    // Copy the received bytes into the internal buffer and wait as long as it
    // takes for the specified message length to arrive.

    auto it = buffer.begin();
    while(size > 0) {
      if(m_messageIt == m_message.buf.begin()) {
        // Begin of new message.

        // Get the type of the new message;
        (*m_messageIt++) = (*it++);

        --size;

        // Check if the received byte matches a known type.
        if(m_message.lType() >= LiteCommType::_COUNT) {
          (m_messageIt = m_message.buf.begin());
          continue;
        }
      }

      std::size_t remainingBytes = m_message.remainingBytes(m_messageIt);

      if(remainingBytes > 0 && size > 0) {
        std::size_t copyBytes = std::min(remainingBytes, size);
        for(std::size_t i = 0; i < copyBytes; ++i)
          (*m_messageIt++) = (*it++);
        remainingBytes -= copyBytes;
        size -= copyBytes;
      }

      if(remainingBytes == 0) {
        parseMessage(m_message);
        m_messageIt = m_message.buf.begin();
      }
    }
    return RSupportStatus_Updates;
  }

  // Error handling
  if(ssize == EBADF)
    return RSupportStatus_FIFONotOpenForReading;
  else if(ssize == 0 || ssize == EAGAIN || errno == EAGAIN)
    return RSupportStatus_Ok;
  else if(ssize == EIO)
    return RSupportStatus_IOError;
  cout << "Error reading fifo: " << strerror(errno) << endl;
  return RSupportStatus_OtherError;
}
}
}
