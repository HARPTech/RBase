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
  String
};

typedef uint32_t EntryID;

enum class Int8
{
  _COUNT
};
enum class Int16
{
  MVMT_STEER_DIRECTION = 0, ///< Defines the steering direction of the device.
                             /// Default is 2^15 / 2 ( = 16384).
  MVMT_FORWARD_VELOCITY = 1,///< Defines the forward velocity of the main
                             /// motors. Default is 2^15 / 2 ( = 16384).
  _COUNT
};
enum class Int32
{
  _COUNT
};
enum class Int64
{
  _COUNT
};
enum class Uint8
{
  _COUNT
};
enum class Uint16
{
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
  _COUNT
};
enum class String
{
  LOG_RBREAKOUT = 0,
  _COUNT
};
}
}

#endif
