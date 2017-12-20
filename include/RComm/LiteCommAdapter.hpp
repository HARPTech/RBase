#ifndef LRT_RCOMM_LITECOMMADAPTER_HPP
#define LRT_RCOMM_LITECOMMADAPTER_HPP

#include <RComm/LiteComm.hpp>
#include <RRegistry/SubscriptionMap.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>

#define LRT_RCOMM_LITECOMMADAPTER_PARSEMESSAGE_SET_CASE(CLASS)         \
  case rregistry::Type::CLASS:                                         \
    if(lProp.property < static_cast<size_t>(rregistry::CLASS::_COUNT)) \
      m_registry->set(static_cast<rregistry::CLASS>(lProp.property),   \
                      lData.CLASS);                                    \
    break;

namespace lrt {
namespace rcomm {

template<class RegistryClass>
class LiteCommAdapter
{
  public:
  /**
   * @brief Represents a Message from LiteComm.
   *
   * This struct is a utility structure to simplify the creation of more
   * transmission methods. Use this Message struct to hold any message data,
   * because all LiteComm functionality generates and takes instances of this
   * Message struct.
   */
  struct Message
  {
    static const std::size_t maxLength = 12;

    using Buffer = std::array<uint8_t, maxLength>;

    Buffer buf;

    /**
     * @brief Returns the type of the message.
     *
     * The type is the first byte of the message casted to rcomm::LiteCommType.
     */
    inline LiteCommType lType() const
    {
      return static_cast<LiteCommType>(buf[0]);
    }

    /**
     * @brief Returns the length of this message based on its type.
     */
    inline std::size_t length() const
    {
      switch(lType()) {
        case LiteCommType::Update:
        case LiteCommType::Append:
          return sizeof(LiteCommType) + sizeof(LiteCommProp) + sizeof(LiteCommData) + 1;
        case LiteCommType::Request:
        case LiteCommType::Subscribe:
        case LiteCommType::Unsubscribe:
          return sizeof(LiteCommType) + sizeof(LiteCommProp) + 1;
      }
    }

    /**
     * @brief Returns the practical begin of the internal buffer, after the
     * LiteCommType declaration.
     */
    inline auto begin() const { return buf.begin() + 1; }

    /**
     * @brief Returns the remaining byte count for this message, based its
     * type.
     */
    inline std::size_t remainingBytes(
      const typename Buffer::const_iterator it) const
    {
      return (buf.begin() + length()) - it;
    }
  };

  LiteCommAdapter(std::shared_ptr<RegistryClass> registry,
                  bool subscribed = false)
    : m_registry(registry)
    , m_subscriptions(rregistry::InitSubscriptionMap(subscribed))
  {}
  ~LiteCommAdapter() {}

  /**
   * @brief Sends the message over the transmission method implemented by child
   * classes.
   *
   * This method is called, when there are Messages to be sent. The only thing
   * implementations have to do to be able to send messages is to implement the
   * transmission of Message structs for the length Message::length()
   */
  virtual void send(const Message& msg) = 0;

  template<class TypeCategory>
  void set(
    TypeCategory property,
    typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type value)
  {
    using namespace rcomm;

    if(!m_acceptProperty)
      return;

    // Check if the property has been subscribed by the current adapter.
    if(!(*m_subscriptions)[static_cast<std::size_t>(
         rregistry::GetEnumTypeOfEntryClass(property))]
                          [static_cast<uint32_t>(property)])
      return;

    Message message;

    LiteCommType lType = LiteCommType::Update;
    rregistry::Type type = GetEnumTypeOfEntryClass(property);
    LiteCommProp lProp;
    LiteCommData lData;

    lProp.property = static_cast<typeof(LiteCommProp::property)>(property);
    LiteCommData::fromType(lData, value);

    // Copy into the message.
    auto it = message.buf.begin();
    (*it++) = static_cast<uint8_t>(lType);
    (*it++) = static_cast<uint8_t>(type);
    for(size_t i = 0; i < sizeof(LiteCommProp); ++i)
      (*it++) = lProp.byte[i];
    for(size_t i = 0; i < sizeof(LiteCommData); ++i)
      (*it++) = lData.byte[i];

    send(message);

    // Check if the message is a string and handle the string uniquely.
    if(type == rregistry::Type::String) {
      std::size_t stringLength = lData.Int32;
      for(std::size_t i = 0; i < stringLength; i += sizeof(LiteCommData)) {
        // The fromType iterates i up to the point it needs to run again,
        // because internally it copies as much as possible from the string into
        // the byte[] buffer of data. After each run, the resulting data should
        // be sent as an LiteCommType::Append message.

        LiteCommData::fromType(lData, value, i + 1);

        append(property, lData);

        for(std::size_t n = 0; n < sizeof(LiteCommData); ++n)
          lData.byte[n] = '\n';
      }
    }
  }

  template<typename TypeCategory>
  void sendRequestSubscribeUpdate(TypeCategory property,
                                  rcomm::LiteCommType lType)
  {
    using namespace rcomm;

    Message message;

    rregistry::Type type = GetEnumTypeOfEntryClass(property);
    LiteCommProp lProp;

    lProp.property = static_cast<typeof(LiteCommProp::property)>(property);

    // Copy into the message.
    auto it = message.buf.begin();
    (*it++) = static_cast<uint8_t>(lType);
    (*it++) = static_cast<uint8_t>(type);
    for(size_t i = 0; i < sizeof(LiteCommProp); ++i)
      (*it++) = lProp.byte[i];

    send(message);
  }

  template<typename TypeCategory>
  inline void request(TypeCategory property)
  {
    sendRequestSubscribeUpdate(property, rcomm::LiteCommType::Request);
  }
  template<typename TypeCategory>
  inline void subscribe(TypeCategory property)
  {
    sendRequestSubscribeUpdate(property, rcomm::LiteCommType::Subscribe);
    (*m_subscriptionsRemote)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                            [static_cast<std::size_t>(property)] = true;
  }
  template<typename TypeCategory>
  inline void unsubscribe(TypeCategory property)
  {
    sendRequestSubscribeUpdate(property, rcomm::LiteCommType::Unsubscribe);
    (*m_subscriptionsRemote)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                            [static_cast<std::size_t>(property)] = false;
  }

  template<typename TypeCategory>
  bool isSubscribed(TypeCategory property)
  {
    return (*m_subscriptions)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                             [static_cast<uint32_t>(property)];
  }
  template<typename TypeCategory>
  bool isSubscribedTo(TypeCategory property)
  {
    return (*m_subscriptionsRemote)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                                   [static_cast<uint32_t>(property)];
  }
  template<typename TypeCategory>
  inline void append(TypeCategory property, const LiteCommData& data)
  {
    using namespace rcomm;

    Message message;

    LiteCommType lType = LiteCommType::Append;
    rregistry::Type type = GetEnumTypeOfEntryClass(property);
    LiteCommProp lProp;

    lProp.property = static_cast<typeof(LiteCommProp::property)>(property);

    // Copy into the message.
    auto it = message.buf.begin();
    (*it++) = static_cast<uint8_t>(lType);
    (*it++) = static_cast<uint8_t>(type);
    for(size_t i = 0; i < sizeof(LiteCommProp); ++i)
      (*it++) = lProp.byte[i];

    // Write the data to be appended.
    for(size_t i = 0; i < sizeof(LiteCommData); ++i)
      (*it++) = data.byte[i];

    send(message);
  }

  void parseMessage(const Message& msg)
  {
    using namespace rcomm;
    auto it = msg.begin();

    rregistry::Type type = static_cast<rregistry::Type>(*it++);
    if(type >= rregistry::Type::_COUNT)
      return;

    LiteCommProp lProp;
    LiteCommData lData;

    for(size_t i = 0; i < sizeof(LiteCommProp); ++i)
      lProp.byte[i] = (*it++);

    if(lProp.property >= rregistry::GetEntryCount(type))
      return;

    switch(msg.lType()) {
      case LiteCommType::Update:
        for(size_t i = 0; i < sizeof(LiteCommData); ++i)
          lData.byte[i] = (*it++);

        m_acceptProperty = false;
        SetLiteCommDataToRegistry(type, lProp.property, lData, m_registry);
        m_acceptProperty = true;

        break;
      case LiteCommType::Append:
        // Currently, only rregistry::String needs to be appended because of the
        // varying length. This is why only the rregistry::Type::String case is
        // handled in this case.
        for(size_t i = 0; i < 8; ++i)
          lData.byte[i] = (*it++);

        if(type == rregistry::Type::String) {
          auto str = m_registry->getPtrFromArray(
            static_cast<rregistry::String>(lProp.property));
#ifdef LRT_STRING_TYPE_STD
          std::size_t it = str->find('\0');
          for(std::size_t i = 0; i < sizeof(LiteCommData); ++i, ++it)
            (*str)[it] = lData.byte[i];
#else
          auto it = std::find(str[0], str[std::strlen(str)], '\0')
            std::copy(lData.byte[0], lData.byte[sizeof(lData)], it);
#endif
        }
        break;
      case LiteCommType::Request:
        LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER(
          type, lProp.property, setBack, this);
        break;
      case LiteCommType::Subscribe:
        (*m_subscriptions)[static_cast<std::size_t>(type)][lProp.property] =
          true;
        break;
      case LiteCommType::Unsubscribe:
        (*m_subscriptions)[static_cast<std::size_t>(type)][lProp.property] =
          false;
        break;
      default:
        // Type not supported or invalid.
        return;
    }
  }

  protected:
  template<class TypeCategory>
  void setBack(TypeCategory property, LiteCommAdapter* self = nullptr)
  {
    set(property, m_registry->get(property));
  }

  std::shared_ptr<RegistryClass> m_registry;

  bool m_acceptProperty = true;

  std::unique_ptr<rregistry::SubscriptionMap> m_subscriptions;
  std::unique_ptr<rregistry::SubscriptionMap> m_subscriptionsRemote =
    rregistry::InitSubscriptionMap(false);
};
}
}

#endif
