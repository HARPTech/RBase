#ifndef LRT_RREGISTRY_DETAIL_HPP
#define LRT_RREGISTRY_DETAIL_HPP

#include "Entries.hpp"
#include "TypeConverter.hpp"
#include <string.h>

namespace lrt {
namespace rregistry {
enum class TypeTraits
{
  None = 0,
  Numeric = 1 << 1,
  Integer = 1 << 2,
  Signed = 1 << 3,
  Unsigned = 1 << 4,
  Boolean = 1 << 5,
  FloatingPoint = 1 << 6,
  String = 1 << 7,
  Long = 1 << 8
};

constexpr inline TypeTraits
operator|(TypeTraits a, TypeTraits b)
{
  return TypeTraits(int(a) | int(b));
}
constexpr inline bool operator&(TypeTraits a, TypeTraits b)
{
  return bool(int(a) & int(b));
}

struct TypeDetail
{
  const char* name;
  const char* description;
  const char* sqlType;
  TypeTraits traits;
  Type type;
};

template<typename TypeCategory>
struct EntryDetail
{
  const char* name;
  const char* brief;
  const char* detailed;
  typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type def;
  typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type min;
  typename rregistry::GetValueTypeOfEntryClass<TypeCategory>::type max;
};
struct EntryDetailLite
{
  const char* name;
  const char* brief;
  const char* detailed;
};

static constexpr TypeDetail TypeDetail[static_cast<size_t>(Type::_COUNT)] = {
  { "Int8",
    "8 bit wide signed integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Signed | TypeTraits::Integer,
    Type::Int8 },
  { "Int16",
    "16 bit wide signed integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Signed | TypeTraits::Integer,
    Type::Int16 },
  { "Int32",
    "32 bit wide signed integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Signed | TypeTraits::Integer,
    Type::Int32 },
  { "Int64",
    "64 bit wide signed integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Signed | TypeTraits::Integer |
      TypeTraits::Long,
    Type::Int64 },
  { "Uint8",
    "8 bit wide unsigned integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Unsigned | TypeTraits::Integer,
    Type::Uint8 },
  { "Uint16",
    "16 bit wide unsigned integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Unsigned | TypeTraits::Integer,
    Type::Uint16 },
  { "Uint32",
    "32 bit wide unsigned integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Unsigned | TypeTraits::Integer,
    Type::Uint32 },
  { "Uint64",
    "64 bit wide unsigned integer.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Unsigned | TypeTraits::Integer |
      TypeTraits::Long,
    Type::Uint64 },
  { "Float",
    "32 bit wide floating point number.",
    "REAL",
    TypeTraits::Numeric | TypeTraits::FloatingPoint | TypeTraits::Signed,
    Type::Float },
  { "Double",
    "64 bit wide floating point number.",
    "REAL",
    TypeTraits::Numeric | TypeTraits::FloatingPoint | TypeTraits::Signed,
    Type::Double },
  { "Bool",
    "Binary true/false value.",
    "INT",
    TypeTraits::Numeric | TypeTraits::Boolean,
    Type::Bool },
  { "String",
    "String containing multiple letters.",
    "TEXT",
    TypeTraits::String,
    Type::String },
};

constexpr static const struct TypeDetail&
GetTypeDetail(Type type)
{
  return TypeDetail[static_cast<size_t>(type)];
}

#define LRT_RREGISTRY_ENTRYDETAIL_ARR(CLASS) \
  static constexpr EntryDetail<CLASS>        \
    CLASS##Detail[static_cast<size_t>(CLASS::_COUNT)]

LRT_RREGISTRY_ENTRYDETAIL_ARR(Int8) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Int16) = {
  { "MVMT_STEER_DIRECTION",
    "Direct steering direction.",
    "",
    0,
    -32767,
    32767 },
  { "MVMT_FORWARD_VELOCITY",
    "Direct movement velocity.",
    "",
    0,
    -32767,
    32767 },
  { "MVMT_MOTOR_PWM_FL",
    "Feedback value of RBreakout",
    "This gives the operator a detailed "
    "feedback about the arduino-calculated desired march speed. Note: A value "
    "of -255 does NOT correspond with a pwm-value of 255, as the signal levels "
    "in reverse-direction-mode have to be reversed as the other side of the "
    "complementary halfbridge is now conducting.",
    0,
    -255,
    255 },
  { "MVMT_MOTOR_PWM_FR",
    "Feedback value of RBreakout",
    "This gives the operator a detailed "
    "feedback about the arduino-calculated desired march speed. Note: A value "
    "of -255 does NOT correspond with a pwm-value of 255, as the signal levels "
    "in reverse-direction-mode have to be reversed as the other side of the "
    "complementary halfbridge is now conducting.",
    0,
    -255,
    255 },
  { "MVMT_MOTOR_PWM_RL",
    "Feedback value of RBreakout",
    "This gives the operator a detailed "
    "feedback about the arduino-calculated desired march speed. Note: A value "
    "of -255 does NOT correspond with a pwm-value of 255, as the signal levels "
    "in reverse-direction-mode have to be reversed as the other side of the "
    "complementary halfbridge is now conducting.",
    0,
    -255,
    255 },
  { "MVMT_MOTOR_PWM_RR",
    "Feedback value of RBreakout",
    "This gives the operator a detailed "
    "feedback about the arduino-calculated desired march speed. Note: A value "
    "of -255 does NOT correspond with a pwm-value of 255, as the signal levels "
    "in reverse-direction-mode have to be reversed as the other side of the "
    "complementary halfbridge is now conducting.",
    0,
    -255,
    255 },
  { "MVMT_SERVO_OUTPUT_PWM_FL",
    "Real PWM output of the servo routines.",
    "",
    0,
    -550,
    550 },
  { "MVMT_SERVO_OUTPUT_PWM_FR",
    "Real PWM output of the servo routines.",
    "",
    0,
    -550,
    550 },
  { "MVMT_SERVO_OUTPUT_PWM_RL",
    "Real PWM output of the servo routines.",
    "",
    0,
    -550,
    550 },
  { "MVMT_SERVO_OUTPUT_PWM_RR",
    "Real PWM output of the servo routines.",
    "",
    0,
    -550,
    550 },
};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Int32) = {
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_FL_X",
    "Position of Servo, (top) X-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and in main moving direction, unit mm.",
    350,
    1,
    5000 },
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_FL_Y",
    "Position of Servo, (top) Y-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and +90° to main moving direction, "
    "unit mm.",
    300,
    1,
    5000 },
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_FR_X",
    "Position of Servo, (top) X-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and in main moving direction, unit mm.",
    350,
    1,
    5000 },
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_FR_Y",
    "Position of Servo, (top) Y-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and +90° to main moving direction, "
    "unit mm.",
    300,
    1,
    5000 },
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_RL_X",
    "Position of Servo, (top) X-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and in main moving direction, unit mm.",
    350,
    1,
    5000 },
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_RL_Y",
    "Position of Servo, (top) Y-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and +90° to main moving direction, "
    "unit mm.",
    300,
    1,
    5000 },
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_RR_X",
    "Position of Servo, (top) X-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and in main moving direction, unit mm.",
    350,
    1,
    5000 },
  { "GEOM_DISTANCE_ANGLE_POINT_CENTER_RR_Y",
    "Position of Servo, (top) Y-Component",
    "Distance between rover center and "
    "servo drive side axis in flat view and +90° to main moving direction, "
    "unit mm.",
    300,
    1,
    5000 },
};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Int64) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint8) = {
  { "MVMT_MOTOR_FL_CURRENT",
    "Measured Voltage above sensFET current shunt FL.",
    "",
    0,
    0,
    255 },
  { "MVMT_MOTOR_FR_CURRENT",
    "Measured Voltage above sensFET current shunt FR.",
    "",
    0,
    0,
    255 },
  { "MVMT_MOTOR_RL_CURRENT",
    "Measured Voltage above sensFET current shunt RL.",
    "",
    0,
    0,
    255 },
  { "MVMT_MOTOR_RR_CURRENT",
    "Measured Voltage above sensFET current shunt RR.",
    "",
    0,
    0,
    255 },
  { "MVMT_SERVO_FL_POSITION",
    "PWM command for Servo control",
    "Value zero "
    "corresponds to 1ms of pulse width, moving servo "
    "to position 0°, whereas a value of 255 moves to "
    "servo maximum angle.",
    128,
    0,
    255 },
  { "MVMT_SERVO_FR_POSITION",
    "PWM command for Servo control",
    "Value zero "
    "corresponds to 1ms of pulse width, moving servo "
    "to position 0°, whereas a value of 255 moves to "
    "servo maximum angle.",
    128,
    0,
    255 },
  { "MVMT_SERVO_RL_POSITION",
    "PWM command for Servo control",
    "Value zero "
    "corresponds to 1ms of pulse width, moving servo "
    "to position 0°, whereas a value of 255 moves to "
    "servo maximum angle.",
    128,
    0,
    255 },
  { "MVMT_SERVO_RR_POSITION",
    "PWM command for Servo control",
    "Value zero "
    "corresponds to 1ms of pulse width, moving servo "
    "to position 0°, whereas a value of 255 moves to "
    "servo maximum angle.",
    128,
    0,
    255 },
  { "REGULATION_KERNEL_FREQUENCY",
    "Frequency of the Regulation Kernel in Hz.",
    "The frequency determines, how often the main loop in the regulation "
    "kernel is run through in one second. Higher frequencies generally mean "
    "higher accuracy but also higher system load.",
    60,
    0,
    255 },
  { "MVMT_SERVO_MAX_DELTA",
    "Maximal change for servo position in one timestep.",
    "",
    5,
    1,
    30 },
  { "MVMT_SERVO_MAX_VELOCITY",
    "Maximal speed of servo position change.",
    "",
    20,
    1,
    50 },
};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint16) = {
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FL",
    "Ratio current through main Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup",
    138,
    0,
    65535 },
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FR",
    "Ratio current through main Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup",
    138,
    0,
    65535 },
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RL",
    "Ratio current through main Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup",
    138,
    0,
    65535 },
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RR",
    "Ratio current through main Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup",
    138,
    0,
    65535 },
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FL_AUX",
    "Ratio current through auxiliary Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup. There may "
    "not be a butterworth filtering element",
    138,
    0,
    65535 },
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_FR_AUX",
    "Ratio current through auxiliary Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup. There may "
    "not be a butterworth filtering element",
    138,
    0,
    65535 },
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RL_AUX",
    "Ratio current through auxiliary Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup. There may "
    "not be a butterworth filtering element",
    138,
    0,
    65535 },
  { "EEC_MOTORCURRENT_TO_SHUNTVOLTAGE_CALIBRATION_RR_AUX",
    "Ratio current through auxiliary Highside-FET to ADC-Voltage",
    "This is a "
    "constant value, yet it has to be calibrated at first startup. There may "
    "not be a butterworth filtering element",
    138,
    0,
    65535 }
};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint32) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Uint64) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Float) = { { "MVMT_SPEED_MULTIPLIER",
                                           "Factor for general movement speed.",
                                           "",
                                           1,
                                           0,
                                           100 } };
LRT_RREGISTRY_ENTRYDETAIL_ARR(Double) = {};
LRT_RREGISTRY_ENTRYDETAIL_ARR(Bool) = {
  { "TEST_RBREAKOUT_COMMUNICATION",
    "Test the communication channel between RMaster and RBrreakout.",
    "",
    false,
    false,
    true },
  { "TEST_PIPE_COMMUNICATION",
    "Test the communication to and from pipe connections.",
    "",
    false,
    false,
    true },
  { "REDY_RBREAKOUT",
    "Mark RBreakout as ready, should be received from RBreakout.",
    "",
    false,
    false,
    true },
  { "MVMT_PARKBREAK",
    "Parkbreak status",
    "True means that all halfbridges "
    "short to ground, generating a low-impedance eddy "
    "current break.",
    false,
    false,
    true },
  { "BRST_RBREAKOUT_MOVEMENT",
    "Send movement updates as a burst package to RBReakout",
    "",
    false,
    false,
    true },
  { "PERS_ENABLE",
    "Enable persistency policy for the registry.",
    "",
    false,
    false,
    true },
  { "PERS_ACTIVE",
    "Marks the state (active or inactive) of the registry persistency policy.",
    "",
    false,
    false,
    true },
};
static constexpr EntryDetailLite
  StringDetail[static_cast<size_t>(String::_COUNT)] = {
    { "VERS_ARDUINO_PROG",
      "Arduino version (Git Hash)",
      "The Git hash of the installed version on the Arduino." }
  };

#define LRT_RREGISTRY_NAMECONVERTER_GENERATOR(CLASS)                 \
  CLASS static GetValueOfName##CLASS(const char* name)               \
  {                                                                  \
    for(size_t i = 0; i < static_cast<size_t>(CLASS::_COUNT); ++i) { \
      if(strcmp(name, CLASS##Detail[i].name) == 0)                   \
        return static_cast<CLASS>(i);                                \
    }                                                                \
    return CLASS::_COUNT;                                            \
  }

LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
  LRT_RREGISTRY_NAMECONVERTER_GENERATOR)

#define LRT_RREGISTRY_NAMECONVERTER_CASE(CLASS)                \
  case Type::CLASS:                                            \
    return static_cast<uint32_t>(GetValueOfName##CLASS(name)); \
    break;

int32_t static GetValueOfName(Type type, const char* name)
{
  switch(type) {
    LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
      LRT_RREGISTRY_NAMECONVERTER_CASE)
    default:
      return 0;
  }
}

#define LRT_RREGISTRY_GETNAMEOFENTRYCLASS_CASE(CLASS) \
  case Type::CLASS:                                   \
    return CLASS##Detail[property].name;

inline const char*
GetNameOfEntryClass(Type type, uint32_t property)
{
  switch(type) {
    LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
      LRT_RREGISTRY_GETNAMEOFENTRYCLASS_CASE)
    default:
      return "";
  }
}

#define LRT_RREGISTRY_GETBRIEFDESCRIPTIONOFCLASS_CASE(CLASS) \
  case Type::CLASS:                                          \
    return CLASS##Detail[property].brief;

inline const char*
GetBriefDescriptionOfEntryClass(Type type, uint32_t property)
{
  switch(type) {
    LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
      LRT_RREGISTRY_GETBRIEFDESCRIPTIONOFCLASS_CASE)
    default:
      return "";
  }
}

#define LRT_RREGISTRY_GETDETAILEDDESCRIPTIONOFCLASS_CASE(CLASS) \
  case Type::CLASS:                                             \
    return CLASS##Detail[property].brief;

inline const char*
GetDetailedDescriptionOfEntryClass(Type type, uint32_t property)
{
  switch(type) {
    LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
      LRT_RREGISTRY_GETDETAILEDDESCRIPTIONOFCLASS_CASE)
    default:
      return "";
  }
}

template<typename TypeCategory>
constexpr const EntryDetail<TypeCategory>&
GetEntryDetail(TypeCategory property);

#define LRT_RREGISTRY_GETENTRYDETAIL_SPECIAL(CLASS)                   \
  template<>                                                          \
  constexpr const EntryDetail<lrt::rregistry::CLASS>& GetEntryDetail( \
    lrt::rregistry::CLASS property)                                   \
  {                                                                   \
    return CLASS##Detail[static_cast<size_t>(property)];              \
  }

LRT_RREGISTRY_CPPTYPELIST_HELPER(LRT_RREGISTRY_GETENTRYDETAIL_SPECIAL)

inline std::size_t
GetFullEntryCount()
{
  std::size_t total = 0;
  for(std::size_t i = 0; i < static_cast<std::size_t>(Type::_COUNT); ++i) {
    total += GetEntryCount(static_cast<Type>(i));
  }
  return total;
}

inline Type
GetTypeFromContinousNumber(int i)
{
  std::size_t type = 0;
  while(i >= 0) {
    i -= GetEntryCount(static_cast<Type>(type));
    if(i >= 0)
      ++type;
  }
  return static_cast<Type>(type);
}

inline std::size_t
NormalizeContinousNumber(int i)
{
  std::size_t type = static_cast<std::size_t>(GetTypeFromContinousNumber(i));
  for(std::size_t n = 0; n < type; ++n) {
    i -= GetEntryCount(static_cast<Type>(n));
  }
  return i;
}

inline std::size_t
GetContinuousNumberFromPropertyType(Type type, uint32_t property)
{
  std::size_t i = 0;
  while(property != NormalizeContinousNumber(i) ||
        type != GetTypeFromContinousNumber(i)) {
    ++i;
  }
  return i;
}
}
}

#endif
