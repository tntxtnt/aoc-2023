#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
namespace views = ranges::views;
#include <chrono>
namespace cron = std::chrono;
using namespace std::chrono_literals;

static constexpr std::string_view kInputFilename = "day11.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

std::pair<std::vector<size_t>, std::vector<size_t>> getEmptyRowColIds(const Input& input) {
    return {
        views::iota(size_t{0}, input.size())                                                 //
            | views::filter([&](auto i) { return input[i].find('#') == std::string::npos; }) //
            | ranges::to<std::vector>(),
        views::iota(size_t{0}, input[0].size())                                                                      //
            | views::filter([&](auto j) { return ranges::all_of(input, [=](auto& row) { return row[j] == '.'; }); }) //
            | ranges::to<std::vector>()};
}

Input getExpandedInput(const Input& input) {
    auto [emptyRowIds, emptyColIds] = getEmptyRowColIds(input);
    Input res;
    res.reserve(input.size() + emptyRowIds.size());
    size_t ir = 0;
    for (size_t i : views::iota(0, std::ssize(input))) {
        std::string row;
        row.reserve(input[i].size() + emptyColIds.size());
        size_t ic = 0;
        for (size_t j : views::iota(0, std::ssize(input[i]))) {
            row += input[i][j];
            if (ic < emptyColIds.size() && j == emptyColIds[ic]) {
                row += '.';
                ++ic;
            }
        }
        res.emplace_back(std::move(row));
        if (ir < emptyRowIds.size() && i == emptyRowIds[ir]) {
            res.emplace_back(input[i].size() + emptyColIds.size(), '.');
            ++ir;
        }
    }
    return res;
}

size_t getDist(size_t a, size_t b) {
    return a < b ? b - a : a - b;
};

auto getGalaxyPositions(const Input& input) {
    return views::cartesian_product(views::iota(size_t{0}, input.size()), views::iota(size_t{0}, input[0].size())) //
           | views::filter([&](const auto& ij) { return input[std::get<0>(ij)][std::get<1>(ij)] == '#'; })         //
           | ranges::to<std::vector>();
}

int part1(const Input& input) {
    const auto galaxyPos = getGalaxyPositions(getExpandedInput(input));
    int res{};
    for (size_t i : views::iota(0U, galaxyPos.size()))
        for (size_t j : views::iota(i + 1, galaxyPos.size())) {
            const auto& [r1, c1] = galaxyPos[i];
            const auto& [r2, c2] = galaxyPos[j];
            const auto dist = getDist(r1, r2) + getDist(c1, c2);
            res += static_cast<int>(dist);
        }
    return res;
}

size_t part2(const Input& input, int multiplier) {
    const auto galaxyPos = getGalaxyPositions(input);
    const auto [emptyRowIds, emptyColIds] = getEmptyRowColIds(input);

    auto emptyCount = [](const auto& ids, size_t v1, size_t v2) {
        if (v1 > v2) std::swap(v1, v2);
        const auto i1 = ranges::upper_bound(ids, v1);
        const auto i2 = ranges::upper_bound(ids, v2);
        return i2 - i1;
    };
    size_t res{};
    for (size_t i : views::iota(0U, galaxyPos.size())) {
        for (size_t j : views::iota(i + 1, galaxyPos.size())) {
            const auto& [r1, c1] = galaxyPos[i];
            const auto& [r2, c2] = galaxyPos[j];
            const auto dist = getDist(r1, r2) + getDist(c1, c2);
            const auto emptyRCcount = emptyCount(emptyRowIds, r1, r2) + emptyCount(emptyColIds, c1, c2);
            res += dist + emptyRCcount * (multiplier - 1);
        }
    }
    return res;
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 374;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr size_t part2CorrectAnswer1 = 374;
    const size_t part2Answer1 = part2(input1, 2);
    const bool part2Correct1 = part2Answer1 == part2CorrectAnswer1;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer1,
               fmt::styled(part2Answer1, fmt::fg(part2Correct1 ? fmt::color::green : fmt::color::red)));
    constexpr size_t part2CorrectAnswer2 = 1030;
    const size_t part2Answer2 = part2(input1, 10);
    const bool part2Correct2 = part2Answer2 == part2CorrectAnswer2;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer2,
               fmt::styled(part2Answer2, fmt::fg(part2Correct2 ? fmt::color::green : fmt::color::red)));
    constexpr size_t part2CorrectAnswer3 = 8410;
    const size_t part2Answer3 = part2(input1, 100);
    const bool part2Correct3 = part2Answer3 == part2CorrectAnswer3;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer3,
               fmt::styled(part2Answer3, fmt::fg(part2Correct3 ? fmt::color::green : fmt::color::red)));

    const bool part2Correct = part2Correct1 && part2Correct2 && part2Correct3;
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
    const auto part2Ans = part2(input, 1'000'000);
    elapsed = cron::steady_clock::now() - startTime;
    fmt::print("Part 2: {} in {}\n", fmt::styled(part2Ans, fmt::fg(fmt::color::yellow)),
               fmt::styled(fmt::format("{:.06f}s", elapsed.count()), fmt::fg(getTimeColor(elapsed))));
}
