project(RSupport
		VERSION 0.1.0)

add_subdirectory(src)

add_library(rsupport SHARED ${SRCS})

target_link_libraries(rsupport rcomm)
target_link_libraries(rsupport rregistry)
