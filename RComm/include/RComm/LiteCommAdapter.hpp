#ifndef LRT_RCOMM_LITECOMMADAPTER_HPP
#define LRT_RCOMM_LITECOMMADAPTER_HPP

#include <RComm/LiteComm.hpp>
#include <RRegistry/DebuggingDataStore.hpp>
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
    std::size_t explicit_length = 0;

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
      switch(buf[0]) {
        case static_cast<uint8_t>(LiteCommType::Update):
        case static_cast<uint8_t>(LiteCommType::Append):
          return 12;
        case static_cast<uint8_t>(LiteCommType::Request):
        case static_cast<uint8_t>(LiteCommType::Subscribe):
        case static_cast<uint8_t>(LiteCommType::Unsubscribe):
          return 4;
        case static_cast<uint8_t>(LiteCommType::Debug):
          return 8;
        default:
          // Must be a custom length.
          return explicit_length;
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

    void setSingleBit(uint8_t bit)
    {
      uint8_t& byte = buf[bit / 8];
      byte |= (1 << (7 - (bit % 8)));
    }
  };

  /**
   * @brief Defines the internal debug mode of the adapter.
   *
   * The debug mode is the current state of receiving LiteCommType::Debug
   * messages containing data useful for debugging the entire system and the
   * decisions made by it. It is the current state of the internal
   * debugging-related state machine, so to speak.
   */
  enum DebugMode
  {
    WaitingForSet,
    SetReceived,
    _COUNT
  };

  using MsgCallback = void (*)(const char*,
                               LiteCommType lType,
                               rregistry::Type,
                               uint16_t property);

  LiteCommAdapter(
    std::shared_ptr<RegistryClass> registry,
    bool subscribed = false,
    std::shared_ptr<rregistry::DebuggingDataStore> debuggingDataStore = nullptr,
    const char* adapterName = "Unnamed Adapter",
    int adapterId = 0)
    : m_registry(registry)
    , m_subscriptions(rregistry::InitSubscriptionMap(subscribed))
    , m_adapterName(adapterName)
    , m_debuggingDataStore(debuggingDataStore)
    , m_adapterId(adapterId)
  {
  }
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
    using namespace rregistry;

    if(!m_acceptProperty)
      return;

    thread_local Message message;

    if(m_dictionary && GetEnumTypeOfEntryClass(property) == Type::Bool) {
      // This variant is currently only used between RMaster and RBReakout,
      // which is why there is no receive logic implemented.

      // Check if this is the trigger property.
      const LiteCommDictEntry* entry = GetDictEntryForTriggerProperty(
        static_cast<Bool>(property), m_dictionary);
      if(entry != nullptr) {
        // This transforms the current update to a burst update.
        message.buf[0] = entry->id | (1 << 7);
        message.buf[1] = 0;
        for(std::size_t i = 0; i < entry->length; ++i) {
          const LiteCommDictEntryHandler* handler = &entry->handlers[i];
          // Append each individual property to the message.
          switch(handler->propertyType) {
            case Type::Uint8:
              message.buf[i + 2] =
                m_registry->get(static_cast<Uint8>(handler->prop));
              break;
            case Type::Int8: {
              int8_t val = m_registry->get(static_cast<Int8>(handler->prop));
              if(val < 0) {
                message.setSingleBit(i + 8);
                val = -val;
              }
              message.buf[i + 2] = static_cast<uint8_t>(val);
              break;
            }
            case Type::Int16: {
              int16_t val = m_registry->get(static_cast<Int16>(handler->prop));
              if(val < 0) {
                message.setSingleBit(i + 8);
                val = -val;
              }
              message.buf[i + 2] = static_cast<uint8_t>(val);
              break;
            }
            default:
              // Not supported!
              return;
              break;
          }
        }
        message.explicit_length = entry->length + 2;

        // Message is ready to be sent!
        send(message);

        // Set the burst request to false.
        m_acceptProperty = false;
        m_registry->set(static_cast<Bool>(property), false);
        m_acceptProperty = true;
      }
      return;
    }

    // Check if the property has been subscribed by the current adapter.
    if(!(*m_subscriptions)[static_cast<std::size_t>(
         rregistry::GetEnumTypeOfEntryClass(property))]
                          [static_cast<uint32_t>(property)])
      return;

    LiteCommType lType = LiteCommType::Update;
    rregistry::Type type = GetEnumTypeOfEntryClass(property);
    LiteCommProp lProp;
    LiteCommData lData;

    lProp.property = static_cast<uint16_t>(property);
    LiteCommData::fromType(lData, value);

    // Copy into the message.
    auto it = message.buf.begin();
    (*it++) = static_cast<uint8_t>(lType);
    (*it++) = static_cast<uint8_t>(type);
    for(size_t i = 0; i < 2; ++i)
      (*it++) = lProp.byte[i];
    for(size_t i = 0; i < 8; ++i)
      (*it++) = lData.byte[i];

    send(message);

    // Check if the message is a string and handle the string uniquely.
    if(type == rregistry::Type::String) {
      std::size_t stringLength = lData.Int32;
      for(std::size_t i = 0; i < stringLength; i += 8) {
        // The fromType iterates i up to the point it needs to run again,
        // because internally it copies as much as possible from the string
        // into the byte[] buffer of data. After each run, the resulting data
        // should be sent as an LiteCommType::Append message.

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
    for(size_t i = 0; i < 2; ++i)
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

#define LRT_RCOMM_LITECOMMADAPTER_REQUESTBYTYPEVAL_CASE(CLASS) \
  case rregistry::Type::CLASS:                                 \
    request(static_cast<rregistry::CLASS>(property));          \
    break;

  inline void requestByTypeVal(rregistry::Type type, uint16_t property)
  {
    switch(type) {
      LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
        LRT_RCOMM_LITECOMMADAPTER_REQUESTBYTYPEVAL_CASE)
    }
  }
#define LRT_RCOMM_LITECOMMADAPTER_SUBSCRIBEBYTYPEVAL_CASE(CLASS) \
  case rregistry::Type::CLASS:                                   \
    subscribe(static_cast<rregistry::CLASS>(property));          \
    break;

  inline void subscribeByTypeVal(rregistry::Type type, uint16_t property)
  {
    switch(type) {
      LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
        LRT_RCOMM_LITECOMMADAPTER_SUBSCRIBEBYTYPEVAL_CASE)
    }
  }
#define LRT_RCOMM_LITECOMMADAPTER_UNSUBSCRIBEBYTYPEVAL_CASE(CLASS) \
  case rregistry::Type::CLASS:                                     \
    unsubscribe(static_cast<rregistry::CLASS>(property));          \
    break;

  inline void unsubscribeByTypeVal(rregistry::Type type, uint16_t property)
  {
    switch(type) {
      LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
        LRT_RCOMM_LITECOMMADAPTER_UNSUBSCRIBEBYTYPEVAL_CASE)
    }
  }

  template<typename TypeCategory>
  bool isSubscribed(TypeCategory property)
  {
    return (*m_subscriptions)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                             [static_cast<uint32_t>(property)];
  }
  template<typename TypeCategory>
  bool setSubscribed(TypeCategory property, bool subscribed)
  {
    (*m_subscriptions)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                      [static_cast<uint32_t>(property)] = subscribed;
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
    for(size_t i = 0; i < 2; ++i)
      (*it++) = lProp.byte[i];

    // Write the data to be appended.
    for(size_t i = 0; i < 8; ++i)
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

    thread_local LiteCommProp lProp;
    thread_local LiteCommData lData;
    thread_local LiteCommDebugPos lDebugPos;

    for(size_t i = 0; i < 2; ++i)
      lProp.byte[i] = (*it++);

    if(lProp.property >= rregistry::GetEntryCount(type))
      return;

    switch(msg.lType()) {
      case LiteCommType::Update:
        for(size_t i = 0; i < 8; ++i) {
          lData.byte[i] = (*it++);
          m_lastData.byte[i] = lData.byte[i];
        }

        m_acceptProperty = false;
        SetLiteCommDataToRegistry(type, lProp.property, lData, m_registry);
        m_acceptProperty = true;

        // There was a set (= update), so the internal debugging state has to
        // be updated to reflect this change.
        m_debugState = DebugMode::SetReceived;
        break;
      case LiteCommType::Append:
        // Currently, only rregistry::String needs to be appended because of
        // the varying length. This is why only the rregistry::Type::String
        // case is handled in this case.
        for(size_t i = 0; i < 8; ++i)
          lData.byte[i] = (*it++);

        if(type == rregistry::Type::String) {
          auto str = m_registry->getPtrFromArray(
            static_cast<rregistry::String>(lProp.property));
#ifdef LRT_STRING_TYPE_STD
          std::size_t it = str->find('\0');
          for(std::size_t i = 0; i < 8; ++i, ++it)
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
      case LiteCommType::Debug:
        if(m_debuggingDataStore) {
          // Read the debugging data, which is an int32 value with 4 byte
          // length.
          for(size_t i = 0; i < 4; ++i)
            lDebugPos.byte[i] = (*it++);

          switch(m_debugState) {
            case DebugMode::WaitingForSet:
              // The received event must be a Get-notification, because no Set
              // has been issued.
              // Generate the data which is received. This should be the value
              // of the property at the time of this request.
              LiteCommData::fromRegistry(
                m_registry, lData, type, lProp.property);

              m_debuggingDataStore->getOp(m_adapterName,
                                          m_adapterId,
                                          type,
                                          lProp.property,
                                          lData,
                                          lDebugPos.pos);
              break;
            case DebugMode::SetReceived:
              // The received event must be a Set-notification, because the
              // last received update was a set too. After this notification,
              // the internal state will immediately be switched to
              // DebugMode::WaitingForSet again to process any following
              // Get-notifications.
              m_debugState = DebugMode::WaitingForSet;

              m_debuggingDataStore->setOp(m_adapterName,
                                          m_adapterId,
                                          type,
                                          lProp.property,
                                          m_lastData,
                                          lDebugPos.pos);
              break;
          }
        }
        break;
      default:
        // Type not supported or invalid.
        return;
    }
    callMessageCallback(msg.lType(), type, lProp.property);
  }

  void setMessageCallback(LiteCommType type, MsgCallback cb)
  {
    m_messageCallback[static_cast<size_t>(type)] = cb;
  }
  inline void callMessageCallback(LiteCommType lType,
                                  rregistry::Type type,
                                  uint16_t property)
  {
    auto cb = m_messageCallback[static_cast<size_t>(lType)];
    if(cb)
      cb(m_adapterName, lType, type, property);
  }

  void setAdapterName(const char* name) { m_adapterName = name; }
  void setAdapterId(int adapterId) { m_adapterId = adapterId; }

  const char* adapterName() { return m_adapterName; }
  int adapterId() { return m_adapterId; }

  protected:
  template<class TypeCategory>
  void setBack(TypeCategory property, LiteCommAdapter* self = nullptr)
  {
    set(property, m_registry->get(property));
  }

  std::shared_ptr<RegistryClass> m_registry;
  std::shared_ptr<rregistry::DebuggingDataStore> m_debuggingDataStore;

  bool m_acceptProperty = true;
  const char* m_adapterName;

  std::unique_ptr<rregistry::SubscriptionMap> m_subscriptions;
  std::unique_ptr<rregistry::SubscriptionMap> m_subscriptionsRemote =
    rregistry::InitSubscriptionMap(false);

  DebugMode m_debugState = DebugMode::WaitingForSet;
  rregistry::Type m_lastSetType;
  uint16_t m_lastSetProperty;
  LiteCommData m_lastData;
  int m_adapterId = 0;
  const rcomm::LiteCommDict* m_dictionary = nullptr;

  MsgCallback m_messageCallback[static_cast<size_t>(LiteCommType::_COUNT)] = {
    nullptr
  };
};
}
}

#endif
