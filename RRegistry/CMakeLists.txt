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

set(RRegistry_SRCS
		${CMAKE_CURRENT_SOURCE_DIR}/src/DBPersistencyPolicy.cpp)

add_library(rregistry SHARED ${RRegistry_SRCS})
target_compile_definitions(rregistry
    INTERFACE LIBRARY_HEADER_ONLY
)

# Needed because of cross references to the communication protocols with the adapter class.
target_link_libraries(rregistry rcomm)

target_include_directories(rregistry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(SQLite3 REQUIRED)
if(${SQLITE3_FOUND})
    target_include_directories(rregistry PRIVATE SYSTEM ${SQLITE3_INCLUDE_DIRS})
    target_link_libraries(rregistry ${SQLITE3_LIBRARIES})
endif()

add_subdirectory(test)

if(INSTALL_RBASE)
		include(GNUInstallDirs)

		install(TARGETS rregistry
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)
endif()
