#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
namespace views = ranges::views;
#include <chrono>
namespace cron = std::chrono;
using namespace std::chrono_literals;

static constexpr std::string_view kInputFilename = "day14.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

int part1(const Input& input) {
    int res{};
    for (auto col : views::iota(0, std::ssize(input[0]))) {
        int base{static_cast<int>(std::ssize(input))};
        for (auto row : views::iota(0, std::ssize(input))) {
            if (input[row][col] == 'O') {
                res += base--;
            } else if (input[row][col] == '#') {
                base = static_cast<int>(std::ssize(input) - row - 1);
            }
        }
    }
    return res;
}

void cycle(const Input& input, std::string& flattenInput) {
    auto v1 = views::iota(0, std::ssize(input));
    auto v2 = views::iota(0, std::ssize(input[0]));
    int w = 0;
    // N
    for (auto [c, r] : views::cartesian_product(v2, v1)) {
        if (r == 0) w = 0;
        char& ch = flattenInput[r * input[0].size() + c];
        if (ch == '#') {
            w = r + 1;
        } else if (ch == 'O') {
            ch = '.';
            flattenInput[w++ * input[0].size() + c] = 'O';
        }
    }
    // W
    for (auto [r, c] : views::cartesian_product(v1, v2)) {
        if (c == 0) w = 0;
        char& ch = flattenInput[r * input[0].size() + c];
        if (ch == '#') {
            w = c + 1;
        } else if (ch == 'O') {
            ch = '.';
            flattenInput[r * input[0].size() + w++] = 'O';
        }
    }
    // S
    for (auto [c, r] : views::cartesian_product(v2, v1 | views::reverse)) {
        if (r == std::ssize(input) - 1) w = static_cast<int>(std::ssize(input) - 1);
        char& ch = flattenInput[r * input[0].size() + c];
        if (ch == '#') {
            w = r - 1;
        } else if (ch == 'O') {
            ch = '.';
            flattenInput[w-- * input[0].size() + c] = 'O';
        }
    }
    // E
    for (auto [r, c] : views::cartesian_product(v1, v2 | views::reverse)) {
        if (c == std::ssize(input[0]) - 1) w = static_cast<int>(std::ssize(input[0]) - 1);
        char& ch = flattenInput[r * input[0].size() + c];
        if (ch == '#') {
            w = c - 1;
        } else if (ch == 'O') {
            ch = '.';
            flattenInput[r * input[0].size() + w--] = 'O';
        }
    }
}

int part2(const Input& input) {
    constexpr int kTargetCycle = 1'000'000'000;
    std::string flattenInput;
    flattenInput.reserve(input.size() * input[0].size());
    for (const auto& line : input) flattenInput += line;

    int repeatIndex = -1;
    int repeatLen = 0;
    std::unordered_map<std::string, int> visited;
    visited[flattenInput] = 0;
    for (int i = 1; i <= kTargetCycle; ++i) {
        cycle(input, flattenInput);
        if (visited.count(flattenInput) == 1) {
            repeatIndex = visited[flattenInput];
            repeatLen = i - repeatIndex;
            break;
        }
        visited[flattenInput] = i;
    }
    const int cycleLeft = (kTargetCycle - repeatIndex) % repeatLen;
    for (int i = cycleLeft; i--;) cycle(input, flattenInput);

    int res{};
    std::string_view sv{flattenInput};
    for (int i : views::iota(0, std::ssize(input))) {
        const auto line = sv.substr(i * input[0].size(), input[0].size());
        res += static_cast<int>(ranges::count(line, 'O') * (input.size() - i));
    }
    return res;
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 136;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int part2CorrectAnswer = 64;
    const int part2Answer = part2(input1);
    const bool part2Correct = part2Answer == part2CorrectAnswer;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer,
               fmt::styled(part2Answer, fmt::fg(part2Correct ? fmt::color::green : fmt::color::red)));

    return {part1Correct, part2Correct};
}

int main() {
    auto [test1, test2] = test();
    if (!test1) return 1;
    auto in = std::ifstream(kInputFilename.data());
    if (!in) {
        fmt::print("Cannot open '{}'\n", kInputFilename);
        return -1;
    }
    const auto input = parseInput(in);

    auto getTimeColor = [](const auto& elapsed) {
        return elapsed < 100ms ? fmt::color::light_green : elapsed < 1s ? fmt::color::orange : fmt::color::orange_red;
    };
    auto startTime = cron::steady_clock::now();
    const auto part1Ans = part1(input);
    cron::duration<double> elapsed = cron::steady_clock::now() - startTime;
    fmt::print("Part 1: {} in {}\n", fmt::styled(part1Ans, fmt::fg(fmt::color::yellow)),
               fmt::styled(fmt::format("{:.06f}s", elapsed.count()), fmt::fg(getTimeColor(elapsed))));

    if (!test2) return 2;
    startTime = cron::steady_clock::now();
    const auto part2Ans = part2(input);
    elapsed = cron::steady_clock::now() - startTime;
    fmt::print("Part 2: {} in {}\n", fmt::styled(part2Ans, fmt::fg(fmt::color::yellow)),
               fmt::styled(fmt::format("{:.06f}s", elapsed.count()), fmt::fg(getTimeColor(elapsed))));
}
