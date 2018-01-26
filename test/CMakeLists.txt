add_library(Catch INTERFACE)
target_compile_definitions(Catch
    INTERFACE LIBRARY_HEADER_ONLY
)
target_include_directories(Catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)
