#ifndef LRT_RCOMM_LITECOMMADAPTER_HPP
#define LRT_RCOMM_LITECOMMADAPTER_HPP

#include <RComm/LiteComm.hpp>
#include <RRegistry/SubscriptionMap.hpp>
#include <RRegistry/TypeConverter.hpp>
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
    static const std::size_t maxLength = 14;

    using Buffer = std::array<char, maxLength>;

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
          return 14;
        case LiteCommType::Request:
        case LiteCommType::Subscribe:
        case LiteCommType::Unsubscribe:
          return 6;
      }
    }

    /**
     * @brief Returns the remaining byte count for this message, based its type.
     */
    inline std::size_t remainingBytes(const typename Buffer::iterator it) const
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

    lProp.property = static_cast<uint32_t>(property);
    LiteCommData::fromType(lData, value);

    // Copy into the message.
    auto it = message.buf.begin();
    (*it++) = static_cast<char>(lType);
    (*it++) = static_cast<char>(type);
    for(size_t i = 0; i < sizeof(LiteCommProp); ++i)
      (*it++) = lProp.byte[i];
    for(size_t i = 0; i < sizeof(LiteCommData); ++i)
      (*it++) = lData.byte[i];

    // Check if the message is a string and handle the string uniquely.
    if(type == rregistry::Type::String) {
      LiteCommData data;
      for(int32_t i = 0; i < lData.Int32; ++i) {
        data.byte[i % sizeof(LiteCommData)] = value[i];
        if(i > 0 && i % sizeof(LiteCommData) == 0) {
          append(property, data);
          // Reset data.
          for(std::size_t n = 0; n < sizeof(LiteCommData); ++n) data.byte[n] = 0;
        }
      }
    }

    send(message);
  }

  template<typename TypeCategory>
  void sendRequestSubscribeUpdate(TypeCategory property,
                                  rcomm::LiteCommType lType)
  {
    using namespace rcomm;

    Message message;

    rregistry::Type type = GetEnumTypeOfEntryClass(property);
    LiteCommProp lProp;

    lProp.property = static_cast<uint32_t>(property);

    // Copy into the message.
    auto it = message.buf.begin();
    (*it++) = static_cast<char>(lType);
    (*it++) = static_cast<char>(type);
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
    LiteCommData lData;

    lProp.property = static_cast<uint32_t>(property);

    // Copy into the message.
    auto it = message.buf.begin();
    (*it++) = static_cast<char>(lType);
    (*it++) = static_cast<char>(type);
    for(size_t i = 0; i < 4; ++i)
      (*it++) = lProp.byte[i];

    // Write the data to be appended.
    for(size_t i = 0; i < sizeof(LiteCommData); ++i)
      (*it++) = data.byte[i];
    for(size_t i = 0; i < sizeof(LiteCommData); ++i)
      (*it++) = lData.byte[i];

    send(message);
  }

  void parseMessage(const Message& msg)
  {
    using namespace rcomm;
    // Start after the type.
    auto it = msg.buf.begin();

    LiteCommType lType = static_cast<LiteCommType>(*it++);
    rregistry::Type type = static_cast<rregistry::Type>(*it++);
    LiteCommProp lProp;
    LiteCommData lData;

    for(size_t i = 0; i < 4; ++i)
      lProp.byte[i] = (*it++);

    switch(lType) {
      case LiteCommType::Update:
        for(size_t i = 0; i < 8; ++i)
          lData.byte[i] = (*it++);

        m_acceptProperty = false;
        switch(type) {
          LRT_RREGISTRY_CPPTYPELIST_HELPER(
            LRT_RCOMM_LITECOMMADAPTER_PARSEMESSAGE_SET_CASE)
          case rregistry::Type::String:
            break;
          default:
            break;
        }
        m_acceptProperty = true;

        break;
      case LiteCommType::Request:
        SetLiteCommDataToRegistry(type, lProp.property, lData, m_registry);
        break;
      case LiteCommType::Subscribe:
        (*m_subscriptions)[static_cast<std::size_t>(type)][lProp.property] =
          true;
        break;
      case LiteCommType::Unsubscribe:
        (*m_subscriptions)[static_cast<std::size_t>(type)][lProp.property] =
          false;
        break;
    }
  }

  protected:
  std::shared_ptr<RegistryClass> m_registry;

  bool m_acceptProperty = true;

  std::unique_ptr<rregistry::SubscriptionMap> m_subscriptions;
  std::unique_ptr<rregistry::SubscriptionMap> m_subscriptionsRemote =
    rregistry::InitSubscriptionMap(false);
};
}
}

#endif
