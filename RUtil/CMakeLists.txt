project(RUtil
		VERSION 0.1.0)

add_subdirectory(src)

add_library(rutil SHARED ${SRCS})
add_library(rutils STATIC ${SRCS})

set_target_properties(rutil PROPERTIES
    CXX_STANDARD_REQUIRED 14
    CXX_EXTENSIONS OFF)
set_target_properties(rutils PROPERTIES
    CXX_STANDARD_REQUIRED 14
    CXX_EXTENSIONS OFF)

target_link_libraries(rutil Boost::log)
target_link_libraries(rutils Boost::log)

target_include_directories(rutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(rutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(rutil PUBLIC -DBOOST_LOG_DYN_LINK)
target_compile_definitions(rutils PUBLIC -DBOOST_LOG_DYN_LINK)

set_property(TARGET rutil PROPERTY CXX_STANDARD 14)
set_property(TARGET rutils PROPERTY CXX_STANDARD 14)

set_property(TARGET rutil PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET rutils PROPERTY POSITION_INDEPENDENT_CODE ON)

if(INSTALL_RBASE)
		include(GNUInstallDirs)

		install(TARGETS rutil
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)
endif()
