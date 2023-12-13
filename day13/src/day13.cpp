#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <ranges>

static constexpr std::string_view kInputFilename = "day13.txt";

using Matrix = std::vector<std::string>;

using Input = std::vector<Matrix>;

Matrix transpose(const Matrix& mat) {
    Matrix res(mat[0].size(), std::string(mat.size(), ' '));
    for (size_t i = 0; i < mat.size(); ++i)
        for (size_t j = 0; j < mat[0].size(); ++j) res[j][i] = mat[i][j];
    return res;
}

Input parseInput(std::istream& in) {
    Input res;
    Matrix mat;
    for (std::string line; std::getline(in, line);) {
        if (line.empty()) {
            Matrix temp;
            temp.swap(mat);
            res.emplace_back(std::move(temp));
        } else {
            mat.push_back(line);
        }
    }
    res.emplace_back(std::move(mat));
    return res;
}

int getReflectLine(const Matrix& mat) {
    for (size_t i = 1; i < mat.size(); ++i) {
        bool ok = true;
        for (size_t d = 0; 0 < i - d && i + d < mat.size(); ++d) {
            if (mat[i - 1 - d] != mat[i + d]) {
                ok = false;
                break;
            }
        }
        if (ok) return static_cast<int>(i);
    }
    return -1;
}

int part1(const Input& input) {
    return std::ranges::fold_left(input, 0, [](int sum, const Matrix& mat) {
        const int hori = getReflectLine(mat);
        return hori != -1 ? sum + 100 * hori : sum + getReflectLine(transpose(mat));
    });
}

int getSmudgeLine(const Matrix& mat) {
    for (size_t i = 1; i < mat.size(); ++i) {
        int diffCount = 0;
        for (size_t d = 0; 0 < i - d && i + d < mat.size(); ++d) {
            if (mat[i - 1 - d] != mat[i + d])
                for (size_t j = 0; j < mat[i + d].size(); ++j) diffCount += mat[i - 1 - d][j] != mat[i + d][j];
            if (diffCount > 1) break;
        }
        if (diffCount == 1) return static_cast<int>(i);
    }
    return -1;
}

int part2(const Input& input) {
    return std::ranges::fold_left(input, 0, [](int sum, const Matrix& mat) {
        const int hori = getSmudgeLine(mat);
        return hori != -1 ? sum + 100 * hori : sum + getSmudgeLine(transpose(mat));
    });
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.

#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 405;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int part2CorrectAnswer = 400;
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
