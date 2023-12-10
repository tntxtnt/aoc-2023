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

void recoverStart(Input& input) {
    auto [r, c] = findStart(input);
    if (r == -1) return;
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
}

Input getExpandedInput(Input& input) {
    recoverStart(input);
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
    return res;
}

void pprint(const Input& input) {
    for (auto& line : input) {
        for (char ch : line) {
            if (ch == '|')
                fmt::print("│");
            else if (ch == '-')
                fmt::print("─");
            else if (ch == 'F')
                fmt::print("┌");
            else if (ch == '7')
                fmt::print("┐");
            else if (ch == 'L')
                fmt::print("└");
            else if (ch == 'J')
                fmt::print("┘");
            else
                fmt::print("{}", ch);
        }
        fmt::print("\n");
    }
    fmt::print("│\n");
}

int part2(Input input, bool debug = false) {
    Input expInp = getExpandedInput(input);
    // O
    std::stack<std::pair<size_t, size_t>> st;
    auto pushStack = [&](size_t r, size_t c) {
        st.emplace(r, c);
        expInp[r][c] = 'O';
    };
    pushStack(0, 0);
    auto pushUnvisitedNeighbor = [&](size_t r, size_t c) {
        if (r < expInp.size() && c < expInp[0].size() && (expInp[r][c] == '.' || expInp[r][c] == ' ')) pushStack(r, c);
    };
    while (!st.empty()) {
        auto [r, c] = st.top();
        st.pop();
        pushUnvisitedNeighbor(r + 1, c);
        pushUnvisitedNeighbor(r - 1, c);
        pushUnvisitedNeighbor(r, c + 1);
        pushUnvisitedNeighbor(r, c - 1);
    }
    // I
    int res{};
    auto markI = [&](size_t i, size_t j) {
        std::stack<std::pair<size_t, size_t>> sti;
        auto pushSti = [&](size_t r, size_t c) {
            sti.emplace(r, c);
            expInp[r][c] = 'I';
            ++res;
        };
        pushSti(i, j);
        while (!sti.empty()) {
            auto [r, c] = sti.top();
            sti.pop();
            if (size_t nr = r - 2; nr < expInp.size() && isConnectorT(expInp[nr][c])) pushSti(nr, c);
            if (size_t nr = r + 2; nr < expInp.size() && isConnectorB(expInp[nr][c])) pushSti(nr, c);
            if (size_t nc = c - 2; nc < expInp[0].size() && isConnectorL(expInp[r][nc])) pushSti(r, nc);
            if (size_t nc = c + 2; nc < expInp[0].size() && isConnectorR(expInp[r][nc])) pushSti(r, nc);
        }
    };
    auto isInsideT = [](char top) { return !isConnectorT(top) && top != 'O'; };
    auto isInsideB = [](char bot) { return !isConnectorB(bot) && bot != 'O'; };
    auto isInsideL = [](char lef) { return !isConnectorL(lef) && lef != 'O'; };
    auto isInsideR = [](char rig) { return !isConnectorR(rig) && rig != 'O'; };
    for (size_t i = 0; i < input.size(); ++i) {
        const size_t r = 1 + i * 2;
        for (size_t j = 0; j < input[0].size(); ++j) {
            const size_t c = 1 + j * 2;
            const char ch = expInp[r][c];
            if (ch == '.') {
                markI(r, c);
            } else if (ch == '|' && (isInsideT(expInp[r - 1][c]) || isInsideB(expInp[r + 1][c]))) {
                markI(r, c);
            } else if (ch == '-' && (isInsideL(expInp[r][c - 1]) || isInsideR(expInp[r][c + 1]))) {
                markI(r, c);
            } else if (ch == 'F' && (isInsideB(expInp[r + 1][c]) || isInsideR(expInp[r][c + 1]))) {
                markI(r, c);
            } else if (ch == '7' && (isInsideL(expInp[r][c - 1]) || isInsideB(expInp[r + 1][c]))) {
                markI(r, c);
            } else if (ch == 'L' && (isInsideT(expInp[r - 1][c]) || isInsideR(expInp[r][c + 1]))) {
                markI(r, c);
            } else if (ch == 'J' && (isInsideL(expInp[r][c - 1]) || isInsideT(expInp[r - 1][c]))) {
                markI(r, c);
            }
        }
    }
    if (debug) pprint(expInp);
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
                                                                10}};
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
    fmt::print("Part 2: {}\n", fmt::styled(part2(input), fmt::fg(fmt::color::yellow)));
}
