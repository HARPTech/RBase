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
  adapter1->addCallback([&](std::string line) {
    INFO("Transferring from Adapter1 to Adapter2: "
         << adapter2->binaryStreamToBitRepresentation(
              adapter2->extractBinaryStreamFromLine(line)));
    REQUIRE(adapter2->parseLine(line) == LRT_RCORE_OK);
  });
  adapter2->setMode(ConsoleAdapter::CALLBACK);
  adapter2->addCallback([&](std::string line) {
    INFO("Transferring from Adapter2 to Adapter1: "
         << adapter2->binaryStreamToBitRepresentation(
              adapter2->extractBinaryStreamFromLine(line)));
    REQUIRE(adapter1->parseLine(line) == LRT_RCORE_OK);
  });

  const int16_t compare = 10;

  REQUIRE(adapter2->subscribe(Int16::MVMT_FORWARD_VELOCITY) == LRT_RCORE_OK);
  REQUIRE(adapter2->subscribe(Int16::MVMT_MOTOR_PWM_FL) == LRT_RCORE_OK);

  REQUIRE(adapter1->isSubscribed(Int16::MVMT_FORWARD_VELOCITY));
  REQUIRE(adapter1->isSubscribed(Int16::MVMT_MOTOR_PWM_FL));

  REQUIRE(adapter1->set(Int16::MVMT_FORWARD_VELOCITY, compare) == LRT_RCORE_OK);
  REQUIRE(adapter1->set(Int16::MVMT_MOTOR_PWM_FL, compare) == LRT_RCORE_OK);

  REQUIRE(registry2->get(Int16::MVMT_FORWARD_VELOCITY) == compare);
  REQUIRE(registry2->get(Int16::MVMT_MOTOR_PWM_FL) == compare);
  REQUIRE(registry2->get(Int16::MVMT_MOTOR_PWM_FR) == 0);

  registry2->set(Int16::MVMT_MOTOR_PWM_FL, 200);
  REQUIRE(registry1->get(Int16::MVMT_MOTOR_PWM_FL) == 0);

  REQUIRE(adapter1->subscribe(Int16::MVMT_MOTOR_PWM_FL) == LRT_RCORE_OK);

  REQUIRE(adapter2->set(Int16::MVMT_MOTOR_PWM_FL, 200) == LRT_RCORE_OK);
  REQUIRE(registry1->get(Int16::MVMT_MOTOR_PWM_FL) == 200);

  registry2->set(Int16::MVMT_MOTOR_PWM_FL, 100);
  REQUIRE(registry1->get(Int16::MVMT_MOTOR_PWM_FL) == 100);
}
