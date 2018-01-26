cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0048 NEW)
project(RRegistry
    VERSION 0.1.0)

set(RRegistry_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PARENT_SCOPE)

set(RRegistry_TEST_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/test
    PARENT_SCOPE)

add_library(rregistry INTERFACE)
target_compile_definitions(rregistry
    INTERFACE LIBRARY_HEADER_ONLY
)

# Needed because of cross references to the communication protocols with the adapter class.
target_link_libraries(rregistry INTERFACE rcomm)

target_include_directories(rregistry INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
