cmake_minimum_required(VERSION 3.0.4)
cmake_policy(SET CMP0048 NEW)
project(RComm
		VERSION 0.1.0)

set(CAPNPC_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

find_package(CapnProto REQUIRED)

file(GLOB CAPNP_FILES "definitions/*.capnp")

capnp_generate_cpp(SRCS HDRS ${CAPNP_FILES})

add_library(rcomm SHARED ${SRCS} ${HDRS})

target_link_libraries(rcomm PUBLIC ${CAPNP_LIBRARIES})
target_compile_definitions(rcomm PUBLIC ${CAPNP_DEFINITIONS})
target_include_directories(rcomm PUBLIC SYSTEM ${CAPNP_INCLUDE_DIRS})
target_include_directories(rcomm PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/definitions)
target_include_directories(rcomm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
