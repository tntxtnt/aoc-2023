add_executable(day1 src/main.cpp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(day1 PRIVATE fmt::fmt)

# Enable sanitizer for Debug config
# If you're getting error LNK2038: mismatch detected for 'annotate_vector': value '0' doesn't match value '1'
# either disable sanitizer or rebuild other libraries with sanitizer on. For more information see
# https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
target_sanitize_options(day1 OPTIONS address undefined leak CONFIGS Debug RelWithDebInfo)

target_fixit(day1)

# Use ${CMAKE_CURRENT_BINARY_DIR} for output dir
# configure_file(${resource_file} ${destination_file} COPYONLY)           # re-copy if source changes
file(COPY "day1.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # copy only once
