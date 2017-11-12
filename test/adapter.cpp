#include "catch.hpp"

#include <RRegistry/Registry.hpp>
#include <RRegistry/TypeConverter.hpp>

#include <iostream>
using std::cout;
using std::endl;

using namespace lrt;
using namespace lrt::rregistry;

const int checkValue = 100;

class Adapter : public Registry::Adapter
{
  public:
  Adapter(std::shared_ptr<Registry> registry, bool subscribed = false)
    : Registry::Adapter(registry, subscribed)
  {}
  ~Adapter() {}

  virtual void send(const Message& msg)
  {
    /*

      Commented out, because this is not needed in normal testing.

    cout << "Transmitting: ";

    for(Message::Buffer::const_iterator it = msg.buf.begin();
        msg.remainingBytes(it) > 0;
        ++it) {
      cout << it << "(" << static_cast<int>(*it) << ") ";
    }

    cout << endl;

    */
    if(m_target)
      m_target->parseMessage(msg);
  }

  void setTarget(Adapter* target) { m_target = target; }

  private:
  Adapter* m_target = nullptr;
};

#define SET_VARIABLE_HELPER(CLASS)                                            \
  case Type::CLASS:                                                           \
    adapter1.set(                                                             \
      static_cast<CLASS>(property),                                           \
      static_cast<typename rregistry::GetValueTypeOfEntryClass<CLASS>::type>( \
        checkValue));                                                         \
    break;

#define CHECK_VARIABLE_HELPER(CLASS)                                          \
  case Type::CLASS:                                                           \
    REQUIRE(                                                                  \
      registry->get(static_cast<CLASS>(property)) ==                          \
      static_cast<typename rregistry::GetValueTypeOfEntryClass<CLASS>::type>( \
        checkValue));                                                         \
    break;

TEST_CASE("rregistry::Registry::Adapter transmissions working.",
          "rcomm, rregistry")
{
  std::shared_ptr<Registry> registry = std::make_shared<Registry>();

  // Subscribe to all values by default.
  Adapter adapter1(registry, true);
  Adapter adapter2(registry, true);

  // Set targets for testing purposes
  adapter1.setTarget(&adapter2);
  adapter2.setTarget(&adapter1);

  // Transmit variables.
  for(size_t type = 0; type < static_cast<size_t>(Type::_COUNT); ++type) {
    for(size_t property = 0;
        property < rregistry::GetEntryCount(static_cast<Type>(type));
        ++property) {
      switch(static_cast<Type>(type)) {
        LRT_RREGISTRY_CPPTYPELIST_HELPER(SET_VARIABLE_HELPER)
        default:
          // Nothing to do in other cases.
          break;
      }
    }
  }

  // Check values.
  for(size_t type = 0; type < static_cast<size_t>(Type::_COUNT); ++type) {
    for(size_t property = 0;
        property < rregistry::GetEntryCount(static_cast<Type>(type));
        ++property) {
      switch(static_cast<Type>(type)) {
        LRT_RREGISTRY_CPPTYPELIST_HELPER(CHECK_VARIABLE_HELPER)
        default:
          // Nothing to do in other cases.
          break;
      }
    }
  }

  // Check string transmissions.

  std::string testString = "This string is very long and should be transmitted "
                           "correctly. The transmission is handled completely "
                           "automatically.";

  // The adapter doesn't set anything to the registry, only the receiving one
  // does.
  adapter1.set(String::VERS_ARDUINO_PROG, testString);

  REQUIRE(registry->get(String::VERS_ARDUINO_PROG) == testString);
}
