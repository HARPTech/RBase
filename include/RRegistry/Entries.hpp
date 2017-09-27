#ifndef LRT_RREGISTRY_ENTRIES_HPP
#define LRT_RREGISTRY_ENTRIES_HPP

#include <stddef.h>
#include <stdint.h>

namespace lrt {
namespace rregistry {

enum class Type
{
  Int8,
  Int16,
  Int32,
  Int64,
  Uint8,
  Uint16,
  Uint32,
  Uint64,
  Float,
  Double,
  Bool,
  String,
  _COUNT
};

typedef uint32_t EntryID;

enum class Int8
{
  _COUNT
};
enum class Int16
{
  MVMT_STEER_DIRECTION = 0, ///< Defines the steering direction of the device.
  MVMT_FORWARD_VELOCITY = 1,///< Defines the forward velocity of the main
  MVMT_MOTOR_PWM_FL = 2,
  MVMT_MOTOR_PWM_FR = 3,
  MVMT_MOTOR_PWM_RL = 4,
  MVMT_MOTOR_PWM_RR = 5,
  _COUNT
};
enum class Int32
{
  GEOM_DISTANCE_ANGLE_POINT_CENTER_FL_X = 0,
  GEOM_DISTANCE_ANGLE_POINT_CENTER_FL_Y = 1,
  GEOM_DISTANCE_ANGLE_POINT_CENTER_FR_X = 2,
  GEOM_DISTANCE_ANGLE_POINT_CENTER_FR_Y = 3,
  GEOM_DISTANCE_ANGLE_POINT_CENTER_RL_X = 4,
  GEOM_DISTANCE_ANGLE_POINT_CENTER_RL_Y = 5,
  GEOM_DISTANCE_ANGLE_POINT_CENTER_RR_X = 6,
  GEOM_DISTANCE_ANGLE_POINT_CENTER_RR_Y = 7,
  _COUNT
};
enum class Int64
{
  _COUNT
};
enum class Uint8
{
  MVMT_MOTOR_FL_CURRENT = 0,
  MVMT_MOTOR_FR_CURRENT = 1,
  MVMT_MOTOR_RL_CURRENT = 2,
  MVMT_MOTOR_RR_CURRENT = 3,
  MVMT_SERVO_FL_POSITION = 4,
  MVMT_SERVO_FR_POSITION = 5,
  MVMT_SERVO_RL_POSITION = 6,
  MVMT_SERVO_RR_POSITION = 7,
  _COUNT
};
enum class Uint16
{
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FL5 = 0,
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FR = 1,
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RL = 2,
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RR = 3,
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FL_AUX = 4,
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FR_AUX = 5,
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RL_AUX = 6,
  EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RR_AUX = 7,
  _COUNT
};
enum class Uint32
{
  _COUNT
};
enum class Uint64
{
  _COUNT
};
enum class Float
{
  MVMT_SPEED_MULTIPLIER = 0,///< Every movement gets multiplied by this factor.
                             /// It defines the "responsiveness" of the device.
  _COUNT
};
enum class Double
{
  _COUNT
};
enum class Bool
{
  TEST_RBREAKOUT_COMMUNICATION = 0,
  REDY_RBREAKOUT = 1,
  //is it important that the following items are on that position? If so, where to put MVMT_PARKBREAK?
  COMM_CONNECT = 2,
  COMM_DISCONNECT = 3,

  _COUNT
};
enum class String
{
  _COUNT
};
}
}

#endif
