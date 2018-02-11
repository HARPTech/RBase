#ifndef LRT_RCOMM_LITECOMMDICT_RBREAKOUT_HPP
#define LRT_RCOMM_LITECOMMDICT_RBREAKOUT_HPP

#include "LiteComm.hpp"

namespace lrt {
namespace rcomm {
const LiteCommDict LiteCommDict_RBreakout = {
  "RBreakout",
  1,
  { 0,
    8,
    rregistry::Bool::BRST_RBREAKOUT_MOVEMENT,
    { LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_FL),
      LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_FR),
      LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_RL),
      LiteCommDictEntryHandler(rregistry::Int16::MVMT_MOTOR_PWM_RR),
      LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_FL_POSITION),
      LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_FR_POSITION),
      LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_RL_POSITION),
      LiteCommDictEntryHandler(rregistry::Uint8::MVMT_SERVO_RR_POSITION) } }
};
}
}

#endif
