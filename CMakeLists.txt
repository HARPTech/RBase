cmake_minimum_required(VERSION 3.0.2)

project(RBase
		VERSION 0.2.0)

option(testing "Enable building tests" OFF)

if(${testing})
		enable_testing()
endif()

add_subdirectory(test)

add_subdirectory(RComm)
add_subdirectory(RRegistry)
add_subdirectory(RSupport)

add_subdirectory(swig)

get_directory_property(hasParent PARENT_DIRECTORY)

if(${hasParent})
		set(RRegistry_INCLUDE_DIRS
				${RRegistry_INCLUDE_DIRS}
				PARENT_SCOPE)
endif()
