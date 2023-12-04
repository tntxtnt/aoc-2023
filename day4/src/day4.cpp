#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>

static constexpr std::string_view kInputFilename = "day4.txt";

struct Card {
    std::vector<int> winningNumbers;
    std::vector<int> myNumbers;
};

using Input = std::vector<Card>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) {
        std::istringstream lineStream{line};
        Card card;
        std::string tok;
        lineStream >> tok >> tok;
        for (; lineStream >> tok && tok[0] != '|';) card.winningNumbers.push_back(std::stoi(tok));
        for (int n; lineStream >> n;) card.myNumbers.push_back(n);
        res.emplace_back(std::move(card));
    }
    return res;
}

int part1(const Input& input) {
    return std::transform_reduce(begin(input), end(input), 0, std::plus{}, [](Card card) {
        std::ranges::sort(card.winningNumbers);
        std::ranges::sort(card.myNumbers);
        std::vector<int> intersections;
        std::ranges::set_intersection(card.winningNumbers, card.myNumbers, std::back_inserter(intersections));
        return 1 << (intersections.size() - 1);
    });
}

int part2(const Input& input) {
    std::vector<int> cardCount(input.size(), 1);
    for (size_t cardId = 0; Card card : input) {
        std::ranges::sort(card.winningNumbers);
        std::ranges::sort(card.myNumbers);
        std::vector<int> intersections;
        std::ranges::set_intersection(card.winningNumbers, card.myNumbers, std::back_inserter(intersections));
        for (size_t i = cardId + 1; i <= cardId + intersections.size(); ++i) cardCount[i] += cardCount[cardId];
        ++cardId;
    }
    return std::ranges::fold_left(cardCount, 0, std::plus{});
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 13;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int part2CorrectAnswer = 30;
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
