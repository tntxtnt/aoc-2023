#include <fmt/format.h>
#include <fmt/color.h>
#include <sstream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <cctype>
#include <fstream>
#include <array>

int part1(const std::vector<std::string>& input) {
    return std::accumulate(begin(input), end(input), 0, [](int sum, std::string_view s) {
        const auto it = std::ranges::find_if(s, [](unsigned char c) { return std::isdigit(c); });
        if (it == end(s)) return sum;
        const auto jt = std::ranges::find_last_if(s, [](unsigned char c) { return std::isdigit(c); }).begin();
        return sum + 10 * (*it - '0') + *jt - '0';
    });
}

constexpr std::array<std::string_view, 9> englishNumbers{"one", "two",   "three", "four", "five",
                                                         "six", "seven", "eight", "nine"};

std::pair<size_t, int> findFirstOfEnglishNumber(std::string_view s) {
    for (size_t i = 0; i < s.size(); ++i)
        if (auto it = std::ranges::find_if(englishNumbers, [sv = s.substr(i)](auto en) { return sv.starts_with(en); });
            it != end(englishNumbers))
            return {i, static_cast<int>(std::distance(begin(englishNumbers), it) + 1)};
    return {std::string::npos, 0};
}

std::pair<size_t, int> findLastOfEnglishNumber(std::string_view s) {
    for (size_t i = s.size() + 1; i--;)
        if (auto it = std::ranges::find_if(englishNumbers, [sv = s.substr(0, i)](auto en) { return sv.ends_with(en); });
            it != end(englishNumbers))
            return {i - it->size(), static_cast<int>(std::distance(begin(englishNumbers), it) + 1)};
    return {std::string::npos, 0};
}

int part2(const std::vector<std::string>& input) {
    return std::accumulate(begin(input), end(input), 0, [](int sum, auto& s) {
        const auto it = std::ranges::find_if(s, [](unsigned char c) { return std::isdigit(c); });
        const auto jt = std::ranges::find_last_if(s, [](unsigned char c) { return std::isdigit(c); }).begin();
        const size_t i1 = std::distance(begin(s), it);
        const size_t j1 = std::distance(begin(s), jt);
        const auto [i2, i2val] = findFirstOfEnglishNumber(s);
        const auto [j2, j2val] = findLastOfEnglishNumber(s);
        const int left = i1 < i2 ? *it - '0' : i2val;
        const int right = jt == end(s) ? j2val : j2 == s.npos ? *jt - '0' : j1 > j2 ? *jt - '0' : j2val;
        return sum + 10 * left + right;
    });
}

std::vector<std::string> parseInput(std::istream& in);
bool test();

int main() {
    if (!test()) return 1;
    if (auto in = std::ifstream("day1.txt"); !in) {
        fmt::print("Cannot open 'day1.txt'\n");
        return 2;
    } else {
        const auto input = parseInput(in);
        fmt::print("Part 1: {}\n", fmt::styled(part1(input), fmt::fg(fmt::color::yellow)));
        fmt::print("Part 2: {}\n", fmt::styled(part2(input), fmt::fg(fmt::color::yellow)));
    }
}


std::vector<std::string> parseInput(std::istream& in) {
    std::vector<std::string> res;
    for (std::string s; in >> s;) res.push_back(s);
    return res;
}

bool test() {
    std::istringstream iss1{R"(
1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    const int part1CorrectAnswer = 142;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    std::istringstream iss2{R"(
two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen
)"};
    iss2.ignore();
    const auto input2 = parseInput(iss2);

    const int part2CorrectAnswer = 281;
    const int part2Answer = part2(input2);
    const bool part2Correct = part2Answer == part2CorrectAnswer;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer,
               fmt::styled(part2Answer, fmt::fg(part2Correct ? fmt::color::green : fmt::color::red)));

    return part1Correct && part2Correct;
}