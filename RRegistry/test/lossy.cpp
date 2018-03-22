#include <catch.hpp>

#include <RComm/LiteCommDropper.hpp>
#include <RRegistry/Registry.hpp>
#include <RRegistry/TypeConverter.hpp>

#include <iostream>
#include <stack>
using std::cout;
using std::endl;

using namespace lrt;
using namespace lrt::rregistry;

const int checkValue = 100;

class StackAdapter : public rregistry::Registry::Adapter
{
  public:
  StackAdapter(std::shared_ptr<Registry> registry, bool subscribed = false)
    : LiteCommAdapter::LiteCommAdapter(registry, subscribed)
  {
  }
  ~StackAdapter() {}

  virtual void send(const Message& msg,
                    rcomm::Reliability = rcomm::DefaultReliability)
  {
    //cout << msg.print().str() << endl;

    m_stack.push(msg);
  }

  void sendStack()
  {
    while(!m_stack.empty()) {
      if(m_target)
        m_target->parseMessage(m_stack.top());
      m_stack.pop();
    }
  }

  void setTarget(StackAdapter* target) { m_target = target; }

  private:
  StackAdapter* m_target = nullptr;
  std::stack<Message> m_stack;
};

void sendPackets(StackAdapter &adapter, rcomm::Reliability reliability) {
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

  // With the dropper policy in place, unordered packets should be dropped and the
  // order should be correct again.
  adapter2.setDropperPolicy(std::make_unique<rcomm::LiteCommDropperLossyPolicy>());

  sendPackets(adapter1, reliability);

  REQUIRE(registry->get(rregistry::Int16::MVMT_FORWARD_VELOCITY) == 30);
  REQUIRE(registry->get(rregistry::Int16::MVMT_STEER_DIRECTION) == 70);
}
