add_executable(day17 src/day17.cpp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(day17 PRIVATE fmt::fmt)

# Enable sanitizer for Debug config
# If you're getting error LNK2038: mismatch detected for 'annotate_vector': value '0' doesn't match value '1'
# either disable sanitizer or rebuild other libraries with sanitizer on. For more information see
# https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
target_sanitize_options(day17 OPTIONS address undefined leak CONFIGS Debug RelWithDebInfo)

target_fixit(day17)

# Use ${CMAKE_CURRENT_BINARY_DIR} for output dir
configure_file("day17.txt" "${CMAKE_CURRENT_BINARY_DIR}/day17.txt" COPYONLY) # re-copy if source changes
# file(COPY "day17.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # copy only once
