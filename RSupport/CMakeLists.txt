project(RSupport
		VERSION 0.1.0)

add_subdirectory(src)
add_subdirectory(test)

add_library(rsupport SHARED ${SRCS})

target_link_libraries(rsupport rcomm)
target_link_libraries(rsupport rregistry)

target_include_directories(rsupport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_property(TARGET rsupport PROPERTY CXX_STANDARD 14)

set_property(TARGET rsupport PROPERTY POSITION_INDEPENDENT_CODE ON)

install(TARGETS rsupport
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)
