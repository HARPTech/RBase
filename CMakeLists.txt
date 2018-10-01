cmake_minimum_required(VERSION 3.0.2)

project(RBase
		VERSION 0.8.0)

set(BUILD_NUMBER "0" CACHE STRING "Build-Number")

set(VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}+${BUILD_NUMBER})

option(testing "Enable building tests" ON)
option(BUILD_SWIG "Enable building SWIG modules." FALSE)
option(INSTALL_RBASE "Enable installing and building of RBase, including SWIG modules." FALSE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(AdditionalCatchParameters -o report.xml -r junit)

get_directory_property(hasParent PARENT_DIRECTORY)

set(Boost_USE_STATIC_LIBS ON)

if(hasParent)
else()
		set(INSTALL_RBASE TRUE)
		set(BUILD_SWIG TRUE)
endif()


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC compiler detected.")
    set (CMAKE_CXX_FLAGS "-std=gnu++14 ${CMAKE_CXX_FLAGS}")
endif ()

set(Boost_USE_STATIC ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

if(NOT Boost_FOUND OR NOT Boost_log_FOUND OR NOT Boost_serialization_FOUND)
		find_package(Boost COMPONENTS log_setup log serialization REQUIRED)
endif()

add_subdirectory(RCore)

add_subdirectory(RUtil)
add_subdirectory(RComm)
add_subdirectory(RRegistry)
add_subdirectory(RSupport)

include(basename_setter)

define_file_basename_for_sources(rutil)
define_file_basename_for_sources(rutils)
define_file_basename_for_sources(rcomm)
define_file_basename_for_sources(rregistry)
define_file_basename_for_sources(rregistrys)
define_file_basename_for_sources(rsupport)
define_file_basename_for_sources(rsupports)

if(BUILD_SWIG)
		add_subdirectory(swig)
endif()

get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    if(BUILD_TESTS)
				add_subdirectory(RCore/test)
    endif()
endif()

if(INSTALL_RBASE)
		# Installation logic, only executed if there is no parent!
		set(CPACK_GENERATOR "DEB")

		set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
		set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Support library for writing regulation kernels and other software around RMaster")
		set(CPACK_PACKAGE_NAME "RBase")
		set(CPACK_PACKAGE_VENDOR "Harptech")
		set(CPACK_PACKAGE_VERSION ${VERSION})
		set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
		set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
		set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
		set(CPACK_RESSOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md)
		set(CPACK_RESSOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
		set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/packages)
		set(CPACK_PACKAGE_CONTACT "Max Heisinger <mail@maximaximal.com>")
		set(CPACK_DEBIAN_PACKAGE_DEPENDS
				"python3, python3-numpy,
        libpython3.4 | libpython3.5 | libpython3.6,
        libboost-serialization1.55.0 | libboost-serialization1.58.0 | libboost-serialization1.62.0")
		set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")

		include(CPack)
else()
		set(RRegistry_INCLUDE_DIRS
				${RRegistry_INCLUDE_DIRS}
				PARENT_SCOPE)
endif()
