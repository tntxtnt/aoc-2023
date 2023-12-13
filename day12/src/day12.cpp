#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <optional>

static constexpr std::string_view kInputFilename = "day12.txt";

using Input = std::vector<std::pair<std::string, std::vector<int>>>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) {
        const auto i = line.find(' ');
        std::istringstream iss(line.substr(i + 1));
        std::vector<int> counts;
        int n{};
        for (char ignore; iss >> n >> ignore;) counts.push_back(n);
        counts.push_back(n);
        res.emplace_back(line.substr(0, i), std::move(counts));
    }
    return res;
}

std::string buildStringFromId(std::string s, unsigned id) {
    for (char& ch : s)
        if (ch == '?') ch = ".#"[id % 2], id /= 2;
    return std::move(s);
}

bool match(std::string_view sv, const std::vector<int>& counts) {
    size_t i = sv.find('#');
    for (int cnt : counts) {
        if (i == sv.npos) return false;
        size_t len = 0;
        while (i + len < sv.size() && sv[i + len] == '#') ++len;
        if (len != cnt) return false;
        i = sv.find('#', i + len);
    }
    return i == sv.npos;
}

int part1(const Input& input) {
    return std::ranges::fold_left(input, 0, [](int sum, const auto& line) {
        auto& [s, counts] = line;
        const unsigned slotCount = static_cast<unsigned>(std::ranges::count(s, '?'));
        int validCount{};
        for (unsigned id = (1U << slotCount); id--;) validCount += match(buildStringFromId(s, id), counts);
        return sum + validCount;
    });
}

struct DynamicProgramming {
    std::string_view s;
    const std::vector<int>& counts;
    std::vector<std::vector<std::vector<std::optional<int64_t>>>> dp;
    DynamicProgramming(std::string_view s, const std::vector<int>& counts) : s{s}, counts{counts} {
        dp.resize(*std::ranges::max_element(counts) + 1,
                  std::vector<std::vector<std::optional<int64_t>>>(
                      s.size() + 1, std::vector<std::optional<int64_t>>(counts.size() + 1, std::nullopt)));
    }
    int64_t solve(int a, int b, int c) {
        if (!dp[a][b][c]) dp[a][b][c] = f(a, b, c);
        return *dp[a][b][c];
    }
    int64_t f(int a, int b, int c) {
        if (c == counts.size()) {
            return a == 0 ? s.substr(b).find('#') == s.npos : 0;
        } else if (b == s.size()) {
            return c == counts.size() - 1 && a == counts[c];
        } else if (a == counts[c]) {
            return s[b] == '#' ? 0 : solve(0, b + 1, c + 1);
        } else {
            if (s[b] == '.') return a == 0 ? solve(0, b + 1, c) : 0;
            if (s[b] == '#') return solve(a + 1, b + 1, c);
            return solve(a + 1, b + 1, c) + (a == 0 ? solve(0, b + 1, c) : 0);
        }
    }
};

int64_t part2(const Input& input) {
    int64_t res{};
    for (auto& [s, counts] : input) {
        std::string newS = s;
        auto newCounts = counts;
        for (int i = 4; i--;) {
            newS += '?';
            newS += s;
            newCounts.insert(end(newCounts), begin(counts), end(counts));
        }
        DynamicProgramming dp(newS, newCounts);
        res += dp.solve(0, 0, 0);
    }
    return res;
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 21;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int64_t part2CorrectAnswer = 525152;
    const int64_t part2Answer = part2(input1);
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
