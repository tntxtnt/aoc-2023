add_executable(day22 src/day22.cpp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(day22 PRIVATE fmt::fmt)

# Enable sanitizer for Debug config
# If you're getting error LNK2038: mismatch detected for 'annotate_vector': value '0' doesn't match value '1'
# either disable sanitizer or rebuild other libraries with sanitizer on. For more information see
# https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
target_sanitize_options(day22 OPTIONS address undefined leak CONFIGS Debug RelWithDebInfo)

target_fixit(day22)

# Use ${CMAKE_CURRENT_BINARY_DIR} for output dir
configure_file("day22.txt" "${CMAKE_CURRENT_BINARY_DIR}/day22.txt" COPYONLY) # re-copy if source changes
# file(COPY "day22.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # copy only once
