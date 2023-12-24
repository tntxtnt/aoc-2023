#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <cstdint>
#include <chrono>
namespace cron = std::chrono;
using namespace std::chrono_literals;
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day7.txt";

struct Card {
    char face;
    int8_t value;
    explicit Card(char ch) : face{ch} {
        static constexpr std::string_view kCardFaces = "Z23456789TJQKA";
        value = static_cast<uint8_t>(kCardFaces.find(ch));
    }
    bool operator<(const Card& rhs) const { return value < rhs.value; }
};

struct Hand {
    std::string rep;
    int bidValue;
    std::vector<std::pair<int, Card>> cardCount;
    Hand(std::string rep, int bidValue) : rep{std::move(rep)}, bidValue{bidValue} {
        std::unordered_map<char, int> freq;
        for (char ch : this->rep) ++freq[ch];
        for (auto& [k, v] : freq) cardCount.emplace_back(v, Card{k});
        ranges::sort(cardCount, std::less{});
        ranges::reverse(cardCount);
    }
    bool operator<(const Hand& rhs) const {
        auto getCount = [](const std::pair<int, Card>& p) { return p.first; };
        const bool cmpLRCnts =
            ranges::lexicographical_compare(cardCount, rhs.cardCount, std::less{}, getCount, getCount);
        const bool cmpRLCnts =
            ranges::lexicographical_compare(rhs.cardCount, cardCount, std::less{}, getCount, getCount);
        if (!cmpLRCnts && !cmpRLCnts) { // equal counts
            auto getFaceValue = [](char ch) { return Card{ch}.value; };
            return ranges::lexicographical_compare(rep, rhs.rep, std::less{}, getFaceValue, getFaceValue);
        }
        return cmpLRCnts;
    }
    void convertToJokerHand() {
        const auto it = ranges::find_if(
            cardCount, [](char face) { return face == 'J'; }, [](const auto& p) { return p.second.face; });
        if (it == end(cardCount)) return;
        const int jokerCnt = it->first;
        if (jokerCnt < 5) {
            cardCount.erase(it);
            cardCount[0].first += jokerCnt;
        }
        for (char& ch : rep)
            if (ch == 'J') ch = 'Z';
    }
};

using Input = std::vector<Hand>;

Input parseInput(std::istream& in) {
    Input res;
    std::string hand;
    int bidValue;
    while (in >> hand >> bidValue) res.emplace_back(hand, bidValue);
    return res;
}

int part1(Input input) {
    ranges::sort(input, std::less{});
    return ranges::fold_left(
        input, 0, [multiplier = 1](int sum, const Hand& hand) mutable { return sum + multiplier++ * hand.bidValue; });
}

int part2(Input input) {
    for (auto& hand : input) hand.convertToJokerHand();
    return part1(std::move(input));
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 6440;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int part2CorrectAnswer = 5905;
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
