#ifndef LRT_RCOMM_LITECOMMADAPTER_HPP
#define LRT_RCOMM_LITECOMMADAPTER_HPP

#include "LiteComm.hpp"
#include "LiteCommDropper.hpp"
#include "LiteCommMessage.hpp"
#include "LiteCommReliability.hpp"
#include <RRegistry/DebuggingDataStore.hpp>
#include <RRegistry/SubscriptionMap.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <memory>

namespace lrt {
namespace rcomm {

enum MessageParseError
{
  Success,
  MessageDropped,
  MessageTypeInvalid,
  RegistryTypeInvalid,
};

template<class RegistryClass>
class LiteCommAdapter
{
  public:
  using Message = LiteCommMessage;

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
    , m_subscriptionsRemote(rregistry::InitSubscriptionMap(false))
    , m_adapterName(adapterName)
    , m_debuggingDataStore(debuggingDataStore)
    , m_adapterId(adapterId)
  {
    setDropperPolicy(std::make_unique<LiteCommDropperPolicy>());
  }
  virtual ~LiteCommAdapter()
  {
    if(m_registry)
      m_registry->removeAdapter(this);
  }

  void setDropperPolicy(LiteCommDropperPolicyPtr policy)
  {
    m_dropperPolicy = std::move(policy);
  };

  /**
   * @brief Sends the message over the transmission method implemented by child
   * classes.
   *
   * This method is called, when there are Messages to be sent. The only thing
   * implementations have to do to be able to send messages is to implement the
   * transmission of Message structs for the length Message::length()
   */
  virtual void send(const Message& msg,
                    const Reliability = DefaultReliability) = 0;

  template<class TypeCategory>
  void set(
    TypeCategory property,
    typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type value,
    Reliability reliability = DefaultReliability)
  {
    using namespace rcomm;
    using namespace rregistry;

    if(!m_acceptProperty)
      return;

    if(m_dictionary && GetEnumTypeOfEntryClass(property) == Type::Bool) {
      // This variant is currently only used between RMaster and RBReakout,
      // which is why there is no receive logic implemented.

      // Check if this is the trigger property.
      const LiteCommDictEntry* entry = GetDictEntryForTriggerProperty(
        static_cast<Bool>(property), m_dictionary);
      if(entry != nullptr) {
        // This transforms the current update to a burst update.
        m_sendMessage.buf[0] = entry->id | (1 << 7);
        m_sendMessage.buf[1] = 0;
        for(std::size_t i = 0; i < entry->length; ++i) {
          const LiteCommDictEntryHandler* handler = &entry->handlers[i];
          // Append each individual property to the message.
          switch(handler->propertyType) {
            case Type::Uint8:
              m_sendMessage.buf[i + 2] =
                m_registry->get(static_cast<Uint8>(handler->prop));
              break;
            case Type::Int8: {
              int8_t val = m_registry->get(static_cast<Int8>(handler->prop));
              if(val < 0) {
                m_sendMessage.setSingleBit(i + 8);
                val = -val;
              }
              m_sendMessage.buf[i + 2] = static_cast<uint8_t>(val);
              break;
            }
            case Type::Int16: {
              int16_t val = m_registry->get(static_cast<Int16>(handler->prop));
              if(val < 0) {
                m_sendMessage.setSingleBit(i + 8);
                val = -val;
              }
              // Stop overflow errors.
              if(abs(val) > 255)
                val = 255;
              m_sendMessage.buf[i + 2] = static_cast<uint8_t>(val);
              break;
            }
            default:
              // Not supported!
              return;
              break;
          }
        }
        m_sendMessage.explicit_length = entry->length + 2;

        // Message is ready to be sent!
        send(m_sendMessage, reliability);

        // Set the burst request to false.
        m_acceptProperty = false;
        m_registry->set(static_cast<Bool>(property), false);
        m_acceptProperty = true;

        return;
      }
    }

    // Check if the property has been subscribed by the current adapter.
    if(!(*m_subscriptions)[static_cast<std::size_t>(
         rregistry::GetEnumTypeOfEntryClass(property))]
                          [static_cast<uint32_t>(property)])
      return;

    LiteCommData lData;

    LiteCommData::fromType(lData, value);

    // Copy into the message.
    m_sendMessage.setLType(LiteCommType::Update, reliability);

    m_sendMessage.setType(GetEnumTypeOfEntryClass(property));
    m_sendMessage.setProperty(static_cast<uint16_t>(property));

    auto it = m_sendMessage.begin();
    for(size_t i = 0; i < 8; ++i)
      (*it++) = lData.byte[i];

    send(m_sendMessage, reliability);

    // Check if the message is a string and handle the string uniquely.
    if(GetEnumTypeOfEntryClass(property) == rregistry::Type::String) {
      std::size_t stringLength = lData.Int32;
      for(std::size_t i = 0; i < stringLength; i += 8) {
        // The fromType iterates i up to the point it needs to run again,
        // because internally it copies as much as possible from the string
        // into the byte[] buffer of data. After each run, the resulting data
        // should be sent as an LiteCommType::Append message.

        LiteCommData::fromType(lData, value, i + 1);

        append(property, lData, reliability);

        for(std::size_t n = 0; n < sizeof(LiteCommData); ++n)
          lData.byte[n] = '\n';
      }
    }
  }

  template<typename TypeCategory>
  void sendRequestSubscribeUpdate(TypeCategory property,
                                  rcomm::LiteCommType lType,
                                  Reliability reliability = DefaultReliability)
  {
    using namespace rcomm;

    // Copy into the message.
    m_sendMessage.setLType(lType, reliability);
    m_sendMessage.setType(GetEnumTypeOfEntryClass(property));
    m_sendMessage.setProperty(
      static_cast<decltype(LiteCommProp::property)>(property));

    send(m_sendMessage, reliability);
  }

  template<typename TypeCategory>
  inline void request(TypeCategory property,
                      Reliability reliability = DefaultReliability)
  {
    sendRequestSubscribeUpdate(
      property, rcomm::LiteCommType::Request, reliability);
  }
  template<typename TypeCategory>
  inline void subscribe(TypeCategory property,
                        Reliability reliability = DefaultReliability)
  {
    sendRequestSubscribeUpdate(
      property, rcomm::LiteCommType::Subscribe, reliability);
    (*m_subscriptionsRemote)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                            [static_cast<std::size_t>(property)] = true;
  }
  template<typename TypeCategory>
  inline void unsubscribe(TypeCategory property,
                          Reliability reliability = DefaultReliability)
  {
    sendRequestSubscribeUpdate(
      property, rcomm::LiteCommType::Unsubscribe, reliability);
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
  void setSubscribed(TypeCategory property, bool subscribed)
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
  inline void append(TypeCategory property,
                     const LiteCommData& data,
                     Reliability reliability = DefaultReliability)
  {
    using namespace rcomm;

    m_sendMessage.setLType(LiteCommType::Append, reliability);
    m_sendMessage.setType(GetEnumTypeOfEntryClass(property));
    m_sendMessage.setProperty(
      static_cast<decltype(LiteCommProp::property)>(property));

    // Write the data to be appended.
    auto it = m_sendMessage.begin();
    for(size_t i = 0; i < 8; ++i)
      (*it++) = data.byte[i];

    send(m_sendMessage, reliability);
  }

  MessageParseError parseMessage(const Message& msg)
  {
    using namespace rcomm;

    rregistry::Type type = msg.getType();
    if(type >= rregistry::Type::_COUNT)
      return MessageTypeInvalid;

    if(m_dropperPolicy) {
      if(m_dropperPolicy->shouldBeDropped(msg)) {
        return MessageDropped;
      }
    }

    thread_local LiteCommData lData;
    thread_local LiteCommDebugPos lDebugPos;

    uint16_t clientId = m_adapterId;
    if(msg.lType() == LiteCommType::Lossy)
      clientId = msg.getClientId();

    uint16_t property = msg.getProperty();

    if(property >= rregistry::GetEntryCount(type))
      return RegistryTypeInvalid;

    auto it = msg.begin();

    switch(msg.realLType()) {
      case LiteCommType::Update:
        for(size_t i = 0; i < 8; ++i) {
          lData.byte[i] = (*it++);
          m_lastData.byte[i] = lData.byte[i];
        }

        m_acceptProperty = false;
        SetLiteCommDataToRegistry(type, property, lData, m_registry);
        m_acceptProperty = true;

        // If there is a consistency policy in place, write the new value
        // into that.
        if(m_registry->m_persistencyPolicy) {
          auto persistencyPolicy = m_registry->m_persistencyPolicy.get();

          if(type == rregistry::Type::Bool &&
             property == static_cast<uint16_t>(rregistry::Bool::PERS_ENABLE)) {
            if(lData.Bool && !m_registry->get(rregistry::Bool::PERS_ACTIVE)) {
              persistencyPolicy->enable();
              m_registry->set(rregistry::Bool::PERS_ACTIVE, true);
            } else if(!lData.Bool &&
                      m_registry->get(rregistry::Bool::PERS_ACTIVE)) {
              persistencyPolicy->disable();
              m_registry->set(rregistry::Bool::PERS_ACTIVE, false);
            }
          }
          if(m_registry->get(rregistry::Bool::PERS_ACTIVE)) {
            persistencyPolicy->push(clientId, type, property, lData);
          }
        }

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
            static_cast<rregistry::String>(property));
#ifdef LRT_STRING_TYPE_STD
          std::size_t it = str->find('\0');
          for(std::size_t i = 0; i < 8; ++i, ++it)
            (*str)[it] = lData.byte[i];
#else
          auto it = std::find(str[0], str[std::strlen(str)], '\0');
          std::copy(lData.byte[0], lData.byte[sizeof(lData)], it);
#endif
        }
        break;
      case LiteCommType::Request:
        LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER(type, property, setBack, this);
        break;
      case LiteCommType::Subscribe:
        (*m_subscriptions)[static_cast<std::size_t>(type)][property] = true;
        break;
      case LiteCommType::Unsubscribe:
        (*m_subscriptions)[static_cast<std::size_t>(type)][property] = false;
        break;
      case LiteCommType::Debug:
        if(m_debuggingDataStore) {
          // Read the debugging data, which is an int32 value with 4 byte
          // length.
          lDebugPos.read(it);

          switch(m_debugState) {
            case DebugMode::WaitingForSet:
              // The received event must be a Get-notification, because no Set
              // has been issued.
              // Generate the data which is received. This should be the value
              // of the property at the time of this request.
              LiteCommData::fromRegistry(m_registry, lData, type, property);

              m_debuggingDataStore->getOp(m_adapterName,
                                          m_adapterId,
                                          type,
                                          property,
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
                                          property,
                                          m_lastData,
                                          lDebugPos.pos);
              break;
          }
        }
        break;
      default:
        // Type not supported or invalid.
        return MessageTypeInvalid;
    }
    callMessageCallback(msg.lType(), type, property);
    return Success;
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

  void remoteUnsubscribeFromAll()
  {
    for(auto type : *m_subscriptions) {
      for(auto entry : type) {
        entry = false;
      }
    }
  }

  void setAcceptProperty(bool accept) { m_acceptProperty = accept; }

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

  std::unique_ptr<rregistry::SubscriptionMap<bool>> m_subscriptions;
  std::unique_ptr<rregistry::SubscriptionMap<bool>> m_subscriptionsRemote;

  DebugMode m_debugState = DebugMode::WaitingForSet;
  rregistry::Type m_lastSetType;
  uint16_t m_lastSetProperty;
  LiteCommData m_lastData;
  int m_adapterId = 0;
  const rcomm::LiteCommDict* m_dictionary = nullptr;

  Message m_sendMessage;

  MsgCallback m_messageCallback[static_cast<size_t>(LiteCommType::_COUNT)] = {
    nullptr
  };

  LiteCommDropperPolicyPtr m_dropperPolicy;
};
}
}

#endif
