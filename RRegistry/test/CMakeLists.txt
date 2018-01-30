file(GLOB SRCS "*.cpp")

add_executable(rregistry_tests ${SRCS})

target_link_libraries(rregistry_tests Catch)
target_link_libraries(rregistry_tests rregistry)

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(rregistry_tests)
