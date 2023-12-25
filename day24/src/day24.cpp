#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
namespace cron = std::chrono;
using namespace std::chrono_literals;
namespace ranges = std::ranges;
namespace views = std::views;
#include <boost/multiprecision/cpp_int.hpp>
using int128_t = boost::multiprecision::int128_t;

static constexpr std::string_view kInputFilename = "day24.txt";

struct Point3 {
    int64_t x{};
    int64_t y{};
    int64_t z{};
};

struct Hailstone {
    Point3 pos;
    Point3 vel;
};

using Input = std::vector<Hailstone>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) {
        std::istringstream iss{line};
        Hailstone hs;
        char ignore;
        iss >> hs.pos.x >> ignore >> hs.pos.y >> ignore >> hs.pos.z >> ignore //
            >> hs.vel.x >> ignore >> hs.vel.y >> ignore >> hs.vel.z;
        res.push_back(hs);
    }
    return res;
}

// Intersection of 2 lines
// x = x1 + vx1 t1
// y = y1 + vy1 t1
// and
// x = x2 + vx2 t2
// y = y2 + vy2 t2
// We have
// x1 + vx1 t1 = x2 + vx2 t2
// y1 + vy1 t1 = y2 + vy2 t2
// <=>
// vx1 t1 - vx2 t2 = x2 - x1
// vy1 t1 - vy2 t2 = y2 - y1
// |vx1 -vx2| |t1| = |x2-x1|
// |vy1 -vy2| |t2|   |y2-y1|
// <=>
// |a1 b1| |t1| = |c1|
// |a2 b2| |t2|   |c2|
std::optional<std::pair<double, double>> integerIntersection(int64_t x1, int64_t y1, int64_t vx1, int64_t vy1,
                                                             int64_t x2, int64_t y2, int64_t vx2, int64_t vy2) {
    const int64_t a1 = vx1;
    const int64_t a2 = vy1;
    const int64_t b1 = -vx2;
    const int64_t b2 = -vy2;
    const int64_t c1 = x2 - x1;
    const int64_t c2 = y2 - y1;
    const int64_t det = a1 * b2 - a2 * b1;
    if (det == 0) return std::nullopt; // parallel
    const double t1 = static_cast<double>(c1 * b2 - c2 * b1) / det;
    const double t2 = static_cast<double>(a1 * c2 - a2 * c1) / det;
    return std::make_pair(t1, t2);
}

bool willCrossInside(const Hailstone& a, const Hailstone& b, int64_t from, int64_t to) {
    auto p = integerIntersection(a.pos.x, a.pos.y, a.vel.x, a.vel.y, //
                                 b.pos.x, b.pos.y, b.vel.x, b.vel.y);
    if (!p) return false; // parallel
    auto [ta, tb] = *p;
    if (ta < 0 || tb < 0) return false; // happened in the past
    const double x = a.pos.x + a.vel.x * ta;
    const double y = a.pos.y + a.vel.y * ta;
    return from <= x && x <= to && from <= y && y <= to;
}

int part1(const Input& input, int64_t from, int64_t to) {
    int res{};
    for (size_t i = 0; i < input.size(); ++i)
        for (size_t j = i + 1; j < input.size(); ++j)
            if (willCrossInside(input[i], input[j], from, to)) ++res;
    return res;
}

// compute a^d mod n
int64_t modPow(int64_t a, int64_t d, int64_t n) {
    int64_t ret = 1;
    int64_t apow = a;
    for (; d; d >>= 1) {
        if (d % 2) ret = (int128_t(ret) * apow % n).convert_to<int64_t>();
        apow = (int128_t(apow) * apow % n).convert_to<int64_t>();
    }
    return ret;
}

bool witness(int64_t a, int64_t n, int64_t d, int64_t r) {
    auto x = modPow(a, d, n);
    if (x == 1 || x == n - 1) return true;
    for (auto i = r - 1; i-- > 0;) {
        x = modPow(x, 2, n);
        if (x == 1) return false;
        if (x == n - 1) return true;
    }
    return false;
}

bool isPrimeDetMillerRabin(int64_t n) {
    int64_t d = n - 1;
    int64_t r = 0;
    for (; d % 2 == 0; d /= 2) ++r; // n-1 = d * 2^r
    return ranges::all_of(std::array<int64_t, 7>{2, 325, 9375, 28178, 450775, 9780504, 1795265022},
                          [=](int64_t a) { return witness(a, n, d, r); });
}

int64_t pollardsRho(int64_t n) {
    auto g = [n](int64_t x) { return ((int128_t(x) * x + 1) % n).convert_to<int64_t>(); };
    int64_t d = 1;
    for (int64_t x = 2, y = 2; d == 1;) {
        x = g(x);
        y = g(g(y));
        d = std::gcd(x > y ? x - y : y - x, n);
    }
    // if (d == n) throw "Failure";
    return d;
}

std::unordered_map<int64_t, int> getPrimeFactors(int64_t n) {
    if (n < 0) n = -n;
    std::unordered_map<int64_t, int> res;
    if (n == 0) return res;
    // 2
    for (; n % 2 == 0; n /= 2) ++res[2];
    // "small" odd integers
    constexpr int iMax = 31623;
    for (int i = 3; i * i < n && i < iMax; i += 2)
        for (; n % i == 0; n /= i) ++res[i];
    while (n > 1) {
        // MR
        if (isPrimeDetMillerRabin(n)) {
            ++res[n];
            break;
        } else {
            // Pollard's rho
            auto d = pollardsRho(n);
            ++res[d];
            n /= d;
        }
    }
    return res;
}

template <class Fn>
void forEachCartesianProduct(const std::vector<std::vector<int64_t>>& mat, Fn&& yield) {
    std::vector<size_t> indices(mat.size());
    std::vector<int64_t> out;
    for (const auto& row : mat) out.push_back(row[0]);
    const size_t n = ranges::fold_left(mat, size_t{1}, [](size_t m, const auto& row) { return m * row.size(); });
    for (size_t i = 0; i < n; ++i) {
        yield(out);
        for (size_t j = indices.size(); j--;) {
            if (++indices[j] < mat[j].size()) {
                out[j] = mat[j][indices[j]];
                break;
            } else {
                out[j] = mat[j][indices[j] = 0];
            }
        }
    }
}

std::vector<int64_t> getDivisors(int64_t n) {
    const auto primeFactors = getPrimeFactors(n);
    std::vector<std::vector<int64_t>> powers;
    for (auto& [k, v] : primeFactors) {
        std::vector<int64_t> p{1};
        while (p.size() <= v) p.push_back(p.back() * k);
        powers.emplace_back(std::move(p));
    }
    std::vector<int64_t> res;
    forEachCartesianProduct(powers,
                            [&](const auto& p) { res.push_back(ranges::fold_left(p, 1LL, std::multiplies{})); });
    ranges::sort(res);
    return res;
}

template <class ComponentFn>
std::vector<int64_t> getPossibleVels(Input& input, ComponentFn&& get) {
    std::unordered_map<int64_t, std::vector<Hailstone>> mx;
    for (auto& hs : input) mx[get(hs.vel)].push_back(hs);
    std::vector<int64_t> vx;
    bool vxFirst = true;
    for (auto& [k, v] : mx) {
        for (auto [hs1, hsi] : v | views::adjacent<2>) {
            const auto dx = get(hs1.pos) - get(hsi.pos);
            auto divisors = getDivisors(dx);
            std::vector<int64_t> vxi;
            for (auto d : divisors | views::reverse) vxi.push_back(get(hsi.vel) - d);
            for (auto d : divisors) vxi.push_back(get(hsi.vel) + d);
            if (vxFirst) {
                vx = std::move(vxi);
                vxFirst = false;
            } else {
                std::vector<int64_t> vxNew;
                ranges::set_intersection(vx, vxi, std::back_inserter(vxNew));
                vx = std::move(vxNew);
            }
        }
    }
    return vx;
}

int64_t toNearestInteger(double f) {
    return static_cast<int64_t>(std::round(f));
}

int64_t part2(Input input) {
    const auto velX = getPossibleVels(input, [](auto& p) { return p.x; });
    const auto velY = getPossibleVels(input, [](auto& p) { return p.y; });
    const auto velZ = getPossibleVels(input, [](auto& p) { return p.z; });
    for (auto [vx, vy, vz] : views::cartesian_product(velX, velY, velZ)) {
        std::vector<int64_t> times;
        const auto& a = input[0];
        if (!ranges::all_of(input | views::drop(1), [&](const auto& b) {
                /*                           B
                                           /  /
                                        /    /
                A                    /     --
                |\                v      /
                |  \           %        /
                 \   \        ^       --
                  |    \     /      /
                  |      v  /      /
                  \        #     --
                   |      ^    /
                   |     /    /
                   \    /   --
                    |  /  /
                    v / v
                     R
                ->   ->   ->
                AR = A# + #R  --> vAR = vA - vR
                ->   ->   ->
                BR = B% + %R  --> vBR = vB - vR
                */
                const auto p = integerIntersection(a.pos.x, a.pos.y, a.vel.x - vx, a.vel.y - vy, //
                                                   b.pos.x, b.pos.y, b.vel.x - vx, b.vel.y - vy);
                if (!p) return false;
                const auto t1 = static_cast<int64_t>(std::round(p->first));
                const auto t2 = static_cast<int64_t>(std::round(p->second));
                if (t1 <= 0 || t2 <= 0) return false;
                if (times.empty()) times.push_back(t1);
                if (t1 != times[0]) return false; // not the same R
                times.push_back(t2);
                return true;
            }))
            continue;
        if (![](auto v) {
                ranges::sort(v);
                return ranges::unique(v).begin() == end(v);
            }(times))
            continue;
        const auto x = a.pos.x + (a.vel.x - vx) * times[0];
        const auto y = a.pos.y + (a.vel.y - vy) * times[0];
        const auto z = a.pos.z + (a.vel.z - vz) * times[0];
        if (ranges::all_of(views::enumerate(times), [&](auto&& tp) {
                auto&& [i, t] = tp;
                return input[i].pos.z + t * input[i].vel.z == z + t * vz;
            }))
            return x + y + z;
    }
    return 0;
}

std::pair<bool, bool> test() {
    auto testPart1 = [](std::istream& is, int64_t i2, int64_t i3, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part1(input, i2, i3);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 1: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::tuple<std::string_view, int64_t, int64_t, int> part1Cases[] = {{R"(
19, 13, 30 @ -2,  1, -2
18, 19, 22 @ -1, -1, -2
20, 25, 34 @ -2, -2, -4
12, 31, 28 @ -1, -2, -1
20, 19, 15 @  1, -5, -3
)",
                                                                                   7, 27, 2}};
    bool part1Correct = true;
    for (auto [sv, input2, input3, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, input2, input3, correctAnswer);
    }

    auto testPart2 = [](std::istream& is, int64_t correctAnswer) {
        const auto input = parseInput(is);
        const int64_t answer = part2(input);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 2: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::pair<std::string_view, int64_t> part2Cases[] = {{R"(
19, 13, 30 @ -2,  1, -2
18, 19, 22 @ -1, -1, -2
20, 25, 34 @ -2, -2, -4
12, 31, 28 @ -1, -2, -1
20, 19, 15 @  1, -5, -3
)",
                                                                    47}};
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

    auto getTimeColor = [](const auto& elapsed) {
        return elapsed < 100ms ? fmt::color::light_green : elapsed < 1s ? fmt::color::orange : fmt::color::orange_red;
    };
    auto startTime = cron::steady_clock::now();
    const auto part1Ans = part1(input, 200000000000000LL, 400000000000000LL);
    cron::duration<double> elapsed = cron::steady_clock::now() - startTime;
    fmt::print("Part 1: {} in {}\n", fmt::styled(part1Ans, fmt::fg(fmt::color::yellow)),
               fmt::styled(fmt::format("{:.06f}s", elapsed.count()), fmt::fg(getTimeColor(elapsed))));

    if (!test2) return 2;
    startTime = cron::steady_clock::now();
    const auto part2Ans = part2(input);
    elapsed = cron::steady_clock::now() - startTime;
    fmt::print("Part 2: {} in {}\n", fmt::styled(part2Ans, fmt::fg(fmt::color::yellow)),
               fmt::styled(fmt::format("{:.06f}s", elapsed.count()), fmt::fg(getTimeColor(elapsed))));
}
