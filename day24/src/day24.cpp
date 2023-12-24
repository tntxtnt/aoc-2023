#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <chrono>
namespace cron = std::chrono;
using namespace std::chrono_literals;
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day24.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) {
        res.push_back(line);
    }
    return res;
}

int part1(const Input& input) {
    return static_cast<int>(input.size());
}

int part2(const Input& input) {
    return static_cast<int>(input.size());
}

std::pair<bool, bool> test() {
    auto testPart1 = [](std::istream& is, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part1(input);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 1: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::pair<std::string_view, int> part1Cases[] = {{R"(
line 1
line 2
line 3
)",
                                                                0}};
    bool part1Correct = true;
    for (auto [sv, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, correctAnswer);
    }

    auto testPart2 = [](std::istream& is, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part2(input);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 2: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::pair<std::string_view, int> part2Cases[] = {{R"(
line 1
line 2
line 3
)",
                                                                0},
                                                               {R"(
line 1
line 2
)",
                                                                0}};
    bool part2Correct = true;
    for (auto [sv, correctAnswer] : part2Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part2Correct &= testPart2(iss, correctAnswer);
    }
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
