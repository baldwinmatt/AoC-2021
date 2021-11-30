# Get list of sources.
file(GLOB_RECURSE SOURCES "*.cpp")

get_filename_component(binary_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "${binary_name}")

# Add the executable.
add_executable(main ${SOURCES})
set_target_properties(main PROPERTIES OUTPUT_NAME "${binary_name}")

# Install application.
install(TARGETS main DESTINATION "bin")
