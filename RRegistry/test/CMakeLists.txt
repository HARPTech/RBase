file(GLOB TEST_SRCS "*.cpp")

add_executable(rregistry_test_adapter
		${CMAKE_CURRENT_SOURCE_DIR}/adapter.cpp)

target_link_libraries(rregistry_test_adapter Catch)
target_link_libraries(rregistry_test_adapter rregistry)

add_test(
		NAME RRegistry_Adapter
		COMMAND rregistry_test_adapter)
