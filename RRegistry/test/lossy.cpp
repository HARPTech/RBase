#include <catch.hpp>

#define LRT_RCORE_DEBUG
#include <RComm/LiteCommDropper.hpp>
#include <RCore/defaults.h>
#include <RRegistry/Registry.hpp>
#include <RRegistry/TypeConverter.hpp>

#include <iostream>
#include <stack>
using std::cout;
using std::endl;

using namespace lrt;
using namespace lrt::rregistry;

const int checkValue = 100;

LRT_RCOMM_UNIVERSAL()

class StackAdapter : public rregistry::Registry::Adapter
{
  public:
  struct StackEntry
  {
    std::array<uint8_t, LRT_RCOMM_BLOCKSIZE> data;
    size_t bytes;
  };

  StackAdapter(std::shared_ptr<Registry> registry, bool subscribed = false)
    : LiteCommAdapter::LiteCommAdapter(registry, subscribed)
  {
    m_rcomm_handle = rcomm_create();

    rcomm_set_transmit_cb(
      m_rcomm_handle,
      [](const uint8_t* data, void* userdata, size_t bytes) {
        StackAdapter* adapter = static_cast<StackAdapter*>(userdata);
        auto entry = std::make_unique<StackEntry>();
        std::copy(data, data + bytes - 1, entry->data.begin());
        entry->bytes = bytes;
        adapter->m_stack.push(std::move(entry));
        return LRT_RCORE_OK;
      },
      (void*)this);
    rcomm_set_accept_cb(
      m_rcomm_handle,
      [](rcomm_block_t* block, void* userdata) {
        StackAdapter* adapter = static_cast<StackAdapter*>(userdata);

        rcomm_transfer_block_to_tb(
          adapter->m_rcomm_handle, block, adapter->m_transmit_buffer.get());

        return LRT_RCORE_OK;
      },
      (void*)this);
  }
  virtual ~StackAdapter() { rcomm_free(m_rcomm_handle); }

  virtual void send(lrt_rcore_transmit_buffer_entry_t* entry,
                    rcomm::Reliability = rcomm::DefaultReliability)
  {
    if(entry->type == static_cast<uint8_t>(rregistry::Type::Int16)) {
      if(entry->property ==
         static_cast<uint16_t>(rregistry::Int16::MVMT_FORWARD_VELOCITY)) {
        entry->seq_number = m_seq_num_forward++;
      }
      if(entry->property ==
         static_cast<uint16_t>(rregistry::Int16::MVMT_STEER_DIRECTION)) {
        entry->seq_number = m_seq_num_steering++;
      }
    }
    rcomm_send_tb_entry(m_rcomm_handle, entry);
  }

  uint16_t m_seq_num_forward = 255;
  uint16_t m_seq_num_steering = 255;

  void sendStack()
  {
    while(!m_stack.empty()) {
      auto entry = std::move(m_stack.top());
      m_stack.pop();

      if(m_target) {
        rcomm_parse_bytes(
          m_target->m_rcomm_handle, entry->data.data(), entry->bytes);
      }
    }
  }

  void setTarget(StackAdapter* target) { m_target = target; }

  private:
  StackAdapter* m_target = nullptr;
  std::stack<std::unique_ptr<StackEntry>> m_stack;
  rcomm_handle_t* m_rcomm_handle;
};

void
sendPackets(StackAdapter& adapter, rcomm::Reliability reliability)
{
  adapter.set(rregistry::Int16::MVMT_FORWARD_VELOCITY, 20, reliability);
  adapter.set(rregistry::Int16::MVMT_FORWARD_VELOCITY, 10, reliability);
  adapter.set(rregistry::Int16::MVMT_STEER_DIRECTION, 20, reliability);
  adapter.set(rregistry::Int16::MVMT_FORWARD_VELOCITY, 30, reliability);
  adapter.set(rregistry::Int16::MVMT_STEER_DIRECTION, 10, reliability);
  adapter.set(rregistry::Int16::MVMT_FORWARD_VELOCITY, 20, reliability);
  adapter.set(rregistry::Int16::MVMT_STEER_DIRECTION, 70, reliability);
  adapter.set(rregistry::Int16::MVMT_FORWARD_VELOCITY, 30, reliability);
  adapter.sendStack();
}

TEST_CASE("Delayed packets get ignored.", "rcomm, lossy")
{
  std::shared_ptr<Registry> registry = std::make_shared<Registry>();

  rcomm::Reliability reliability = rcomm::DefaultReliability;

  // Subscribe to all values by default.
  StackAdapter adapter1(nullptr, true);
  StackAdapter adapter2(registry, true);

  // Set targets for testing purposes
  adapter1.setTarget(&adapter2);
  adapter2.setTarget(&adapter1);

  // Check lossy transmissions
  reliability = rcomm::Reliability::BasicDelivery;

  sendPackets(adapter1, reliability);

  REQUIRE(registry->get(rregistry::Int16::MVMT_FORWARD_VELOCITY) == 20);
  REQUIRE(registry->get(rregistry::Int16::MVMT_STEER_DIRECTION) == 20);

  // With the dropper policy in place, unordered packets should be dropped and
  // the order should be correct again.
  adapter2.setDropperPolicy(
    std::make_unique<rcomm::LiteCommDropperLossyPolicy>());

  sendPackets(adapter1, reliability);

  REQUIRE(registry->get(rregistry::Int16::MVMT_FORWARD_VELOCITY) == 30);
  REQUIRE(registry->get(rregistry::Int16::MVMT_STEER_DIRECTION) == 70);
}
