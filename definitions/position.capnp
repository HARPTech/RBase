@0xd86e765a5c2d09fb;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("lrt::rcomm");

struct RegistryPosition {
  category @0 : UInt16;
  property @1 : UInt16;
}
