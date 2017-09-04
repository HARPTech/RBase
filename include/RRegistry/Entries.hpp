#ifndef LRT_RREGISTRY_ENTRIES_HPP
#define LRT_RREGISTRY_ENTRIES_HPP

#include <stddef.h>

namespace lrt {
namespace rregistry {

enum class Int8
{
};
enum class Int16
{
  MVMT_STEER_DIRECTION = 10, ///< Defines the steering direction of the device.
                             /// Default is 2^15 / 2 ( = 16384).
  MVMT_FORWARD_VELOCITY = 11,///< Defines the forward velocity of the main
                             /// motors. Default is 2^15 / 2 ( = 16384).
};
enum class Int32
{
};
enum class Int64
{
};
enum class Uint8
{
};
enum class Uint16
{
};
enum class Uint32
{
};
enum class Uint64
{
};
enum class Float
{
  MVMT_SPEED_MULTIPLIER = 10,///< Every movement gets multiplied by this factor.
                             /// It defines the "responsiveness" of the device.
};
enum class Double
{
};
enum class Bool
{
  TEST_RBREAKOUT_COMMUNICATION = 0,
};
enum class String
{
};
}
}

#endif
