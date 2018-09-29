# This macro has been taken from https://stackoverflow.com/a/27990434
function(define_file_basename_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
	# Get source file's current list of compile definitions.
	get_property(defs SOURCE "${sourcefile}"
	    PROPERTY COMPILE_DEFINITIONS)
	# Add the FILE_BASENAME=filename compile definition to the list.
	get_filename_component(basename "${sourcefile}" NAME)
	list(APPEND defs "FILE_BASENAME=\"${basename}\"")
	# Set the updated compile definitions on the source file.
	set_property(
	    SOURCE "${sourcefile}"
	    PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach()
endfunction()
