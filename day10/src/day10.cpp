#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <fmt/ostream.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <queue>
#include <stack>
#include <tuple>

static constexpr std::string_view kInputFilename = "day10.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

enum class Dir { Left, Right, Top, Bottom };

bool isConnectorT(char top) {
    return top == '|' || top == 'F' || top == '7';
};
bool isConnectorB(char bot) {
    return bot == '|' || bot == 'L' || bot == 'J';
};
bool isConnectorL(char lef) {
    return lef == '-' || lef == 'F' || lef == 'L';
};
bool isConnectorR(char rig) {
    return rig == '-' || rig == 'J' || rig == '7';
};

std::pair<size_t, size_t> findStart(const Input& input) {
    for (size_t i = 0; i < input.size(); ++i)
        for (size_t j = 0; j < input[0].size(); ++j)
            if (input[i][j] == 'S') return {i, j};
    return {-1, -1};
}

int part1(const Input& input) {
    const size_t rows = input.size();
    const size_t cols = input[0].size();
    auto [sr, sc] = findStart(input);
    if (sr == -1) return -1;
    std::queue<std::tuple<size_t, size_t, int>> q;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols));
    q.emplace(sr, sc, 0);
    visited[sr][sc] = true;
    auto enqueueUnvisitedNeighbor = [&](size_t r, size_t c, int dist, Dir from) {
        if (!(r < rows && c < cols && input[r][c] != '.' && !visited[r][c])) return;
        if (from == Dir::Left && !isConnectorR(input[r][c])) return;
        if (from == Dir::Right && !isConnectorL(input[r][c])) return;
        if (from == Dir::Top && !isConnectorB(input[r][c])) return;
        if (from == Dir::Bottom && !isConnectorT(input[r][c])) return;
        q.emplace(r, c, dist);
        visited[r][c] = true;
    };
    int res{};
    while (!q.empty()) {
        auto [r, c, d] = q.front();
        q.pop();
        res = std::max(res, d);
        const char rep = input[r][c];
        if (rep == 'S') {
            enqueueUnvisitedNeighbor(r - 1, c, d + 1, Dir::Bottom);
            enqueueUnvisitedNeighbor(r + 1, c, d + 1, Dir::Top);
            enqueueUnvisitedNeighbor(r, c - 1, d + 1, Dir::Right);
            enqueueUnvisitedNeighbor(r, c + 1, d + 1, Dir::Left);
        } else if (rep == '|') {
            enqueueUnvisitedNeighbor(r - 1, c, d + 1, Dir::Bottom);
            enqueueUnvisitedNeighbor(r + 1, c, d + 1, Dir::Top);
        } else if (rep == '-') {
            enqueueUnvisitedNeighbor(r, c - 1, d + 1, Dir::Right);
            enqueueUnvisitedNeighbor(r, c + 1, d + 1, Dir::Left);
        } else if (rep == 'L') {
            enqueueUnvisitedNeighbor(r - 1, c, d + 1, Dir::Bottom);
            enqueueUnvisitedNeighbor(r, c + 1, d + 1, Dir::Left);
        } else if (rep == 'J') {
            enqueueUnvisitedNeighbor(r - 1, c, d + 1, Dir::Bottom);
            enqueueUnvisitedNeighbor(r, c - 1, d + 1, Dir::Right);
        } else if (rep == '7') {
            enqueueUnvisitedNeighbor(r + 1, c, d + 1, Dir::Top);
            enqueueUnvisitedNeighbor(r, c - 1, d + 1, Dir::Right);
        } else if (rep == 'F') {
            enqueueUnvisitedNeighbor(r + 1, c, d + 1, Dir::Top);
            enqueueUnvisitedNeighbor(r, c + 1, d + 1, Dir::Left);
        }
    }
    return res;
}

std::pair<size_t, size_t> recoverStart(Input& input) {
    auto [r, c] = findStart(input);
    if (r == -1) return {-1, -1};
    unsigned kind{}; // 0bTBLR
    if (r - 1 < input.size() && isConnectorT(input[r - 1][c])) kind |= 0b1000;
    if (r + 1 < input.size() && isConnectorB(input[r + 1][c])) kind |= 0b0100;
    if (c - 1 < input[0].size() && isConnectorL(input[r][c - 1])) kind |= 0b0010;
    if (c + 1 < input[0].size() && isConnectorR(input[r][c + 1])) kind |= 0b0001;
    if (kind == 0b1100) input[r][c] = '|';
    if (kind == 0b1010) input[r][c] = 'J';
    if (kind == 0b1001) input[r][c] = 'L';
    if (kind == 0b0110) input[r][c] = '7';
    if (kind == 0b0101) input[r][c] = 'F';
    if (kind == 0b0011) input[r][c] = '-';
    return {r, c};
}

std::tuple<size_t, size_t, Input> getExpandedInput(Input& input) {
    auto [sr, sc] = recoverStart(input);
    Input res;
    res.push_back(std::string(input[0].size() * 2 + 1, ' '));
    for (const auto& line : input) {
        std::string newLine{" "};
        for (char ch : line) newLine += ch, newLine += ' ';
        res.push_back(std::move(newLine));
        res.push_back(std::string(input[0].size() * 2 + 1, ' '));
    }
    for (size_t i = 1; i < res.size(); i += 2)
        for (size_t j = 1; j < res[i].size(); j += 2) {
            if (isConnectorL(res[i][j]) && j + 2 < res[0].size() && isConnectorR(res[i][j + 2])) res[i][j + 1] = '-';
            if (isConnectorT(res[i][j]) && i + 2 < res.size() && isConnectorB(res[i + 2][j])) res[i + 1][j] = '|';
        }
    return {sr, sc, res};
}

void pprint(const Input& input, size_t sr, size_t sc, const std::vector<std::vector<bool>>& isBorder,
            const std::vector<std::vector<bool>>& isOutside) {
    for (size_t i = 0; auto& line : input) {
        for (size_t j = 0; char ch : line) {
            const auto color = i == sr && j == sc ? fg(fmt::color::blue)
                               : isBorder[i][j]   ? fg(fmt::color::pale_green)
                               : isOutside[i][j]  ? fg(fmt::color::pale_golden_rod)
                               : i % 2 && j % 2   ? fg(fmt::color::white)
                                                  : fg(fmt::color::gray);
            if (isOutside[i][j])
                fmt::print(color, "O");
            else if (ch == '|')
                fmt::print(color, "│");
            else if (ch == '-')
                fmt::print(color, "─");
            else if (ch == 'F')
                fmt::print(color, "┌");
            else if (ch == '7')
                fmt::print(color, "┐");
            else if (ch == 'L')
                fmt::print(color, "└");
            else if (ch == 'J')
                fmt::print(color, "┘");
            else
                fmt::print(color, "{}", ch);
            ++j;
        }
        fmt::print("\n");
        ++i;
    }
}

int part2(Input input, bool debug = false) {
    auto [sr, sc, expInp] = getExpandedInput(input);
    if (sr == -1) return -1;
    auto dfs = [](size_t sr, size_t sc, auto& visited, auto&& validNeighbor, auto&& visit) {
        std::stack<std::pair<size_t, size_t>> st;
        auto push = [&](size_t r, size_t c) {
            if (visited[r][c] || !validNeighbor(r, c)) return;
            st.emplace(r, c);
            visited[r][c] = true;
        };
        for (push(sr, sc); !st.empty();) {
            auto [r, c] = st.top();
            st.pop();
            visit(r, c, push);
        }
    };
    // Border
    std::vector<std::vector<bool>> isBorder(expInp.size(), std::vector<bool>(expInp[0].size()));
    dfs(
        sr = 1 + 2 * sr, sc = 1 + 2 * sc, isBorder, [](size_t, size_t) { return true; },
        [&](size_t r, size_t c, auto&& push) {
            if (expInp[r][c] == '|') push(r - 1, c), push(r + 1, c);
            if (expInp[r][c] == '-') push(r, c - 1), push(r, c + 1);
            if (expInp[r][c] == 'F') push(r + 1, c), push(r, c + 1);
            if (expInp[r][c] == '7') push(r + 1, c), push(r, c - 1);
            if (expInp[r][c] == 'L') push(r - 1, c), push(r, c + 1);
            if (expInp[r][c] == 'J') push(r - 1, c), push(r, c - 1);
        });
    // O
    std::vector<std::vector<bool>> isOutside(expInp.size(), std::vector<bool>(expInp[0].size()));
    dfs(
        0, 0, isOutside, [&](size_t r, size_t c) { return !isBorder[r][c]; },
        [&](size_t r, size_t c, auto&& push) {
            if (r - 1 < expInp.size()) push(r - 1, c);
            if (r + 1 < expInp.size()) push(r + 1, c);
            if (c - 1 < expInp[0].size()) push(r, c - 1);
            if (c + 1 < expInp[0].size()) push(r, c + 1);
        });
    int res{};
    for (size_t i = 1; i < expInp.size(); i += 2)
        for (size_t j = 1; j < expInp[0].size(); j += 2)
            if (!isBorder[i][j] && !isOutside[i][j]) ++res;
    if (debug) pprint(expInp, sr, sc, isBorder, isOutside);
    return res;
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
..F7.
.FJ|.
SJ.L7
|F--J
LJ...
)",
                                                                8}};
    bool part1Correct = true;
    for (auto [sv, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, correctAnswer);
    }

    auto testPart2 = [](std::istream& is, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part2(input, true);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 2: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::pair<std::string_view, int> part2Cases[] = {{R"(
.F----7F7F7F7F-7....
.|F--7||||||||FJ....
.||.FJ||||||||L7....
FJL7L7LJLJ||LJ.L-7..
L--J.L7...LJS7F-7L7.
....F-J..F7FJ|L7L7L7
....L7.F7||L7|.L7L7|
.....|FJLJ|FJ|F7|.LJ
....FJL-7.||.||||...
....L---J.LJ.LJLJ...
)",
                                                                8},
                                                               {R"(
FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJ7F7FJ-
L---JF-JLJ.||-FJLJJ7
|F|F-JF---7F7-L7L|7|
|FFJF7L7F-JF7|JL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L
)",
                                                                10},
                                                               {R"(
F-S---7
|.....|
|.F-7.|
|.|.|.|
|.L-J.|
|.....|
L-----J
)",
                                                                25}};
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
    fmt::print("Part 1: {}\n", fmt::styled(part1(input), fmt::fg(fmt::color::yellow)));
    if (!test2) return 2;
    fmt::print("Part 2: {}\n", fmt::styled(part2(input, true), fmt::fg(fmt::color::yellow)));
}
