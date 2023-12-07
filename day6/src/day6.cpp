#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>

static constexpr std::string_view kInputFilename = "day6.txt";

using Input = std::vector<std::pair<int64_t, int64_t>>;

Input parseInput(std::istream& in) {
    Input res;
    std::string line;
    std::string ignore;
    std::getline(in, line);
    {
        std::istringstream iss{line};
        iss >> ignore;
        for (int64_t t; iss >> t;) res.emplace_back(t, 0);
    }
    std::getline(in, line);
    {
        std::istringstream iss{line};
        iss >> ignore;
        for (auto& [_, d] : res) iss >> d;
    }
    return res;
}

int64_t part1(const Input& input) {
    return std::ranges::fold_left(input | std::views::transform([](const auto& p) {
                                      auto [t, d] = p;
                                      return std::ranges::count_if(std::views::iota(1, t),
                                                                   [=](int i) { return i * (t - i) > d; });
                                  }),
                                  1LL, std::multiplies{});
}

int64_t part2(const Input& input) {
    auto t = std::ranges::fold_left(input, std::string{}, [](auto s, auto& p) { return s + std::to_string(p.first); });
    auto d = std::ranges::fold_left(input, std::string{}, [](auto s, auto& p) { return s + std::to_string(p.second); });
    Input newInput{std::make_pair(std::stoll(t), std::stoll(d))};
    return part1(newInput);
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
Time:      7  15   30
Distance:  9  40  200
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int64_t part1CorrectAnswer = 288;
    const auto part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int64_t part2CorrectAnswer = 71503;
    const auto part2Answer = part2(input1);
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
    fmt::print("Part 1: {}\n", fmt::styled(part1(input), fmt::fg(fmt::color::yellow)));
    if (!test2) return 2;
    fmt::print("Part 2: {}\n", fmt::styled(part2(input), fmt::fg(fmt::color::yellow)));
}
