#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <queue>

static constexpr std::string_view kInputFilename = "day17.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

auto dir2i = [](char dir) {
    if (dir == '>') return 0;
    if (dir == '<') return 1;
    if (dir == '^') return 2;
    return 3;
};
auto dir2p = [](char dir) -> std::pair<int, int> {
    if (dir == '>') return {0, 1};
    if (dir == '<') return {0, -1};
    if (dir == '^') return {-1, 0};
    return {1, 0};
};
auto dirccw = [](char dir) {
    if (dir == '>') return '^';
    if (dir == '<') return 'v';
    if (dir == '^') return '<';
    return '>';
};
auto dircw = [](char dir) {
    if (dir == '>') return 'v';
    if (dir == '<') return '^';
    if (dir == '^') return '>';
    return '<';
};

int part1(const Input& input) {
    const size_t rows = input.size();
    const size_t cols = input[0].size();
    std::vector<std::vector<std::array<int, 12>>> visited(
        rows, std::vector<std::array<int, 12>>(cols, std::array<int, 12>{}));
    auto getVisited = [&](size_t r, size_t c, char dir, int len) -> int& {
        return visited[r][c][dir2i(dir) * 3 + len];
    };
    using PqTp = std::tuple<int, size_t, size_t, char, int>;
    std::priority_queue<PqTp, std::vector<PqTp>, std::greater<PqTp>> pq;
    auto push = [&](size_t r, size_t c, char newDir, int newLen, int dist) {
        const auto [dr, dc] = dir2p(newDir);
        if (size_t nr = r + dr, nc = c + dc; nr < rows && nc < cols)
            if (getVisited(nr, nc, newDir, newLen) == 0) pq.emplace(dist + input[nr][nc] - '0', nr, nc, newDir, newLen);
    };
    pq.emplace(input[0][1] - '0', 0, 1, '>', 2);
    pq.emplace(input[1][0] - '0', 1, 0, 'v', 2);
    while (!pq.empty()) {
        auto [dist, r, c, dir, len] = pq.top();
        pq.pop();
        if (getVisited(r, c, dir, len) != 0) continue;
        if (r + 1 == rows && c + 1 == cols) return dist;
        if (len > 0) push(r, c, dir, len - 1, dist); // straigt
        push(r, c, dirccw(dir), 2, dist);            // CCW
        push(r, c, dircw(dir), 2, dist);             // CW
        getVisited(r, c, dir, len) = dist;
    }
    return 0;
}

int part2(const Input& input) {
    const size_t rows = input.size();
    const size_t cols = input[0].size();
    std::vector<std::vector<std::array<int, 40>>> visited(
        rows, std::vector<std::array<int, 40>>(cols, std::array<int, 40>{}));
    auto getVisited = [&](size_t r, size_t c, char dir, int len) -> int& {
        return visited[r][c][dir2i(dir) * 10 + len - 1];
    };
    using PqTp = std::tuple<int, size_t, size_t, char, int>;
    std::priority_queue<PqTp, std::vector<PqTp>, std::greater<PqTp>> pq;
    auto push = [&](size_t r, size_t c, char newDir, int newLen, int dist) {
        const auto [dr, dc] = dir2p(newDir);
        if (size_t nr = r + dr, nc = c + dc; nr < rows && nc < cols)
            if (getVisited(nr, nc, newDir, newLen) == 0) pq.emplace(dist + input[nr][nc] - '0', nr, nc, newDir, newLen);
    };
    pq.emplace(input[0][1] - '0', 0, 1, '>', 1);
    pq.emplace(input[1][0] - '0', 1, 0, 'v', 1);
    while (!pq.empty()) {
        auto [dist, r, c, dir, len] = pq.top();
        pq.pop();
        if (getVisited(r, c, dir, len) != 0) continue;
        if (r + 1 == rows && c + 1 == cols && len >= 4) return dist;
        if (len < 10) push(r, c, dir, len + 1, dist);   // straigt
        if (len >= 4) push(r, c, dirccw(dir), 1, dist); // CCW
        if (len >= 4) push(r, c, dircw(dir), 1, dist);  // CW
        getVisited(r, c, dir, len) = dist;
    }
    return 0;
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
2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533
)",
                                                                102}};
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
2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533
)",
                                                                94},
                                                               {R"(
111111111111
999999999991
999999999991
999999999991
999999999991
)",
                                                                71},
                                                               {R"(
3354334
3354645
4112534
3551413
3231515
)",
                                                                30}};
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
