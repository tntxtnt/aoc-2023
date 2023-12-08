add_executable(day8 src/day8.cpp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(day8 PRIVATE fmt::fmt)

# Enable sanitizer for Debug config
# If you're getting error LNK2038: mismatch detected for 'annotate_vector': value '0' doesn't match value '1'
# either disable sanitizer or rebuild other libraries with sanitizer on. For more information see
# https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
target_sanitize_options(day8 OPTIONS address undefined leak CONFIGS Debug RelWithDebInfo)

target_fixit(day8)

# Use ${CMAKE_CURRENT_BINARY_DIR} for output dir
configure_file("day8.txt" "${CMAKE_CURRENT_BINARY_DIR}/day8.txt" COPYONLY) # re-copy if source changes
# file(COPY "day8.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # copy only once
