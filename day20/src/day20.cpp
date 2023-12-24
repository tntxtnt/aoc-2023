#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <ranges>
#include <chrono>
namespace cron = std::chrono;
using namespace std::chrono_literals;
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day20.txt";

enum class Pulse { None, Low, High };
struct Module {
    virtual Pulse receivePulse(const std::string&, Pulse pulse) { return pulse; }
    virtual void addInput(const std::string&) {}
    virtual void reset() {}
    virtual ~Module() {}
};
struct Broadcaster : Module {};
struct FlipFlop : Module {
    bool on = false;
    Pulse receivePulse(const std::string&, Pulse pulse) override {
        if (pulse == Pulse::None) return Pulse::None;
        if (pulse == Pulse::High) return Pulse::None;
        on = !on;
        return on ? Pulse::High : Pulse::Low;
    }
    void reset() override { on = false; }
};
struct Conjunction : Module {
    std::unordered_map<std::string, Pulse> memo;
    void addInput(const std::string& inputName) override { memo[inputName] = Pulse::Low; }
    Pulse receivePulse(const std::string& inputName, Pulse pulse) override {
        memo[inputName] = pulse;
        if (ranges::all_of(memo, [](auto& kv) { return kv.second == Pulse::High; })) return Pulse::Low;
        return Pulse::High;
    }
    void reset() override {
        for (auto& [k, v] : memo) v = Pulse::Low;
    }
};

using Input = std::pair<std::unordered_map<std::string, std::unique_ptr<Module>>,
                        std::unordered_map<std::string, std::vector<std::string>>>;

Input parseInput(std::istream& in) {
    std::vector<std::string> lines;
    std::unordered_map<std::string, std::unique_ptr<Module>> modules;
    for (std::string line; std::getline(in, line);) {
        const std::string moduleRep = line.substr(0, line.find(' '));
        if (moduleRep == "broadcaster") {
            modules["broadcaster"] = std::make_unique<Broadcaster>();
        } else {
            const std::string moduleName = moduleRep.substr(1);
            if (moduleRep[0] == '%') {
                modules[moduleName] = std::make_unique<FlipFlop>();
            } else {
                modules[moduleName] = std::make_unique<Conjunction>();
            }
        }
        lines.push_back(line);
    }
    std::unordered_map<std::string, std::vector<std::string>> adj;
    for (auto& line : lines) {
        const std::string fromRep = line.substr(0, line.find(' '));
        const std::string from = fromRep[0] == 'b' ? fromRep : fromRep.substr(1);
        std::vector<std::string> to;
        std::istringstream iss{line.substr(line.find('>') + 1)};
        for (std::string tok; iss >> tok;) {
            if (tok.back() == ',') tok.pop_back();
            to.push_back(tok);
            if (auto it = modules.find(tok); it != end(modules)) it->second->addInput(from);
        }
        adj[from] = std::move(to);
    }
    return std::make_pair(std::move(modules), adj);
}

int part1(const Input& input) {
    auto& [modules, adj] = input;
    int hiCnt{};
    int loCnt{};
    for (int t = 0; t < 1000; ++t) {
        std::queue<std::tuple<std::string, Pulse, std::string, int>> q;
        q.emplace("broadcaster", Pulse::Low, "button", 0);
        while (!q.empty()) {
            auto [name, pulse, fromName, timestamp] = q.front();
            q.pop();
            if (pulse == Pulse::Low)
                ++loCnt;
            else if (pulse == Pulse::High)
                ++hiCnt;
            if (auto it = modules.find(name); it != end(modules)) {
                const auto pulseOut = it->second->receivePulse(fromName, pulse);
                if (pulseOut == Pulse::None) continue;
                if (auto adjIt = adj.find(name); adjIt != end(adj))
                    for (auto& nb : adjIt->second) q.emplace(nb, pulseOut, name, timestamp + 1);
            }
        }
    }
    return loCnt * hiCnt;
}

int64_t part2(const Input& input) {
    auto& [modules, adj] = input;
    for (auto& [k, v] : modules) v->reset();
    std::string conjName =
        ranges::find_if(adj, [](auto& kv) { return ranges::find(kv.second, "rx") != end(kv.second); })->first;
    std::unordered_map<std::string, std::vector<int>> cycles;
    if (auto* p = dynamic_cast<Conjunction*>(modules.find(conjName)->second.get()); p != nullptr)
        for (auto k : p->memo | views::keys) cycles[k];
    for (int t = 1; t < 1'000'000; ++t) {
        if (ranges::all_of(cycles, [](auto& kv) -> bool { return kv.second.size() >= 2; })) break;
        std::queue<std::tuple<std::string, Pulse, std::string, int>> q;
        q.emplace("broadcaster", Pulse::Low, "button", 0);
        while (!q.empty()) {
            auto [name, pulse, fromName, timestamp] = q.front();
            q.pop();
            if (auto it = cycles.find(fromName); it != end(cycles))
                if (pulse == Pulse::High) it->second.push_back(t);
            if (auto it = modules.find(name); it != end(modules)) {
                const auto pulseOut = it->second->receivePulse(fromName, pulse);
                if (pulseOut == Pulse::None) continue;
                if (auto adjIt = adj.find(name); adjIt != end(adj))
                    for (auto& nb : adjIt->second) q.emplace(nb, pulseOut, name, timestamp + 1);
            }
        }
    }
    int64_t res{1};
    for (auto& [k, v] : cycles) {
        if (v[1] - v[0] != v[0]) {
            fmt::print("Anomaly at {} {} {}\n", k, v[0], v[1]);
            return -1;
        }
        res = std::lcm(res, v[1] - v[0]);
    }
    return res;
}

std::pair<bool, bool> test() {
    auto testPart1 = [](std::istream& is, int64_t correctAnswer) {
        const auto input = parseInput(is);
        const auto answer = part1(input);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 1: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::pair<std::string_view, int> part1Cases[] = {{R"(
broadcaster -> a, b, c
%a -> b
%b -> c
%c -> inv
&inv -> a
)",
                                                                32000000},
                                                               {R"(
broadcaster -> a
%a -> inv, con
&inv -> b
%b -> con
&con -> output
)",
                                                                11687500}};
    bool part1Correct = true;
    for (auto [sv, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, correctAnswer);
    }

    bool part2Correct = true;
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
