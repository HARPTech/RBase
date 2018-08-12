#ifndef LRT_RCOMM_LITECOMMDROPPER_HPP
#define LRT_RCOMM_LITECOMMDROPPER_HPP

#include <RRegistry/SubscriptionMap.hpp>
#include <memory>

#include <iostream>

namespace lrt {
namespace rcomm {

class LiteCommDropperPolicy
{
  public:
  LiteCommDropperPolicy() {}
  virtual ~LiteCommDropperPolicy() {}

  virtual bool shouldBeDropped(bool reliable,
                               uint8_t type,
                               uint16_t property,
                               uint16_t seq_number,
                               int adapterId)
  {
    return false;
  }
};

using LiteCommDropperPolicyPtr = std::unique_ptr<LiteCommDropperPolicy>;

class LiteCommDropperLossyPolicy : public LiteCommDropperPolicy
{
  public:
  LiteCommDropperLossyPolicy() {}
  virtual ~LiteCommDropperLossyPolicy() {}

  virtual bool shouldBeDropped(bool reliable,
                               uint8_t type,
                               uint16_t property,
                               uint16_t seq_number,
                               int adapterId)
  {
    auto& entry = (*m_records)[static_cast<std::size_t>(type)]
                              [static_cast<std::size_t>(property)];

    if(seq_number >= entry.sequentNumber) {
      entry.sequentNumber = seq_number;
      return false;
    } else if(entry.adapterId != adapterId) {
      entry.adapterId = adapterId;
      entry.sequentNumber = seq_number;
      return false;
    } else {
      return true;
    }
  }

  private:
  struct EntryRecord
  {
    EntryRecord(uint16_t init)
    {
      sequentNumber = init;
      adapterId = init;
    }
    uint16_t sequentNumber = 0;
    uint16_t adapterId = 0;
  };

  std::unique_ptr<rregistry::SubscriptionMap<EntryRecord>> m_records =
    rregistry::InitSubscriptionMap<EntryRecord>(0);
};
}
}

#endif
