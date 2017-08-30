@0xd4c01bfdb3067538;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("lrt::rcomm");

struct Update {
  values @0 :List(import "value.capnp".RegistryValue);
}
