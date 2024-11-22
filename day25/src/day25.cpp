#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <random>
#include <cmath>
namespace cron = std::chrono;
using namespace std::chrono_literals;
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>
namespace views = ranges::views;

static constexpr std::string_view kInputFilename = "day25.txt";

std::mt19937 prbg{1};

struct Input {
    std::vector<size_t> vertices;
    std::unordered_map<size_t, std::unordered_map<size_t, int>> adj;
    size_t edgeCount{0};
    std::vector<int> vertexGroupSizes;

    Input fastmincut() const {
        if (vertices.size() <= 6) return contract(2);
        const auto t = static_cast<size_t>(std::ceil(1 + vertices.size() / std::sqrt(2)));
        Input g1 = contract(t).fastmincut();
        Input g2 = contract(t).fastmincut();
        return g1.edgeCount < g2.edgeCount ? g1 : g2;
    }

private:
    Input contract(size_t t) const {
        Input res = *this;
        while (res.vertices.size() > t) res.removeRandomEdge();
        return res;
    }
    void removeRandomEdge() {
        // pop random edge
        size_t u{};
        size_t v{};
        auto pop1 = [&] {
            const size_t uid = std::uniform_int_distribution<size_t>{0, vertices.size() - 1}(prbg);
            std::swap(vertices[uid], vertices.back());
            const auto u = vertices.back();
            vertices.pop_back();
            return u;
        };
        for (;;) {
            u = pop1();
            v = pop1();
            if (adj[u].count(v) > 0) break;
            vertices.push_back(u);
            vertices.push_back(v);
        }
        // remove from adj
        const auto adju = std::move(adj[u]);
        const auto adjv = std::move(adj[v]);
        adj.erase(u);
        adj.erase(v);
        // insert uv
        const auto uv = vertexGroupSizes.size();
        const auto uvSize = vertexGroupSizes[u] + vertexGroupSizes[v];
        vertices.push_back(uv);
        vertexGroupSizes.push_back(uvSize);
        for (auto& [k, w] : adju) {
            if (k == v) continue;
            adj[k].erase(u);
            adj[k][uv] += w;
            adj[uv][k] += w;
        }
        for (auto& [k, w] : adjv) {
            if (k == u) continue;
            adj[k].erase(v);
            adj[k][uv] += w;
            adj[uv][k] += w;
        }
        edgeCount -= adju.at(v);
    }
};

Input parseInput(std::istream& in) {
    Input res;
    std::unordered_map<std::string, size_t> m;
    auto getId = [&](const std::string& s) {
        if (auto it = m.find(s); it != end(m)) {
            return it->second;
        } else {
            auto [jt, ok] = m.emplace(s, m.size());
            return jt->second;
        }
    };
    for (std::string line; std::getline(in, line);) {
        std::istringstream iss{line};
        std::string from;
        iss >> from;
        from.pop_back();
        size_t fromId = getId(from);
        for (std::string to; iss >> to;) {
            size_t toId = getId(to);
            res.adj[fromId][toId] = 1;
            res.adj[toId][fromId] = 1;
        }
    }
    res.vertices = res.adj | views::keys | ranges::to<std::vector>();
    res.vertexGroupSizes.resize(res.vertices.size(), 1);
    for (auto& [k, adjk] : res.adj) res.edgeCount += ranges::fold_left(adjk | views::values, 0LL, std::plus{});
    res.edgeCount /= 2;
    fmt::print("|V| = {}, |E| = {}\n", res.vertices.size(), res.edgeCount);
    return res;
}

int part1(const Input& input) {
    for (size_t t = 1; t < 100; ++t) {
        const auto g = input.fastmincut();
        fmt::print("trial {}: |V| = {}, |E| = {}\n", t, g.vertices.size(), g.edgeCount);
        if (g.edgeCount == 3) {
            fmt::print("{} {}\n", g.vertexGroupSizes[g.vertices[0]], g.vertexGroupSizes[g.vertices[1]]);
            return (int)(g.vertexGroupSizes[g.vertices[0]] * g.vertexGroupSizes[g.vertices[1]]);
        }
    }
    return -1;
}

int part2(const Input& input) {
    return static_cast<int>(input.vertices.size());
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
jqt: rhn xhk nvd
rsh: frs pzl lsr
xhk: hfx
cmg: qnr nvd lhk bvb
rhn: xhk bvb hfx
bvb: xhk hfx
pzl: lsr hfx nvd
qnr: nvd
ntq: jqt hfx bvb xhk
nvd: lhk
lsr: lhk
rzs: qnr cmg lsr rsh
frs: qnr lhk lsr
)",
                                                                54}};
    bool part1Correct = true;
    for (auto [sv, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, correctAnswer);
    }
    return {part1Correct, 0};
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
    const auto part1Ans = part1(input);
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
