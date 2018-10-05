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
add_library(rregistrys STATIC ${RRegistry_SRCS})

target_compile_definitions(rregistry
    INTERFACE LIBRARY_HEADER_ONLY
)
target_compile_definitions(rregistrys
    INTERFACE LIBRARY_HEADER_ONLY
)


if(NOT Threads_FOUND)
    find_package (Threads)
endif()

# Needed because of cross references to the communication protocols with the adapter class.
target_link_libraries(rregistry rcomm Threads::Threads)
target_link_libraries(rregistrys rcomm Threads::Threads)
target_link_libraries(rregistry rutil)
target_link_libraries(rregistrys rutils)

target_include_directories(rregistry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(rregistrys PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(SQLite3 REQUIRED)
if(${SQLITE3_FOUND})
    target_include_directories(rregistry PRIVATE SYSTEM ${SQLITE3_INCLUDE_DIRS})
    target_include_directories(rregistrys PRIVATE SYSTEM ${SQLITE3_INCLUDE_DIRS})
    target_link_libraries(rregistry ${SQLITE3_LIBRARIES})
    target_link_libraries(rregistrys ${SQLITE3_LIBRARIES})
endif()

if(BUILD_TESTS)
		add_subdirectory(test)
endif()

if(INSTALL_RBASE)
		include(GNUInstallDirs)

		install(TARGETS rregistry
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)
endif()
