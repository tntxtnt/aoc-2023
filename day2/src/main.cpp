#include <fmt/format.h>
#include <fmt/color.h>
#include <sstream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <ranges>
#include <fstream>

struct SetOfCubes {
    int red;
    int green;
    int blue;
    bool operator<(const SetOfCubes& rhs) const { return red <= rhs.red && green <= rhs.green && blue <= rhs.blue; }
};

struct Game {
    std::vector<SetOfCubes> subsets;
};

int part1(const SetOfCubes& input0, const std::vector<Game>& input1) {
    int res{};
    for (int gameId = 0; auto& game : input1) {
        ++gameId;
        if (std::ranges::all_of(game.subsets, [&](auto& subset) { return subset < input0; })) res += gameId;
    }
    return res;
}

int part2(const std::vector<Game>& input) {
    return std::transform_reduce(begin(input), end(input), 0, std::plus{}, [](const Game& game) {
        SetOfCubes minSet{};
        for (auto& subset : game.subsets) {
            minSet.red = std::min(minSet.red, -subset.red);
            minSet.green = std::min(minSet.green, -subset.green);
            minSet.blue = std::min(minSet.blue, -subset.blue);
        }
        return -minSet.red * minSet.green * minSet.blue;
    });
}

std::vector<Game> parseInput(std::istream& in);
bool test();

int main() {
    if (!test()) return 1;
    if (auto in = std::ifstream("day2.txt"); !in) {
        fmt::print("Cannot open 'day2.txt'\n");
        return 2;
    } else {
        const auto input1 = parseInput(in);
        const auto input0 = SetOfCubes(12, 13, 14);
        fmt::print("Part 1: {}\n", fmt::styled(part1(input0, input1), fmt::fg(fmt::color::yellow)));
        fmt::print("Part 2: {}\n", fmt::styled(part2(input1), fmt::fg(fmt::color::yellow)));
    }
}


std::vector<Game> parseInput(std::istream& in) {
    std::vector<Game> res;
    for (std::string ignore; in >> ignore >> ignore;) {
        Game game;
        for (bool playing = true; playing;) {
            SetOfCubes subset{};
            for (int val; in >> val >> ignore;) {
                if (ignore[0] == 'r') {
                    subset.red = val;
                } else if (ignore[0] == 'g') {
                    subset.green = val;
                } else {
                    subset.blue = val;
                }
                if (ignore.back() != ',') {
                    game.subsets.emplace_back(std::move(subset));
                    if (ignore.back() != ';') playing = false;
                    break;
                }
            }
        }
        res.emplace_back(std::move(game));
    }
    return res;
}

bool test() {
    std::istringstream iss1{R"(
Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
)"};
    iss1.ignore();
    const auto input0 = SetOfCubes(12, 13, 14);
    const auto input1 = parseInput(iss1);

    const int part1CorrectAnswer = 8;
    const int part1Answer = part1(input0, input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    const int part2CorrectAnswer = 2286;
    const int part2Answer = part2(input1);
    const bool part2Correct = part2Answer == part2CorrectAnswer;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer,
               fmt::styled(part2Answer, fmt::fg(part2Correct ? fmt::color::green : fmt::color::red)));

    return part1Correct && part2Correct;
}