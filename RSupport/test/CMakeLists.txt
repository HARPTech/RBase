file(GLOB SRCS "*.cpp")

add_executable(rsupport_tests ${SRCS})

target_link_libraries(rsupport_tests Catch)
target_link_libraries(rsupport_tests rregistry)
target_link_libraries(rsupport_tests rsupport)

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(rsupport_tests)
