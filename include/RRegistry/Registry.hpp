#ifndef LRT_RREGISTRY_REGISTRY_HPP
#define LRT_RREGISTRY_REGISTRY_HPP

#include "Detail.hpp"
#include "Entries.hpp"
#include "RegistryReceiver.hpp"
#include "TypeConverter.hpp"
#include <RComm/LiteCommAdapter.hpp>
#include <algorithm>
#include <array>
#include <vector>

namespace lrt {
namespace rregistry {

#define LRT_RREGISTRY_REGISTRY_INITDEFAULTS_HELPER(CLASS)  \
  case Type::CLASS:                                        \
    set(static_cast<CLASS>(property),                      \
        CLASS##Detail[static_cast<size_t>(property)].def); \
    break;

class Registry
{
  public:
  Registry() { initDefaults(); }
  ~Registry() {}

  using Adapter = rcomm::LiteCommAdapter<Registry>;
  using AdapterPtr = std::shared_ptr<Adapter>;

  // The receivers are a collection of raw pointers to make easy registering
  // easier to do. For example in class constructors and destructors.
  using ReceiverPtr = RegistryReceiver*;

  void initDefaults()
  {
    for(size_t type = 0; type < static_cast<size_t>(rregistry::Type::_COUNT);
        ++type) {
      for(size_t property = 0; property < rregistry::GetEntryCount(
                                            static_cast<rregistry::Type>(type));
          ++property) {
        switch(static_cast<Type>(type)) {
          LRT_RREGISTRY_CPPTYPELIST_HELPER(
            LRT_RREGISTRY_REGISTRY_INITDEFAULTS_HELPER)
          default:
            // Nothing to do in other cases.
            break;
        }
      }
    }
  }

  template<class TypeCategory>
  inline void set(
    TypeCategory property,
    typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type value)
  {
    setToArray(property, value);
    for(auto adapter : m_adapters)
      adapter->set(property, value);
    for(auto receiver : m_receivers)
      receiver->onUpdate(GetEnumTypeOfEntryClass(property),
                         static_cast<uint32_t>(property));
  }

  template<class TypeCategory>
  inline auto get(TypeCategory property) ->
    typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type
  {
    return getFromArray(property);
  }

  void registerAdapter(AdapterPtr adapter) { m_adapters.push_back(adapter); }
  void removeAdapter(AdapterPtr adapter)
  {
    std::remove(m_adapters.begin(), m_adapters.end(), adapter);
  }
  void removeAdapter(Adapter* adapter)
  {
    std::remove_if(m_adapters.begin(),
                   m_adapters.end(),
                   [adapter](AdapterPtr ptr) { return ptr.get() == adapter; });
  }

  void addReceiver(ReceiverPtr receiver) { m_receivers.push_back(receiver); }
  void removeReceiver(ReceiverPtr receiver)
  {
    std::remove(m_receivers.begin(), m_receivers.end(), receiver);
  }

  const std::vector<AdapterPtr>& adapters() { return m_adapters; }
  const std::vector<ReceiverPtr>& receivers() { return m_receivers; }

  private:
  std::vector<AdapterPtr> m_adapters;
  std::vector<ReceiverPtr> m_receivers;

  template<typename TypeCategory>
  using ValueArray =
    std::array<typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type,
               rregistry::GetEntryCount(rregistry::GetEnumTypeOfEntryClass(
                 static_cast<TypeCategory>(0)))>;

#define LRT_RREGISTRY_REGISTRY_MEMBER_HELPER(CLASS) \
  Registry::ValueArray<CLASS> m_##CLASS##Array;

  LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
    LRT_RREGISTRY_REGISTRY_MEMBER_HELPER)

  template<typename TypeCategory>
  inline void setToArray(
    TypeCategory property,
    typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type value);

  template<typename TypeCategory>
  inline auto getFromArray(TypeCategory property) ->
    typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type;
};

#define LRT_RREGISTRY_REGISTRY_GETARRAYFORTYPE_HELPER(CLASS)         \
  template<>                                                         \
  inline void Registry::setToArray(                                  \
    CLASS property,                                                  \
    typename rregistry::GetValueTypeOfEntryClass<CLASS>::type value) \
  {                                                                  \
    m_##CLASS##Array[static_cast<std::size_t>(property)] = value;    \
  }                                                                  \
  template<>                                                         \
  inline auto Registry::getFromArray(CLASS property)                 \
    ->typename rregistry::GetValueTypeOfEntryClass<CLASS>::type      \
  {                                                                  \
    return m_##CLASS##Array[static_cast<std::size_t>(property)];     \
  }

LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
  LRT_RREGISTRY_REGISTRY_GETARRAYFORTYPE_HELPER)
}
}

#endif
