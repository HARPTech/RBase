#ifndef LRT_RCOMM_LITECOMMDICT_RBREAKOUT_HPP
#define LRT_RCOMM_LITECOMMDICT_RBREAKOUT_HPP

#include "LiteComm.hpp"

namespace lrt {
namespace rcomm {

extern LiteCommDict LiteCommDict_RBreakout;

}
}

#endif

#ifdef LITECOMMDICT_RBREAKOUT_INIT
namespace lrt {
namespace rcomm {
LiteCommDict LiteCommDict_RBreakout = { "RBreakout", 1 };

void
LiteCommDict_RBreakout_Init()
{
  LiteCommDictEntry* entries = static_cast<LiteCommDictEntry*>(
    malloc(sizeof(LiteCommDictEntry) * LiteCommDict_RBreakout.length));

  LiteCommDictEntryHandler* handlers1 = static_cast<LiteCommDictEntryHandler*>(
    malloc(sizeof(LiteCommDictEntryHandler) * 8));

  handlers1[0] = LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_FL);
  handlers1[1] = LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_FR);
  handlers1[2] = LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_RL);
  handlers1[3] = LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_RR);
  handlers1[4] =
    LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_FL_POSITION);
  handlers1[5] =
    LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_FR_POSITION);
  handlers1[6] =
    LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_RL_POSITION);
  handlers1[7] =
    LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_RR_POSITION);

  entries[0].handlers = handlers1;
  entries[0].length = 8;
  entries[0].trigger = rregistry::Bool::BRST_RBREAKOUT_MOVEMENT;
  entries[0].id = 0;

  LiteCommDict_RBreakout.entries = entries;
}
}
}

#endif
