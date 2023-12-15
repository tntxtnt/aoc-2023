#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <array>
#include <ranges>

static constexpr std::string_view kInputFilename = "day15.txt";

using Input = std::vector<std::string>;

uint8_t HASH(std::string_view sv, uint8_t val = 0) {
    for (char c : sv) val = static_cast<uint8_t>((static_cast<unsigned>(val) + c) * 17);
    return val;
}

Input parseInput(std::istream& in) {
    std::string line;
    std::getline(in, line);
    std::istringstream iss{line};
    Input res;
    for (std::string tok; std::getline(iss, tok, ',');) res.push_back(tok);
    return res;
}

int part1(const Input& input) {
    return std::ranges::fold_left(input, 0, [](int sum, std::string_view sv) { return sum + HASH(sv); });
}

int part2(const Input& input) {
    std::array<std::vector<std::pair<std::string_view, int>>, 256> boxes{};
    for (std::string_view sv : input) {
        char op = sv.back() == '-' ? '-' : '=';
        auto label = sv.substr(0, sv.find(op));
        const auto h = HASH(label);
        const auto it = std::ranges::find_if(boxes[h], [&](auto& p) { return p.first == label; });
        if (op == '-') {
            if (it != end(boxes[h])) boxes[h].erase(it);
        } else {
            const int value = std::stoi(sv.substr(sv.find(op) + 1).data());
            if (it != end(boxes[h])) {
                it->second = value;
            } else {
                boxes[h].emplace_back(label, value);
            }
        }
    }
    int res{};
    for (int i : std::views::iota(0, 256)) {
        if (boxes[i].empty()) continue;
        for (int slot = 1; auto& p : boxes[i]) {
            res += (i + 1) * slot * p.second;
            ++slot;
        }
    }
    return res;
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 1320;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int part2CorrectAnswer = 145;
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
