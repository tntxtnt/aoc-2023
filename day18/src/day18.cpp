#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <set>
#include <stack>
#include <ranges>

static constexpr std::string_view kInputFilename = "day18.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

int part1(const Input& input, int sr, int sc) {
    std::set<std::pair<int, int>> trenches;
    trenches.emplace(0, 0);
    for (int r = 0, c = 0; auto& line : input) {
        std::istringstream iss{line};
        char dir;
        int len;
        iss >> dir >> len;
        const int dr = dir == 'U' ? -1 : dir == 'D' ? 1 : 0;
        const int dc = dir == 'L' ? -1 : dir == 'R' ? 1 : 0;
        while (len--) trenches.emplace(r += dr, c += dc);
    }
    int res{};
    std::set<std::pair<int, int>> visited;
    std::stack<std::pair<int, int>> st;
    st.emplace(sr, sc);
    visited.emplace(sr, sc);
    while (!st.empty()) {
        auto [r, c] = st.top();
        st.pop();
        ++res;
        for (int dr : {-1, 0, 1}) {
            for (int dc : {-1, 0, 1}) {
                if (int nr = r + dr, nc = c + dc;
                    trenches.count(std::make_pair(nr, nc)) == 0 && visited.count(std::make_pair(nr, nc)) == 0) {
                    visited.emplace(nr, nc);
                    st.emplace(nr, nc);
                }
            }
        }
        if (trenches.count(std::make_pair(r, c)) > 0) continue;
    }
    return res + (int)trenches.size();
}

int part2(const Input& input) {
    return static_cast<int>(input.size());
}

std::pair<bool, bool> test() {
    auto testPart1 = [](std::istream& is, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part1(input, 1, 1);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 1: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    auto testPart2 = [](std::istream& is, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part2(input);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 2: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };

    constexpr std::pair<std::string_view, int> part1Cases[] = {{R"(
R 6 (#70c710)
D 5 (#0dc571)
L 2 (#5713f0)
D 2 (#d2c081)
R 2 (#59c680)
D 2 (#411b91)
L 5 (#8ceee2)
U 2 (#caa173)
L 1 (#1b58a2)
U 2 (#caa171)
R 2 (#7807d2)
U 3 (#a77fa3)
L 2 (#015232)
U 2 (#7a21e3)
)",
                                                                62}};
    bool part1Correct = true;
    for (auto [sv, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, correctAnswer);
    }

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
    fmt::print("Part 1: {}\n", fmt::styled(part1(input, 1, 1), fmt::fg(fmt::color::yellow)));
    if (!test2) return 2;
    fmt::print("Part 2: {}\n", fmt::styled(part2(input), fmt::fg(fmt::color::yellow)));
}
