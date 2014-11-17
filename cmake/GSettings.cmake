# Used for GResource.
#
# resource_dir: Directory where the .gresource.xml is located.
# resource_file: Filename of the .gresource.xml file (just the
# filename, not the complete path).
# output_dir: Directory where the C output file is written.
# output_file: This variable will be set with the complete path of the
# output C file.

function(add_gschema schemas_dir output_dir)

find_program(GLIB_COMPILE_SCHEMAS NAMES glib-compile-schemas)

if (NOT GLIB_COMPILE_SCHEMAS)
message(FATAL "Could not find glib-compile-schemas")
endif()

# Get the output file path
set(output_compile_file "${output_dir}/gschemas.compiled")

# Command to compile the resources
execute_process(COMMAND ${GLIB_COMPILE_SCHEMAS} --targetdir=${output_dir} ${schemas_dir})

endfunction()

