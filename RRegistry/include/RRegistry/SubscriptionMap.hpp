#ifndef LRT_RREGISTRY_SUBSCRIPTIONMAP_HPP
#define LRT_RREGISTRY_SUBSCRIPTIONMAP_HPP

#include "Entries.hpp"
#include "TypeConverter.hpp"
#include <array>
#include <memory>
#include <vector>

namespace lrt {
namespace rregistry {
template<typename Store = bool>
using SubscriptionMap =
  std::array<std::vector<Store>, static_cast<std::size_t>(Type::_COUNT)>;

template<typename Store = bool>
inline std::unique_ptr<SubscriptionMap<Store>>
InitSubscriptionMap(Store defaultValue = false)
{
  std::unique_ptr<SubscriptionMap<Store>> subscriptions =
    std::make_unique<SubscriptionMap<Store>>();
  for(std::size_t i = 0; i < subscriptions->size(); ++i) {
    (*subscriptions)[i].assign(GetEntryCount(static_cast<Type>(i)),
                               defaultValue);
  }
  return subscriptions;
}
}
}

#endif
