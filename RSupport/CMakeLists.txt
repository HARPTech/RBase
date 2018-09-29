project(RSupport
		VERSION 0.1.0)

add_subdirectory(src)
add_subdirectory(test)

add_library(rsupport SHARED ${SRCS})
add_library(rsupports STATIC ${SRCS})

set_target_properties(rsupport PROPERTIES
    CXX_STANDARD_REQUIRED 14
    CXX_EXTENSIONS OFF)
set_target_properties(rsupports PROPERTIES
    CXX_STANDARD_REQUIRED 14
    CXX_EXTENSIONS OFF)

target_link_libraries(rsupport rcomm)
target_link_libraries(rsupports rcomm)
target_link_libraries(rsupport rregistry)
target_link_libraries(rsupports rregistrys)
target_link_libraries(rsupport rutil)
target_link_libraries(rsupports rutils)

if(BOOST_LIBRARYDIR)
		target_link_libraries(rsupport libboost_serialization.so)
		target_link_libraries(rsupports libboost_serialization.a)
    target_include_directories(rsupport PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})
    target_include_directories(rsupports PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})
else()
		target_link_libraries(rsupport Boost::serialization)
		target_link_libraries(rsupports Boost::serialization)
endif()

target_include_directories(rsupport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(rsupports PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_property(TARGET rsupport PROPERTY CXX_STANDARD 14)
set_property(TARGET rsupports PROPERTY CXX_STANDARD 14)

set_property(TARGET rsupport PROPERTY POSITION_INDEPENDENT_CODE ON)

if(INSTALL_RBASE)
		include(GNUInstallDirs)

		install(TARGETS rsupport
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)

		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/example/regulation-kernel.py
				DESTINATION share/rbase/examples/)
endif()
