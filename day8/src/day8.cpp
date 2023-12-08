#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <stdexcept>
namespace ranges = std::ranges;
namespace views = std::views;
using namespace std::string_literals;

static constexpr std::string_view kInputFilename = "day8.txt";

struct Input {
    std::string instrs;
    std::unordered_map<std::string, std::pair<std::string, std::string>> adj;
};

Input parseInput(std::istream& in) {
    Input res;
    in >> res.instrs;
    for (std::string from, ignore, to1, to2; in >> from >> ignore >> to1 >> to2;)
        res.adj[from] = std::make_pair(to1.substr(1, 3), to2.substr(0, 3));
    return res;
}

int part1(const Input& input, std::string s = "AAA") {
    return 1 + *(views::iota(0) | views::drop_while([&](int i) {
                     return "ZZZ" != (s = input.instrs[i % input.instrs.size()] == 'L'
                                              ? input.adj.find(s)->second.first
                                              : input.adj.find(s)->second.second);
                 })).begin();
}

int64_t part2(const Input& input) {
    return ranges::fold_left(input.adj                                                    //
                                 | views::keys                                            //
                                 | views::filter([](auto& s) { return s.back() == 'A'; }) //
                                 | views::transform([&](std::string s) {
                                       return 1 + *(views::iota(0) | views::drop_while([&](int i) {
                                                        return (s = input.instrs[i % input.instrs.size()] == 'L'
                                                                        ? input.adj.find(s)->second.first
                                                                        : input.adj.find(s)->second.second)
                                                                   .back() != 'Z';
                                                    })).begin();
                                   }),
                             1LL, [](auto res, int e) { return std::lcm(res, e); });
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ)
)"};
    std::istringstream iss2{R"(
LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ)
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);
    iss2.ignore();
    const auto input2 = parseInput(iss2);

    constexpr int part1CorrectAnswer1 = 2;
    const int part1Answer1 = part1(input1);
    const bool part1Correct1 = part1Answer1 == part1CorrectAnswer1;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer1,
               fmt::styled(part1Answer1, fmt::fg(part1Correct1 ? fmt::color::green : fmt::color::red)));
    constexpr int part1CorrectAnswer2 = 6;
    const int part1Answer2 = part1(input2);
    const bool part1Correct2 = part1Answer2 == part1CorrectAnswer2;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer2,
               fmt::styled(part1Answer2, fmt::fg(part1Correct2 ? fmt::color::green : fmt::color::red)));
    const bool part1Correct = part1Correct1 && part1Correct2;

    std::istringstream iss3{R"(
LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX)
)"};
    iss3.ignore();
    const auto input3 = parseInput(iss3);
    constexpr int64_t part2CorrectAnswer = 6;
    const int64_t part2Answer = part2(input3);
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
