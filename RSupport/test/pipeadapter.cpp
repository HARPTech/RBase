#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <RRegistry/Registry.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <RSupport/PipeAdapter.hpp>

TEST_CASE("rsupport::PipeAdapter transmissions working.",
          "rcomm, rregistry, rsupport")
{
  using namespace lrt;
  using namespace rregistry;
  using namespace rsupport;

  std::shared_ptr<Registry> registry1 = std::make_shared<Registry>();
  std::shared_ptr<Registry> registry2 = std::make_shared<Registry>();

  std::shared_ptr<PipeAdapter> adapter1 =
    std::make_shared<PipeAdapter>(registry1);
  std::shared_ptr<PipeAdapter> adapter2 =
    std::make_shared<PipeAdapter>(registry2);

  std::string queues = "./fifo-queue";

  RSupportStatus status;

  status = adapter1->create(queues.c_str());
  REQUIRE(status == RSupportStatus_Ok);
  status = adapter2->connect(queues.c_str());
  REQUIRE(status == RSupportStatus_Ok);

  status = adapter1->service();
  REQUIRE(status == RSupportStatus_Updates);
  status = adapter2->service();
  REQUIRE(status == RSupportStatus_Ok);

  REQUIRE(adapter1->inFd() > 0);
  REQUIRE(adapter2->inFd() > 0);
  REQUIRE(adapter2->outFd() > 0);
  REQUIRE(adapter1->outFd() > 0);

  const int16_t compare = 10;

  adapter2->subscribe(Int16::MVMT_FORWARD_VELOCITY);
  status = adapter1->service();
  REQUIRE(status == RSupportStatus_Updates);

  adapter1->set(Int16::MVMT_FORWARD_VELOCITY, compare);

  status = adapter2->service();
  REQUIRE(status == RSupportStatus_Updates);

  REQUIRE(registry2->get(Int16::MVMT_FORWARD_VELOCITY) == compare);
}
