#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include <ranges>
#include <chrono>
namespace cron = std::chrono;
using namespace std::chrono_literals;
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day19.txt";

struct PartRatings {
    int x{};
    int m{};
    int a{};
    int s{};
    explicit PartRatings(std::string_view sv) {
        x = std::stoi(sv.substr(sv.find("x=") + 2).data());
        m = std::stoi(sv.substr(sv.find("m=") + 2).data());
        a = std::stoi(sv.substr(sv.find("a=") + 2).data());
        s = std::stoi(sv.substr(sv.find("s=") + 2).data());
    }
    int get(std::string_view label) const { return label == "x" ? x : label == "m" ? m : label == "a" ? a : s; }
};
struct PartRatingCompare {
    std::string label;
    bool lt;
    int value;
    std::string next;
    explicit PartRatingCompare(std::string_view sv) {
        label = sv.substr(0, 1);
        lt = sv[1] == '<';
        value = std::stoi(sv.substr(2, sv.find(":") - 2).data());
        next = sv.substr(sv.find(":") + 1);
    }
    bool operator()(const PartRatings& part) const { return lt ? part.get(label) < value : part.get(label) > value; }
};
struct Workflow {
    std::string name;
    std::vector<PartRatingCompare> steps;
    std::string last;
    Workflow() = default;
    explicit Workflow(std::string_view sv) {
        name = sv.substr(0, sv.find("{"));
        sv = sv.substr(sv.find("{") + 1);
        while (sv.find(",") != sv.npos) {
            std::string_view s = sv.substr(0, sv.find(","));
            steps.emplace_back(PartRatingCompare{s});
            sv = sv.substr(sv.find(",") + 1);
        }
        last = sv.substr(0, sv.size() - 1);
    }
    std::string operator()(const PartRatings& part) const {
        for (auto& step : steps)
            if (step(part)) return step.next;
        return last;
    }
};
using Input = std::pair<std::unordered_map<std::string, Workflow>, std::vector<PartRatings>>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) {
        Workflow workflow{line};
        res.first[workflow.name] = workflow;
        if (line.empty()) break;
    }
    for (std::string line; std::getline(in, line);) res.second.emplace_back(PartRatings{line});
    return res;
}

int part1(const Input& input) {
    auto& [workflows, ratings] = input;
    return ranges::fold_left(
        ratings //
            | views::filter([&](auto& rating) {
                  std::string s = "in";
                  while (s != "A" && s != "R") {
                      const auto it = workflows.find(s);
                      if (it == end(workflows)) return false;
                      s = it->second(rating);
                  }
                  return s == "A";
              }) //
            | views::transform([](auto& rating) { return rating.x + rating.m + rating.a + rating.s; }),
        0, std::plus{});
}

struct RangeRatings {
    struct Range {
        int a{1};
        int b{4000};
        int len() const { return b - a + 1; }
        bool valid() const { return a <= b; }
    };
    Range x;
    Range m;
    Range a;
    Range s;
    Range& get(std::string_view label) { return label == "x" ? x : label == "m" ? m : label == "a" ? a : s; }
    int64_t value() const { return (int64_t)x.len() * m.len() * a.len() * s.len(); }
};

int64_t part2(const Input& input) {
    int64_t res{};
    auto& [workflows, ratings] = input;
    std::stack<std::pair<std::string, RangeRatings>> st;
    st.emplace("in", RangeRatings{});
    while (!st.empty()) {
        auto [wfname, currRating] = st.top();
        st.pop();
        if (wfname == "R") continue;
        if (wfname == "A") {
            res += currRating.value();
            continue;
        }
        const auto it = workflows.find(wfname);
        if (it == end(workflows)) continue;
        bool valid = true;
        for (auto& step : it->second.steps) {
            auto& r = currRating.get(step.label);
            auto nextRating = currRating;
            auto& rn = nextRating.get(step.label);
            if (step.lt) {
                rn.b = std::min(rn.b, step.value - 1);
                if (rn.valid()) st.emplace(step.next, nextRating);
                r.a = std::max(r.a, step.value);
            } else {
                rn.a = std::max(rn.a, step.value + 1);
                if (rn.valid()) st.emplace(step.next, nextRating);
                r.b = std::min(r.b, step.value);
            }
            if (valid = r.valid(); !valid) break;
        }
        if (valid) st.emplace(it->second.last, currRating);
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
px{a<2006:qkq,m>2090:A,rfg}
pv{a>1716:R,A}
lnx{m>1548:A,A}
rfg{s<537:gd,x>2440:R,A}
qs{s>3448:A,lnx}
qkq{x<1416:A,crn}
crn{x>2662:A,R}
in{s<1351:px,qqz}
qqz{s>2770:qs,m<1801:hdj,R}
gd{a>3333:R,R}
hdj{m>838:A,pv}

{x=787,m=2655,a=1222,s=2876}
{x=1679,m=44,a=2067,s=496}
{x=2036,m=264,a=79,s=2244}
{x=2461,m=1339,a=466,s=291}
{x=2127,m=1623,a=2188,s=1013}
)",
                                                                19114}};
    bool part1Correct = true;
    for (auto [sv, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, correctAnswer);
    }

    auto testPart2 = [](std::istream& is, int64_t correctAnswer) {
        const auto input = parseInput(is);
        const auto answer = part2(input);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 2: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::pair<std::string_view, int64_t> part2Cases[] = {{R"(
px{a<2006:qkq,m>2090:A,rfg}
pv{a>1716:R,A}
lnx{m>1548:A,A}
rfg{s<537:gd,x>2440:R,A}
qs{s>3448:A,lnx}
qkq{x<1416:A,crn}
crn{x>2662:A,R}
in{s<1351:px,qqz}
qqz{s>2770:qs,m<1801:hdj,R}
gd{a>3333:R,R}
hdj{m>838:A,pv}

{x=787,m=2655,a=1222,s=2876}
{x=1679,m=44,a=2067,s=496}
{x=2036,m=264,a=79,s=2244}
{x=2461,m=1339,a=466,s=291}
{x=2127,m=1623,a=2188,s=1013}
)",
                                                                    167409079868000}};
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
