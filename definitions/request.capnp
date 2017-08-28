@0x9aac2e48dc988834;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("lrt::rcomm");

struct Request {
  positions @0 :List(import "position.capnp".RegistryPosition);
}
