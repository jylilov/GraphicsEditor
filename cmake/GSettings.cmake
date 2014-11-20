# Used for GSettings.
#
# schemas_dir: Directory where the *.gschema.xml are located.
# output_dir: Directory where the compiled file is written.

function(add_gschema schemas_dir output_dir)

find_program(GLIB_COMPILE_SCHEMAS NAMES glib-compile-schemas)

if (NOT GLIB_COMPILE_SCHEMAS)
message(FATAL "Could not find glib-compile-schemas")
endif()

file(MAKE_DIRECTORY ${output_dir})

# Command to compile the schemas
execute_process(
    COMMAND ${GLIB_COMPILE_SCHEMAS} --targetdir=${output_dir} ${schemas_dir}
)

endfunction()

