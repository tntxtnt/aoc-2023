cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(2023 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Goodies
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers.cmake)
# Static analysis
# find_static_analysis(CLANGTIDY CPPCHECK CPPLINT)

# Executables
add_subdirectory("day1")
add_subdirectory("day2")
add_subdirectory("day3")
add_subdirectory("day4")

# Tests
enable_testing()
add_subdirectory(test/compiler)
