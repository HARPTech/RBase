# Imports python into CMake and the swig system.

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_DIRS})

set_property(SOURCE rregistry.i PROPERTY CPLUSPLUS ON)
SWIG_ADD_LIBRARY(RRegistry LANGUAGE python SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/rregistry.i)
SWIG_LINK_LIBRARIES(RRegistry rregistry ${PYTHON_LIBRARIES})
set_property(SOURCE rregistry.i PROPERTY SWIG_MODULE_NAME RRegistry)

set_property(SOURCE rsupport.i PROPERTY CPLUSPLUS ON)
SWIG_ADD_LIBRARY(RSupport LANGUAGE python SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/rsupport.i)
SWIG_LINK_LIBRARIES(RSupport rsupport ${PYTHON_LIBRARIES})
set_property(SOURCE rsupport.i PROPERTY SWIG_MODULE_NAME RSupport)

install(FILES
		${CMAKE_CURRENT_BINARY_DIR}/RRegistry.py
		${CMAKE_CURRENT_BINARY_DIR}/_RRegistry.so
		${CMAKE_CURRENT_BINARY_DIR}/RSupport.py
		${CMAKE_CURRENT_BINARY_DIR}/_RSupport.so
		DESTINATION share/python3
		)
