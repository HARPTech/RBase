@0x89eebea4f705e752;

# The Packet type defines a generic packet sent over the wire. It contains all
# other possible variations and required data points and relies on Cap'n Proto
# to condense it as densely as possible.

struct Packet {
  union {
    update @0 :import "update.capnp".Update;
    request @1 :import "request.capnp".Request;
  }
}
