#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <stack>
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day9.txt";

using Input = std::vector<std::vector<int>>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) {
        std::istringstream iss{line};
        std::vector<int> p;
        for (int n; iss >> n;) p.push_back(n);
        res.emplace_back(std::move(p));
    }
    return res;
}

std::stack<std::vector<int>> getExtrapolateStack(const std::vector<int>& p) {
    std::stack<std::vector<int>> res;
    res.push(p);
    for (size_t i = p.size(); i-- > 1;) {
        const auto& q = res.top();
        std::vector<int> qnext;
        std::adjacent_difference(begin(q) + 1, end(q), std::back_inserter(qnext));
        qnext[0] = q[1] - q[0];
        if (ranges::all_of(qnext, [](int n) { return n == 0; })) break;
        res.push(std::move(qnext));
    }
    return res;
}

int part1(const Input& input) {
    return ranges::fold_left(input | views::transform([](const auto& p) {
                                 auto st = getExtrapolateStack(p);
                                 int next{};
                                 for (; !st.empty(); st.pop()) next += st.top().back();
                                 return next;
                             }),
                             0, std::plus{});
}

int part2(const Input& input) {
    return ranges::fold_left(input | views::transform([](const auto& p) {
                                 auto st = getExtrapolateStack(p);
                                 int prev{};
                                 for (; !st.empty(); st.pop()) prev = st.top().front() - prev;
                                 return prev;
                             }),
                             0, std::plus{});
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 114;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int part2CorrectAnswer = 2;
    const int part2Answer = part2(input1);
    const bool part2Correct = part2Answer == part2CorrectAnswer;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer,
               fmt::styled(part2Answer, fmt::fg(part2Correct ? fmt::color::green : fmt::color::red)));

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
