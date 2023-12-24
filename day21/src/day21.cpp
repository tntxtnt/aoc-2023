#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <queue>
#include <array>
#include <unordered_map>
#include <ranges>
#include <chrono>
namespace cron = std::chrono;
using namespace std::chrono_literals;
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day21.txt";

namespace std
{
template <>
struct hash<std::pair<int, int>> {
    size_t hashCombine(size_t seed, int val) const {
        return seed ^= std::hash<int>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    size_t operator()(const std::pair<int, int>& x) const { return hashCombine(hashCombine(0, x.first), x.second); }
};
} // namespace std

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

int part1(const Input& input, int steps) {
    int sr{-1};
    int sc{-1};
    for (auto [r, c] : views::cartesian_product(views::iota(0U, input.size()), views::iota(0U, input[0].size())))
        if (input[r][c] == 'S') {
            sr = r;
            sc = c;
            break;
        }
    std::queue<std::tuple<size_t, size_t, int>> q;
    q.emplace(sr, sc, 0);
    std::vector<std::vector<int>> visited(input.size(), std::vector<int>(input[0].size(), -1));
    visited[sr][sc] = 0;
    while (!q.empty()) {
        auto [r, c, dist] = q.front();
        if (dist == steps) break;
        q.pop();
        for (auto [dr, dc] : std::array<std::pair<int, int>, 4>{{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}})
            if (const size_t nr = r + dr, nc = c + dc;
                r < input.size() && c < input[0].size() && input[nr][nc] != '#' && visited[nr][nc] < dist + 1) {
                visited[nr][nc] = dist + 1;
                q.emplace(nr, nc, dist + 1);
            }
    }
    return static_cast<int>(q.size());
}

int64_t part2(const Input& input, int steps) {
    int sr{};
    int sc{};
    for (auto [r, c] : views::cartesian_product(views::iota(0U, input.size()), views::iota(0U, input[0].size())))
        if (input[r][c] == 'S') {
            sr = r;
            sc = c;
            break;
        }
    std::queue<std::tuple<int, int, int>> q;
    q.emplace(sr, sc, 0);
    std::unordered_map<std::pair<int, int>, int> visited;
    visited.emplace(std::make_pair(sr, sc), 0);
    auto loop = [](int r, int rows) {
        r -= r / rows * rows;
        r += rows;
        return r % rows;
    };
    auto [quot, rem] = std::div(steps, (int)input.size());
    int prevDist = 0;
    std::vector<int64_t> f;
    while (!q.empty()) {
        auto [r, c, dist] = q.front();
        if (dist > prevDist) {
            prevDist = dist;
            if (prevDist == rem) {
                f.push_back(q.size());
                rem += (int)input.size();
                fmt::print("{} {}\n", dist, q.size());
            }
        }
        if (f.size() == 3) break;
        if (dist == steps) break;
        q.pop();
        for (auto& [dr, dc] : std::array<std::pair<int, int>, 4>{{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}})
            if (const int nr = r + dr, nc = c + dc, rr = loop(nr, (int)input.size()),
                cc = loop(nc, (int)input[0].size());
                input[rr][cc] != '#' && visited[std::make_pair(nr, nc)] < dist + 1) {
                visited[std::make_pair(nr, nc)] = dist + 1;
                q.emplace(nr, nc, dist + 1);
            }
    }
    /* Lagrange Quadratic Interpolation Using Basis Functions
v0(x) = (x - x1)(x - x2) / (x0 - x1)(x0 - x2)
v1(x) = (x - x0)(x - x2) / (x1 - x0)(x1 - x2)
v2(x) = (x - x0)(x - x1) / (x2 - x0)(x2 - x1)
g(x) = f0v0(x) + f1v1(x) + f2v2(x)
     =  f0 h0(x) / 2L^2
      + f1 h1(x) / -L^2
      + f2 h2(x) / 2L^2
2L^2 * g(x) = f0(x^2 - (x1 + x2)x + x1x2) - 2f1(x^2 - (x0 + x2)x + x0x2) + f2(x^2 - (x0 + x1)x + x0x1)
            = (f0 - 2f1 + f2)x^2 - (f0x1 + f0x2 - 2f1x0 - 2f1x2 + f2x0 + f2x1)x + f0x1x2 - 2f1x0x2 + f2x0x1
            = (f0 - 2f1 + f2)x^2 - (x1(f0 + f2) + x2(f0 - 2f1) + x0(f2 - 2f1))x + x2(f0x1 - f1x0) + x0(f2x1 - f1x2)

Let z = ceil(x / L)
z0 = 1
z1 = 2
z2 = 3
2 g(z) = (f0 - 2f1 + f2)z^2 - (z1(f0 + f2) + z2(f0 - 2f1) + z0(f2 - 2f1))z + z2(f0z1 - f1z0) + z0(f2z1 - f1z2)
       = (f0 - 2f1 + f2)z^2 - (2f0 + 2f2 + 3f0 - 6f1 + f2 - 2f1)z + 3(2f0 - f1) + 2f2 - 3f1
       = (f0 - 2f1 + f2)z^2 - (5f0 + 3f2 - 8f1)z + 6f0 - 6f1 + 2f2
Let m = f1 - f0
    n = f2 - f1
--> a = (n - m) / 2               2a = f0 - 2f1 + f2
    b = m - 3a                    2b = (2f1 - 2f0 - 3*(f0 - 2f1 + f2)) = -5f0 + 8f1 - 3f2
    c = f0 - b - a                2c = 2f0 + 5f0 - 8f1 + 3f2 - f0 + 2f1 - f2 = 6f0 - 6f1 + 2f2

Let z = floor(x / L)
z0 = 0
z1 = 1
z2 = 2
2 g(z) = (f0 - 2f1 + f2)z^2 - (z1(f0 + f2) + z2(f0 - 2f1) + z0(f2 - 2f1))z + z2(f0z1 - f1z0) + z0(f2z1 - f1z2)
       = (f0 - 2f1 + f2)z^2 - (f0 + f2 + 2f0 - 4f1)z + 2f0
       = (f0 - 2f1 + f2)z^2 - (3f0 - 4f1 + f2)z + 2f0
Let m = f1 - f0
    n = f2 - f1
--> a = (n - m) / 2          2a = f0 - 2f1 + f2
    b = (3m - n) / 2         2b = 3f1 - 3f0 - f2 + f1 = -3f0 + 4f1 - f2
    c = f0                   2c = 2f0

Let z = floor(x / L) - 1
z0 = -1
z1 = 0
z2 = 1
2 g(z) = (f0 - 2f1 + f2)z^2 - (z1(f0 + f2) + z2(f0 - 2f1) + z0(f2 - 2f1))z + z2(f0z1 - f1z0) + z0(f2z1 - f1z2)
       = (f0 - 2f1 + f2)z^2 - (f0 - 2f1 - f2 + 2f1)z + 2f1
       = (f0 - 2f1 + f2)z^2 + (f2 - f0)z + 2f1
Let m = f1 - f0
    n = f2 - f1
--> a = (n - m) / 2          2a = f0 - 2f1 + f2
    b = (n + m) / 2          2b = f2 - f0
    c = f1                   2c = 2f1
    */
    const auto c = f[1];
    const auto b = (f[2] - f[0]) / 2;
    const auto a = b - c + f[0];
    --quot;
    fmt::print("{}*x^2 + {}*x + {}\n", a, b, c);
    return a * quot * quot + b * quot + c;
}

std::pair<bool, bool> test() {
    auto testPart1 = [](std::istream& is, int steps, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part1(input, steps);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 1: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::tuple<std::string_view, int, int> part1Cases[] = {{R"(
...........
.....###.#.
.###.##..#.
..#.#...#..
....#.#....
.##..S####.
.##..#...#.
.......##..
.##.#.####.
.##..##.##.
...........
)",
                                                                      6, 16}};
    bool part1Correct = true;
    for (auto [sv, steps, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, steps, correctAnswer);
    }

    auto testPart2 = [](std::istream& is, int steps, int64_t correctAnswer) {
        const auto input = parseInput(is);
        const auto answer = part2(input, steps);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 2: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::tuple<std::string_view, int, int64_t> part2Cases[] = {{R"(
...........
.....###.#.
.###.##..#.
..#.#...#..
....#.#....
.##..S####.
.##..#...#.
.......##..
.##.#.####.
.##..##.##.
...........
)",
                                                                          100, 5978 /*6536*/}};
    bool part2Correct = true;
    for (auto [sv, steps, correctAnswer] : part2Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part2Correct &= testPart2(iss, steps, correctAnswer);
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

    auto getTimeColor = [](const auto& elapsed) {
        return elapsed < 100ms ? fmt::color::light_green : elapsed < 1s ? fmt::color::orange : fmt::color::orange_red;
    };
    auto startTime = cron::steady_clock::now();
    const auto part1Ans = part1(input, 64);
    cron::duration<double> elapsed = cron::steady_clock::now() - startTime;
    fmt::print("Part 1: {} in {}\n", fmt::styled(part1Ans, fmt::fg(fmt::color::yellow)),
               fmt::styled(fmt::format("{:.06f}s", elapsed.count()), fmt::fg(getTimeColor(elapsed))));

    if (!test2) return 2;
    startTime = cron::steady_clock::now();
    const auto part2Ans = part2(input, 26501365);
    elapsed = cron::steady_clock::now() - startTime;
    fmt::print("Part 2: {} in {}\n", fmt::styled(part2Ans, fmt::fg(fmt::color::yellow)),
               fmt::styled(fmt::format("{:.06f}s", elapsed.count()), fmt::fg(getTimeColor(elapsed))));
}
