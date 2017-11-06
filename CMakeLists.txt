cmake_minimum_required(VERSION 3.0.2)

project(RRegistry
    VERSION 0.1.0)

set(RRegistry_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PARENT_SCOPE)

set(RRegistry_TEST_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/test
    PARENT_SCOPE)
