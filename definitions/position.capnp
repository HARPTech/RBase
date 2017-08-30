@0xd86e765a5c2d09fb;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("lrt::rcomm");

struct RegistryPosition {
  type : union {
    int8 @0 : Void;
    int16 @1 : Void;
    int32 @2 : Void;
    int64 @3 : Void;
    uint8 @4 : Void;
    uint16 @5 : Void;
    uint32 @6 : Void;
    uint64 @7 : Void;
    float @8 : Void;
    double @9 : Void;
    bool @10 : Void;
    string @11 : Void;
  }
  property @12 : UInt32;
}
