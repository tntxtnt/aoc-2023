#include <iostream>
#include <fmt/format.h>
#include <fmt/color.h>

auto main() -> int {
    fmt::print("{} {}!\n", fmt::styled("Hello", fmt::fg(fmt::color::green)),
               fmt::styled("world", fmt::fg(fmt::color::cyan)));
}
