#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <queue>
#include <stack>
#include <array>
#include <unordered_map>
#include <unordered_set>
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day23.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

int part1(const Input& input) {
    std::queue<std::tuple<int, size_t, size_t, size_t, size_t>> q;
    std::vector<std::vector<int>> visited(input.size(), std::vector<int>(input[0].size()));
    q.emplace(0, 0, 1, -1, -1);
    while (!q.empty()) {
        auto [dist, r, c, fr, fc] = q.front();
        q.pop();
        visited[r][c] = dist;
        if (input[r][c] == '.') {
            for (auto [dr, dc] : std::array<std::pair<int, int>, 4>{{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}})
                if (const size_t nr = r + dr, nc = c + dc;
                    nr < input.size() && nc < input[0].size() && (nr != fr || nc != fc) && input[nr][nc] != '#')
                    q.emplace(dist + 1, nr, nc, r, c);
        } else if (input[r][c] == '^') {
            if (const size_t nr = r - 1, nc = c; nr < input.size() && nr != fr && input[nr][nc] != '#')
                q.emplace(dist + 1, nr, nc, r, c);
        } else if (input[r][c] == '>') {
            if (const size_t nr = r, nc = c + 1; nc < input[0].size() && nc != fc && input[nr][nc] != '#')
                q.emplace(dist + 1, nr, nc, r, c);
        } else if (input[r][c] == 'v') {
            if (const size_t nr = r + 1, nc = c; nr < input.size() && nr != fr && input[nr][nc] != '#')
                q.emplace(dist + 1, nr, nc, r, c);
        } else if (input[r][c] == '<') {
            if (const size_t nr = r, nc = c - 1; nc < input[0].size() && nc != fc && input[nr][nc] != '#')
                q.emplace(dist + 1, nr, nc, r, c);
        }
    }
    return visited[input.size() - 1][input[0].size() - 2];
}

constexpr int kMult = 1000;

int toInt(size_t r, size_t c) {
    return (int)(r * kMult + c);
}

std::pair<size_t, size_t> fromInt(int v) {
    return {v / kMult, v % kMult};
}

std::unordered_map<int, std::vector<std::pair<int, int>>> parseAdjMap(const Input& input) {
    std::unordered_map<int, std::vector<std::pair<int, int>>> res;
    res[toInt(0, 1)];
    res[toInt(input.size() - 1, input[0].size() - 2)];
    for (size_t r = 0; r < input.size(); ++r) {
        for (size_t c = 0; c < input[0].size(); ++c) {
            if (input[r][c] == '#') continue;
            int nbCnt{};
            for (auto [dr, dc] : std::array<std::pair<int, int>, 4>{{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}})
                if (const size_t nr = r + dr, nc = c + dc;
                    nr < input.size() && nc < input[0].size() && input[nr][nc] != '#')
                    ++nbCnt;
            if (nbCnt > 2) res[toInt(r, c)];
        }
    }
    for (int k : res | views::keys) {
        auto [sr, sc] = fromInt(k);
        std::queue<std::tuple<size_t, size_t, int>> q;
        std::unordered_set<int> visited;
        q.emplace(sr, sc, 0);
        visited.insert(toInt(sr, sc));
        while (!q.empty()) {
            auto [r, c, dist] = q.front();
            q.pop();
            const int ki = toInt(r, c);
            if (ki != k && res.count(ki) != 0) {
                res[k].emplace_back(ki, dist);
                continue;
            }
            for (auto [dr, dc] : std::array<std::pair<int, int>, 4>{{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}})
                if (const size_t nr = r + dr, nc = c + dc;
                    nr < input.size() && nc < input[0].size() && input[nr][nc] != '#') {
                    const int vi = toInt(nr, nc);
                    if (visited.count(vi) != 0) continue;
                    q.emplace(nr, nc, dist + 1);
                    visited.insert(vi);
                }
        }
    }
    return res;
}

int part2(Input input) {
    for (auto& row : input)
        for (char& ch : row)
            if (ch != '#') ch = '.';
    auto adj = parseAdjMap(input);

    std::stack<std::pair<int, int>> st;
    st.emplace(toInt(0, 1), 0);
    std::unordered_set<int> visited;
    const int dest = toInt(input.size() - 1, input[0].size() - 2);
    int res{};
    while (!st.empty()) {
        auto [k, dist] = st.top();
        st.pop();
        if (dist == -1) {
            visited.erase(k);
            continue;
        }
        visited.insert(k);
        st.emplace(k, -1);
        for (auto& e : adj[k]) {
            auto [kn, w] = e;
            if (visited.count(kn) != 0) continue;
            if (kn == dest) {
                res = std::max(res, dist + w);
            } else {
                st.emplace(kn, dist + w);
            }
        }
    }
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
#.#####################
#.......#########...###
#######.#########.#.###
###.....#.>.>.###.#.###
###v#####.#v#.###.#.###
###.>...#.#.#.....#...#
###v###.#.#.#########.#
###...#.#.#.......#...#
#####.#.#.#######.#.###
#.....#.#.#.......#...#
#.#####.#.#.#########v#
#.#...#...#...###...>.#
#.#.#v#######v###.###v#
#...#.>.#...>.>.#.###.#
#####v#.#.###v#.#.###.#
#.....#...#...#.#.#...#
#.#########.###.#.#.###
#...###...#...#...#.###
###.###.#.###v#####v###
#...#...#.#.>.>.#.>.###
#.###.###.#.###.#.#v###
#.....###...###...#...#
#####################.#
)",
                                                                94}};
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
#.#####################
#.......#########...###
#######.#########.#.###
###.....#.>.>.###.#.###
###v#####.#v#.###.#.###
###.>...#.#.#.....#...#
###v###.#.#.#########.#
###...#.#.#.......#...#
#####.#.#.#######.#.###
#.....#.#.#.......#...#
#.#####.#.#.#########v#
#.#...#...#...###...>.#
#.#.#v#######v###.###v#
#...#.>.#...>.>.#.###.#
#####v#.#.###v#.#.###.#
#.....#...#...#.#.#...#
#.#########.###.#.#.###
#...###...#...#...#.###
###.###.#.###v#####v###
#...#...#.#.>.>.#.>.###
#.###.###.#.###.#.#v###
#.....###...###...#...#
#####################.#
)",
                                                                154}};
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
