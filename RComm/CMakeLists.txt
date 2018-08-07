cmake_minimum_required(VERSION 3.0.4)
cmake_policy(SET CMP0048 NEW)
project(RComm
		VERSION 0.1.0)

add_library(rcomm STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/RComm.cpp)

target_link_libraries(rcomm rcore)

install(TARGETS rcomm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)

target_include_directories(rcomm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
