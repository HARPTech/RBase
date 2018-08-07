#ifndef LRT_RCOMM_RELIABILITY_HPP
#define LRT_RCOMM_RELIABILITY_HPP

namespace lrt {
namespace rcomm {
enum Reliability
{
  BasicDelivery = (1 << 0),
  Ordered = (1 << 1),
  Acknowledged = (1 << 2)
};
inline Reliability
operator|(Reliability a, Reliability b)
{
  return static_cast<Reliability>(static_cast<int>(a) | static_cast<int>(b));
}
const Reliability DefaultReliability =
  Reliability::Ordered | Reliability::Acknowledged;

inline bool
reliability_contains(Reliability hay, Reliability needle)
{
  return (static_cast<uint8_t>(hay) & static_cast<uint8_t>(needle));
}
}
}

#endif
