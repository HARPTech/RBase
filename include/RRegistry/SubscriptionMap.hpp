#ifndef LRT_RREGISTRY_SUBSCRIPTIONMAP_HPP
#define LRT_RREGISTRY_SUBSCRIPTIONMAP_HPP

#include "Entries.hpp"
#include "TypeConverter.hpp"
#include <array>
#include <memory>
#include <vector>

namespace lrt {
namespace rregistry {
using SubscriptionMap =
  std::array<std::vector<bool>, static_cast<std::size_t>(Type::_COUNT)>;

inline std::unique_ptr<SubscriptionMap>
InitSubscriptionMap()
{
  std::unique_ptr<SubscriptionMap> subscriptions =
    std::make_unique<SubscriptionMap>();
  for(std::size_t i = 0; i < subscriptions->size(); ++i) {
    (*subscriptions)[i].assign(GetEntryCount(static_cast<Type>(i)), false);
  }
  return subscriptions;
}
}
}

#endif
