#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <array>
#include <map>
#include <limits>
#include <cstdint>
#include <tuple>
#include <optional>
#include <utility>

static constexpr std::string_view kInputFilename = "day5.txt";

struct ClosedInterval {
    int64_t a;
    int64_t b;
    bool contains(int64_t v) const { return a <= v && v <= b; }
    std::tuple<std::optional<ClosedInterval>, std::optional<ClosedInterval>, std::optional<ClosedInterval>>
    union_difference(const ClosedInterval& o) const {
        if (a < o.a) {     // a [oa ob]
            if (b < o.a) { // a b [oa ob]
                return {{}, {*this}, {}};
            } else if (b > o.b) { // a [oa ob] b
                return {o, ClosedInterval{a, o.a - 1}, ClosedInterval{o.b + 1, b}};
            } else { // a [oa b ob]
                return {ClosedInterval{o.a, b}, ClosedInterval{a, o.a - 1}, {}};
            }
        } else if (a > o.b) { // [oa ob] a
            if (b < o.a) {    // b [oa ob] a // impossible
                std::unreachable();
            } else if (b > o.b) { // [oa ob] a b // obvious
                return {{}, {*this}, {}};
            } else { // [oa b ob] a // impossible
                std::unreachable();
            }
        } else {           // [oa a ob]
            if (b < o.a) { // b [oa a ob] // impossible
                std::unreachable();
            } else if (b > o.b) { // [oa a ob] b
                return {ClosedInterval{a, o.b}, ClosedInterval{o.b + 1, b}, {}};
            } else { // [oa a b ob]
                return {*this, {}, {}};
            }
        }
    }
};

struct CITransf {
    ClosedInterval ci;
    int64_t delta;
};

struct Input {
    std::vector<int64_t> seeds;
    std::array<std::vector<CITransf>, 7> mapping;
};

Input parseInput(std::istream& in) {
    Input res;
    std::string line;
    std::getline(in, line);
    if (std::istringstream iss{line}) {
        iss >> line;
        for (int64_t n; iss >> n;) res.seeds.push_back(n);
    }
    std::getline(in, line);
    for (auto& m : res.mapping) {
        std::getline(in, line);
        for (; std::getline(in, line) && !line.empty();) {
            int64_t to, from, len;
            std::istringstream iss{line};
            iss >> to >> from >> len;
            ClosedInterval ci{from, from + len - 1};
            m.emplace_back(ci, to - from);
        }
    }
    return res;
}

int64_t part1(const Input& input) {
    return std::ranges::min(
        input.seeds | std::views::transform([&](int64_t seed) {
            return std::ranges::fold_left(input.mapping, seed, [](auto seed, const auto& m) {
                const auto it = std::ranges::find_if(m, [&](const CITransf& t) { return t.ci.contains(seed); });
                return it != end(m) ? seed + it->delta : seed;
            });
        }));
}

int64_t part2(const Input& input) {
    std::vector<ClosedInterval> seeds;
    for (size_t i = 0; i < input.seeds.size(); i += 2)
        seeds.emplace_back(input.seeds[i], input.seeds[i] + input.seeds[i + 1] - 1);
    for (const auto& m : input.mapping) {
        std::vector<ClosedInterval> seedsNext;
        for (const auto& transf : m) {
            if (seeds.empty()) break;
            std::vector<ClosedInterval> seedsContinue;
            for (const auto& seedRng : seeds) {
                auto [u, d1, d2] = seedRng.union_difference(transf.ci);
                if (u) seedsNext.push_back(ClosedInterval{u->a + transf.delta, u->b + transf.delta});
                if (d1) {
                    seedsContinue.push_back(*d1);
                    if (d2) seedsContinue.push_back(*d2);
                }
            }
            seeds = std::move(seedsContinue);
        }
        seeds.insert(end(seeds), begin(seedsNext), end(seedsNext));
    }
    return std::ranges::min(seeds | std::views::transform([](const auto& ci) { return ci.a; }));
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int64_t part1CorrectAnswer = 35;
    const auto part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int64_t part2CorrectAnswer = 46;
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
