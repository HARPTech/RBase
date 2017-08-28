@0xcc0360d955ca925d;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("lrt::rcomm");

struct RegistryValue {
  union {
    int8 @0 : Int8;
    int16 @1 : Int16;
    int32 @2 : Int32;
    int64 @3 : Int64;
    uint8 @4 : UInt8;
    uint16 @5 : UInt16;
    uint32 @6 : UInt32;
    uint64 @7 : UInt64;
    float @8 : Float32;
    double @9 : Float64;
    string @10 : Text;
  }
}
