#ifndef LRT_RCOMM_LITECOMMDROPPER_HPP
#define LRT_RCOMM_LITECOMMDROPPER_HPP

#include "LiteCommMessage.hpp"
#include <RRegistry/SubscriptionMap.hpp>

namespace lrt {
namespace rcomm {

class LiteCommDropperLossyPolicy
{
  protected:
  LiteCommDropperLossyPolicy() {}
  ~LiteCommDropperLossyPolicy() {}

  bool shouldBeDropped(const LiteCommMessage& message)
  {
    switch(message.lType()) {
      case LiteCommType::Lossy: {
        // Analyse the packet. If the sequent number is smaller than the
        // internal one, it should be dropped.
        auto& entry =
          (*m_records)[static_cast<std::size_t>(message.getType())]
                      [static_cast<std::size_t>(message.getProperty())];
        auto clientId = message.getClientId();
        auto sequentNumber = message.getSequentNumber();

        if(sequentNumber > entry.sequentNumber) {
          entry.sequentNumber = sequentNumber;
          return false;
        } else if(entry.clientId != clientId) {
          entry.clientId = clientId;
          entry.sequentNumber = sequentNumber;
          return false;
        } else {
          return true;
        }
      }
      default:
        return false;
    }
  }

  private:
  struct EntryRecord
  {
    EntryRecord(uint16_t init)
    {
      sequentNumber = init;
      clientId = init;
    }
    uint16_t sequentNumber = 0;
    uint16_t clientId = 0;
  };

  std::unique_ptr<rregistry::SubscriptionMap<EntryRecord>> m_records =
    rregistry::InitSubscriptionMap<EntryRecord>(0);
};
}
}

#endif
