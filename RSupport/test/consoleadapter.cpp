#include <catch.hpp>

#include <RRegistry/Registry.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <RSupport/ConsoleAdapter.hpp>

TEST_CASE("rsupport::ConsoleAdapter transmissions working.",
          "rcomm, rregistry, rsupport")
{
  using namespace lrt;
  using namespace rregistry;
  using namespace rsupport;

  std::shared_ptr<Registry> registry1 = std::make_shared<Registry>();
  std::shared_ptr<Registry> registry2 = std::make_shared<Registry>();

  std::shared_ptr<ConsoleAdapter> adapter1 =
    std::make_shared<ConsoleAdapter>(registry1, false);
  std::shared_ptr<ConsoleAdapter> adapter2 =
    std::make_shared<ConsoleAdapter>(registry2, false);

  registry1->registerAdapter(adapter1);
  registry2->registerAdapter(adapter2);

  // Pipe data between adapters.
  adapter1->setMode(ConsoleAdapter::CALLBACK);
  adapter1->addCallback([&](std::string line) { adapter2->parseLine(line); });
  adapter2->setMode(ConsoleAdapter::CALLBACK);
  adapter2->addCallback([&](std::string line) { adapter1->parseLine(line); });

  const int16_t compare = 10;

  adapter2->subscribe(Int16::MVMT_FORWARD_VELOCITY);
  adapter2->subscribe(Int16::MVMT_MOTOR_PWM_FL);

  adapter1->set(Int16::MVMT_FORWARD_VELOCITY, compare);
  adapter1->set(Int16::MVMT_MOTOR_PWM_FL, compare);

  REQUIRE(registry2->get(Int16::MVMT_FORWARD_VELOCITY) == compare);
  REQUIRE(registry2->get(Int16::MVMT_MOTOR_PWM_FL) == compare);
  REQUIRE(registry2->get(Int16::MVMT_MOTOR_PWM_FR) == 0);

  registry2->set(Int16::MVMT_MOTOR_PWM_FL, 200);
  REQUIRE(registry1->get(Int16::MVMT_MOTOR_PWM_FL) == 0);

  adapter1->subscribe(Int16::MVMT_MOTOR_PWM_FL);
  registry2->set(Int16::MVMT_MOTOR_PWM_FL, 200);
  REQUIRE(registry1->get(Int16::MVMT_MOTOR_PWM_FL) == 200);
}
