cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(2023 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

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
add_subdirectory("day5")
add_subdirectory("day6")
add_subdirectory("day7")
add_subdirectory("day8")
add_subdirectory("day9")
add_subdirectory("day10")
add_subdirectory("day11")
add_subdirectory("day12")
add_subdirectory("day13")
add_subdirectory("day14")
add_subdirectory("day15")
add_subdirectory("day16")
add_subdirectory("day17")
add_subdirectory("day18")
add_subdirectory("day19")
add_subdirectory("day20")
add_subdirectory("day21")
add_subdirectory("day22")
add_subdirectory("day23")
add_subdirectory("day24")
add_subdirectory("day25")

# Tests
enable_testing()
add_subdirectory(test/compiler)
