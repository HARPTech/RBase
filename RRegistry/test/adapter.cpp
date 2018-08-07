#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#define LRT_RCORE_DEBUG
#include <RComm/LiteCommDropper.hpp>
#include <RCore/defaults.h>
#include <RRegistry/Registry.hpp>
#include <RRegistry/TypeConverter.hpp>

#include <iostream>
#include <thread>
using std::cout;
using std::endl;

LRT_RCOMM_UNIVERSAL_DEFINITIONS()

using namespace lrt;
using namespace lrt::rregistry;

const int checkValue = 100;

class Adapter : public rregistry::Registry::Adapter
{
  public:
  Adapter(std::shared_ptr<Registry> registry, bool subscribed = false)
    : LiteCommAdapter::LiteCommAdapter(registry, subscribed)
  {
    m_rcomm_handle = rcomm_create();
    rcomm_set_transmit_cb(
      m_rcomm_handle,
      [](const uint8_t* data, void* userdata, size_t bytes) {
        assert(data != 0);
        assert(userdata != 0);
        assert(bytes > 0);

        Adapter* adapter = static_cast<Adapter*>(userdata);
        Adapter* target = adapter->m_target;
        if(target) {
          rcomm_parse_bytes(target->m_rcomm_handle, data, bytes);
        }
        //thread_local static rcomm_block_t block;
        //std::copy(data, data + bytes, block.data);
        //block.significant_bytes = bytes;
        // cout << "TRANSMIT " << rcomm_to_str(&block) << endl;
        return LRT_RCORE_OK;
      },
      (void*)this);
    rcomm_set_accept_cb(
      m_rcomm_handle,
      [](rcomm_block_t* block, void* userdata) {
        Adapter* adapter = static_cast<Adapter*>(userdata);

        rcomm_transfer_block_to_tb(
          adapter->m_rcomm_handle, block, adapter->m_transmit_buffer.get());

        // cout << "ACCEPT " << rcomm_to_str(block) << endl;

        return LRT_RCORE_OK;
      },
      (void*)this);
  }
  virtual ~Adapter() { rcomm_free(m_rcomm_handle); }

  virtual void send(lrt_rcore_transmit_buffer_entry_t* entry,
                    rcomm::Reliability = rcomm::DefaultReliability)
  {
    rcomm_send_tb_entry(m_rcomm_handle, entry);
  }

  void setTarget(Adapter* target) { m_target = target; }

  private:
  Adapter* m_target = nullptr;
  rcomm_handle_t* m_rcomm_handle;
};

#define SET_VARIABLE_HELPER(CLASS)                                            \
  case Type::CLASS:                                                           \
    adapter1.set(                                                             \
      static_cast<CLASS>(property),                                           \
      static_cast<typename rregistry::GetValueTypeOfEntryClass<CLASS>::type>( \
        checkValue),                                                          \
      reliability);                                                           \
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

  rcomm::Reliability reliability = rcomm::DefaultReliability;

  // Subscribe to all values by default.
  Adapter adapter1(registry, true);
  Adapter adapter2(registry, true);

  // Set targets for testing purposes
  adapter1.setTarget(&adapter2);
  adapter2.setTarget(&adapter1);

  // Transmit variables.
  rregistry::Type upToType = Type::_COUNT;

  for(size_t type = 0; type < static_cast<size_t>(upToType); ++type) {
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
  for(size_t type = 0; type < static_cast<size_t>(upToType); ++type) {
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
  // adapter1.set(String::VERS_ARDUINO_PROG, testString);

  // REQUIRE(registry->get(String::VERS_ARDUINO_PROG) == testString);

  // Test lossy sends

  reliability = rcomm::Reliability::BasicDelivery;

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
}
