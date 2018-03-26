#ifndef LRT_RCOMM_LITECOMMMESSAGE_HPP
#define LRT_RCOMM_LITECOMMMESSAGE_HPP

#include "LiteComm.hpp"
#include "LiteCommReliability.hpp"
#include <RRegistry/Entries.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <memory>
#include <sstream>

namespace lrt {
namespace rcomm {

/**
 * @brief Represents a Message from LiteComm.
 *
 * This struct is a utility structure to simplify the creation of more
 * transmission methods. Use this Message struct to hold any message data,
 * because all LiteComm functionality generates and takes instances of this
 * Message struct.
 */
struct LiteCommMessage
{
  static const std::size_t maxLength = 17;
  std::size_t explicit_length = 0;

  uint16_t sequentNumber = 0;
  uint16_t clientId = 0;

  using Buffer = std::array<uint8_t, maxLength>;

  Buffer buf;

  LiteCommMessage() { buf.fill(0); }

  LiteCommMessage(const LiteCommMessage& msg)
  {
    buf = msg.buf;
    sequentNumber = msg.sequentNumber;
    clientId = msg.clientId;
    explicit_length = msg.explicit_length;
  }

  virtual ~LiteCommMessage() {}

  /**
   * @brief Returns the type of the message.
   *
   * The type is the first byte of the message casted to rcomm::LiteCommType.
   */
  inline LiteCommType lType(size_t pos = 0) const
  {
    return static_cast<LiteCommType>(buf[pos]);
  }
  /**
   * @brief Returns the real (embedded, if Lossy) type of the message.
   */
  inline LiteCommType realLType() const
  {
    auto type = lType();
    switch(type) {
      case LiteCommType::Lossy:
        return lType(sizeof(uint16_t) * 2 + sizeof(uint8_t));
      default:
        return type;
    }
  }

  /**
   * @brief Returns the length of this message based on its type.
   */
  inline std::size_t length(std::size_t pos = 0) const
  {
    switch(lType(pos)) {
      case LiteCommType::Update:
      case LiteCommType::Append:
        return pos + 12;
      case LiteCommType::Request:
      case LiteCommType::Subscribe:
      case LiteCommType::Unsubscribe:
        return pos + 4;
      case LiteCommType::Debug:
        return pos + 8;
      case LiteCommType::Lossy:
        return length(sizeof(uint16_t) * 2 + 1);
      default:
        // Must be a custom length.
        return pos + explicit_length;
    }
  }

  /**
   * @brief Returns the begin of the data segment.
   */
  inline Buffer::iterator begin(std::size_t pos = 0)
  {
    switch(lType(pos)) {
      case LiteCommType::Lossy:
        return begin(sizeof(uint16_t) * 2 + sizeof(uint8_t));
      default:
        return buf.begin() + pos + sizeof(uint16_t) + sizeof(uint8_t) * 2;
    }
  }
  /**
   * @brief Returns the begin of the data segment.
   */
  inline Buffer::const_iterator begin(std::size_t pos = 0) const
  {
    switch(lType(pos)) {
      case LiteCommType::Lossy:
        return begin(sizeof(uint16_t) * 2 + sizeof(uint8_t));
      default:
        return buf.begin() + pos + sizeof(uint16_t) + sizeof(uint8_t) * 2;
    }
  }

  inline void setLType(LiteCommType type,
                       Reliability reliability = DefaultReliability)
  {
    if(reliability & BasicDelivery) {
      // Activate Lossy mode using client IDs and sequent numbers.
      buf[0] = static_cast<uint8_t>(LiteCommType::Lossy);
      buf[5] = static_cast<uint8_t>(type);

      // Set the client number and sequent number these are kept track of
      // internally.
      setClientId(clientId);
      setSequentNumber(sequentNumber++);
    } else {
      buf[0] = static_cast<uint8_t>(type);
    }
  }
  inline void setType(rregistry::Type type)
  {
    switch(lType()) {
      case LiteCommType::Lossy:
        buf[6] = static_cast<uint8_t>(type);
        break;
      default:
        buf[1] = static_cast<uint8_t>(type);
    }
  }
  inline rregistry::Type getType() const
  {
    switch(lType()) {
      case LiteCommType::Lossy:
        return static_cast<rregistry::Type>(buf[6]);
        break;
      default:
        return static_cast<rregistry::Type>(buf[1]);
    }
  }

  inline void setClientId(uint16_t clientId)
  {
    thread_local LiteCommClientId id;
    id.id = clientId;
    id.write(buf.begin() + 1);
  }
  inline uint16_t getClientId() const
  {
    thread_local LiteCommClientId id;
    return id.read(buf.begin() + 1);
  }
  inline void setSequentNumber(uint16_t number)
  {
    thread_local LiteCommSequentNumber n;
    n.n = number;
    n.write(buf.begin() + 3);
  }
  inline uint16_t getSequentNumber() const
  {
    thread_local LiteCommSequentNumber n;
    return n.read(buf.begin() + 3);
  }
  inline void setProperty(uint16_t property)
  {
    thread_local LiteCommProp prop;
    prop.property = property;
    switch(lType()) {
      case LiteCommType::Lossy:
        prop.write(buf.begin() + 7);
        break;
      default:
        prop.write(buf.begin() + 2);
    }
  }
  inline uint16_t getProperty() const
  {
    thread_local LiteCommProp property;
    switch(lType()) {
      case LiteCommType::Lossy:
        return property.read(buf.begin() + 7);
      default:
        return property.read(buf.begin() + 2);
    }
  }

  /**
   * @brief Returns the remaining byte count for this message, based its
   * type.
   */
  inline std::size_t remainingBytes(
    const typename Buffer::const_iterator it) const
  {
    return (buf.begin() + length()) - it;
  }

  void setSingleBit(uint8_t bit)
  {
    uint8_t& byte = buf[bit / 8];
    byte |= (1 << (7 - (bit % 8)));
  }

  std::string print() const
  {
    std::stringstream outStream;
    for(auto byte = buf.begin(); byte != buf.begin() + length(); ++byte) {
      std::bitset<8> bits(*byte);
      outStream << "  " << bits << "\n";
    }
    return outStream.str();
  }
};
}
}

#endif
