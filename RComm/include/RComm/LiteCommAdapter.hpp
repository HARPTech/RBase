#ifndef LRT_RCOMM_LITECOMMADAPTER_HPP
#define LRT_RCOMM_LITECOMMADAPTER_HPP

#include "LiteComm.hpp"
#include "LiteCommDropper.hpp"
#include "LiteCommReliability.hpp"
#include <RCore/librcp/message.h>
#include <RCore/transmit_buffer.h>
#include <RRegistry/SubscriptionMap.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <algorithm>
#include <array>
#include <cassert>
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
                               lrt_rcp_message_type_t lType,
                               rregistry::Type,
                               uint16_t property,
                               RegistryClass* registry);

#define LRT_RCOMM_LITECOMMADAPTER_TB_FINISHED_UPDATE_CASE(CLASS)               \
  case rregistry::Type::CLASS:                                                 \
    adapter->m_registry->set(                                                  \
      static_cast<rregistry::CLASS>(entry->property),                          \
      lrt_librcp_##CLASS##_from_data(                                          \
        (const uint8_t*)entry->data->s,                                        \
        sizeof(rregistry::GetValueTypeOfEntryClass<rregistry::CLASS>::type))); \
    break;
#define LRT_RCOMM_LITECOMMADAPTER_TB_FINISHED_REQUEST_CASE(CLASS) \
  case rregistry::Type::CLASS:                                    \
    adapter->m_registry->setBack(                                 \
      static_cast<rregistry::CLASS>(entry->property));            \
    break;

  LiteCommAdapter(::std::shared_ptr<RegistryClass> registry,
                  bool subscribed = false,
                  const char* adapterName = "Unnamed Adapter",
                  int adapterId = 0)
    : m_registry(registry)
    , m_adapterName(adapterName)
    , m_subscriptions(::lrt::rregistry::InitSubscriptionMap(subscribed))
    , m_subscriptionsRemote(::lrt::rregistry::InitSubscriptionMap(false))
    , m_adapterId(adapterId)
  {
    setDropperPolicy(::std::make_unique<LiteCommDropperPolicy>());
    m_transmit_buffer.reset(lrt_rcore_transmit_buffer_init());
    lrt_rcore_transmit_buffer_set_data_ready_cb(
      m_transmit_buffer.get(),
      [](lrt_rcore_transmit_buffer_entry_t* entry, void* userdata) {
        LiteCommAdapter<RegistryClass>* adapter =
          static_cast<LiteCommAdapter<RegistryClass>*>(userdata);
        assert(entry != 0);
        assert(entry->data != 0);
        assert(entry->data->l >= 0);
        adapter->send(entry, adapter->m_currentReliability);
        return LRT_RCORE_OK;
      },
      (void*)this);
    lrt_rcore_transmit_buffer_set_finished_cb(
      m_transmit_buffer.get(),
      [](lrt_rcore_transmit_buffer_entry_t* entry, void* userdata) {
        LiteCommAdapter<RegistryClass>* adapter =
          static_cast<LiteCommAdapter<RegistryClass>*>(userdata);

        // Call message callbacks.
        adapter->callMessageCallback(entry->message_type,
                                     static_cast<rregistry::Type>(entry->type),
                                     entry->property);

        switch(entry->message_type) {
          case LRT_RCP_MESSAGE_TYPE_UPDATE: {
            if(adapter->m_dropperPolicy) {
              if(adapter->m_dropperPolicy->shouldBeDropped(
                   entry->reliable,
                   entry->type,
                   entry->property,
                   entry->seq_number,
                   adapter->m_adapterId)) {
                return LRT_RCORE_OK;
              }
            }
            adapter->m_hadUpdate = true;

            adapter->m_acceptProperty = false;
            if(adapter->m_registry) {
              switch(static_cast<rregistry::Type>(entry->type)) {
                LRT_RREGISTRY_CPPTYPELIST_HELPER(
                  LRT_RCOMM_LITECOMMADAPTER_TB_FINISHED_UPDATE_CASE)
                default:
                  break;
              }
            }
            adapter->m_acceptProperty = true;
            break;
          }
          case LRT_RCP_MESSAGE_TYPE_REQUEST: {
            LRT_RREGISTRY_TYPETOTEMPLATEFUNCHELPER(
              static_cast<rregistry::Type>(entry->type),
              entry->property,
              adapter->setBack,
              adapter);
            break;
          }
          case LRT_RCP_MESSAGE_TYPE_SUBSCRIBE: {
            (*adapter->m_subscriptions)[entry->type][entry->property] = true;
            break;
          }
          case LRT_RCP_MESSAGE_TYPE_UNSUBSCRIBE: {
            (*adapter->m_subscriptions)[entry->type][entry->property] = false;
            break;
          }
        }
        return LRT_RCORE_OK;
      },
      (void*)this);
  }
  virtual ~LiteCommAdapter()
  {
    m_subscriptions.reset();
    m_subscriptionsRemote.reset();
    if(m_registry)
      m_registry->removeAdapter(this);
  }

  void setDropperPolicy(LiteCommDropperPolicyPtr policy)
  {
    m_dropperPolicy = std::move(policy);
  };

  bool hadUpdate() { return m_hadUpdate; }
  void resetHadUpdate() { m_hadUpdate = false; }

  /**
   * @brief Sends the message over the transmission method implemented by child
   * classes.
   *
   * This method is called, when there are Messages to be sent. The only thing
   * implementations have to do to be able to send messages is to implement the
   * transmission of Message structs for the length Message::length()
   */
  virtual void send(lrt_rcore_transmit_buffer_entry_t* entry,
                    const Reliability = DefaultReliability) = 0;

#define LRT_RCOMM_LITECOMMADAPTER_SET_CASE(CLASS)                    \
  case rregistry::Type::CLASS:                                       \
    lrt_rcore_transmit_buffer_send_##CLASS(                          \
      m_transmit_buffer.get(),                                       \
      type,                                                          \
      static_cast<uint16_t>(property),                               \
      value,                                                         \
      reliability_contains(reliability, Reliability::Acknowledged)); \
    break;

  template<class TypeCategory>
  inline void set(
    TypeCategory property,
    typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type value,
    Reliability reliability = DefaultReliability)
  {
    using namespace rcomm;
    using namespace rregistry;

    if(!m_acceptProperty)
      return;

    // Check if the property has been subscribed by the current adapter.
    uint8_t type =
      static_cast<uint8_t>(rregistry::GetEnumTypeOfEntryClass(property));

    if(!(*m_subscriptions)[type][static_cast<uint16_t>(property)])
      return;

    m_currentReliability = reliability;

    switch(rregistry::GetEnumTypeOfEntryClass(property)) {
      LRT_RREGISTRY_CPPTYPELIST_HELPER(LRT_RCOMM_LITECOMMADAPTER_SET_CASE)
      default:
        break;
    }
  }

  template<typename TypeCategory>
  void sendRequestSubscribeUpdate(TypeCategory property,
                                  lrt_rcp_message_type_t messageType,
                                  Reliability reliability = DefaultReliability)
  {
    m_currentReliability = reliability;
    lrt_rcore_transmit_buffer_send_ctrl(
      m_transmit_buffer.get(),
      static_cast<uint8_t>(rregistry::GetEnumTypeOfEntryClass(property)),
      static_cast<uint16_t>(property),
      messageType,
      reliability_contains(reliability, Reliability::Acknowledged));
  }

  template<typename TypeCategory>
  inline void request(TypeCategory property,
                      Reliability reliability = DefaultReliability)
  {
    sendRequestSubscribeUpdate(
      property, LRT_RCP_MESSAGE_TYPE_REQUEST, reliability);
  }
  template<typename TypeCategory>
  inline void subscribe(TypeCategory property,
                        Reliability reliability = DefaultReliability)
  {
    sendRequestSubscribeUpdate(
      property, LRT_RCP_MESSAGE_TYPE_SUBSCRIBE, reliability);
    (*m_subscriptionsRemote)[static_cast<std::size_t>(
      rregistry::GetEnumTypeOfEntryClass(property))]
                            [static_cast<std::size_t>(property)] = true;
  }
  template<typename TypeCategory>
  inline void unsubscribe(TypeCategory property,
                          Reliability reliability = DefaultReliability)
  {
    sendRequestSubscribeUpdate(
      property, LRT_RCP_MESSAGE_TYPE_UNSUBSCRIBE, reliability);
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
      default:
        break;
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
      default:
        break;
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
      default:
        break;
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

  void setMessageCallback(lrt_rcp_message_type_t type, MsgCallback cb)
  {
    m_messageCallback[static_cast<size_t>(type)] = cb;
  }
  inline void callMessageCallback(lrt_rcp_message_type_t lType,
                                  rregistry::Type type,
                                  uint16_t property)
  {
    auto cb = m_messageCallback[static_cast<size_t>(lType)];
    if(cb)
      cb(m_adapterName,
         lType,
         type,
         property,
         m_registry ? m_registry.get() : nullptr);
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

  bool m_acceptProperty = true;
  const char* m_adapterName;

  std::unique_ptr<rregistry::SubscriptionMap<bool>> m_subscriptions;
  std::unique_ptr<rregistry::SubscriptionMap<bool>> m_subscriptionsRemote;

  int m_adapterId = 0;

  MsgCallback m_messageCallback[static_cast<size_t>(
    LRT_RCP_MESSAGE_TYPE_COUNT)] = { nullptr };

  LiteCommDropperPolicyPtr m_dropperPolicy;

  RCore::TransmitBufferPtr m_transmit_buffer;
  Reliability m_currentReliability;
  bool m_hadUpdate = false;
};
}
}

#endif
