file(GLOB TEST_SRCS "*.cpp")

add_executable(rsupport_test_pipeadapter
		${CMAKE_CURRENT_SOURCE_DIR}/pipeadapter.cpp)

target_link_libraries(rsupport_test_pipeadapter Catch)
target_link_libraries(rsupport_test_pipeadapter rregistry)
target_link_libraries(rsupport_test_pipeadapter rsupport)

add_test(
		NAME RSupport_PipeAdapter
		COMMAND rsupport_test_pipeadapter)
