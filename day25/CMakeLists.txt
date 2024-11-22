add_executable(day25 src/day25.cpp)

find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
target_link_libraries(day25 PRIVATE fmt::fmt range-v3::range-v3)

# Enable sanitizer for Debug config
# If you're getting error LNK2038: mismatch detected for 'annotate_vector': value '0' doesn't match value '1'
# either disable sanitizer or rebuild other libraries with sanitizer on. For more information see
# https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
target_sanitize_options(day25 OPTIONS address undefined leak CONFIGS Debug RelWithDebInfo)

target_fixit(day25)

# Use ${CMAKE_CURRENT_BINARY_DIR} for output dir
configure_file("day25.txt" "${CMAKE_CURRENT_BINARY_DIR}/day25.txt" COPYONLY) # re-copy if source changes
# file(COPY "day25.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) # copy only once
