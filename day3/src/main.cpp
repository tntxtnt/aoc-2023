#include <fmt/format.h>
#include <fmt/color.h>
#include <sstream>
#include <vector>
#include <cctype>
#include <fstream>
#include <unordered_set>

struct Matrix {
    const std::vector<std::string>& data;
    const int rows;
    const int cols;
    explicit Matrix(const std::vector<std::string>& matrix)
    : data{matrix}, rows{static_cast<int>(data.size())}, cols{static_cast<int>(data[0].size())} {}
    int64_t cid(int i, int j) const { return i * cols + j; }
    template <class Func>
    void forEachCell(Func&& f) const {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) f(i, j, data[i][j]);
    }
    template <class Func>
    void forEachNeighborCell(int i, int j, Func&& f) const {
        for (int di : {-1, 0, 1})
            for (int dj : {-1, 0, 1})
                if (isValidIj(i + di, j + dj)) f(i + di, j + dj, data[i + di][j + dj]);
    }
    bool isValidIj(int i, int j) const { return 0 <= i && i < rows && 0 <= j && j < cols; }
    char operator()(int i, int j) const { return data[i][j]; }
};

int part1(const std::vector<std::string>& input) {
    int res{};
    Matrix mat{input};
    std::unordered_set<int64_t> adjacentToSymbols;
    mat.forEachCell([&](int i, int j, char cellValue) {
        if (cellValue == '.' || isdigit(cellValue)) return;
        mat.forEachNeighborCell(i, j, [&](int ni, int nj, int) { adjacentToSymbols.emplace(mat.cid(ni, nj)); });
    });
    mat.forEachCell([&](int i, int& j, char cellValue) { // notice int& j, j will be modified
        if (!isdigit(cellValue)) return;
        int val = 0;
        bool isAdjacentToSymbol = false;
        for (; j < mat.cols && isdigit(mat(i, j)); ++j) {
            val = 10 * val + mat(i, j) - '0';
            if (!isAdjacentToSymbol && adjacentToSymbols.count(mat.cid(i, j)) > 0) isAdjacentToSymbol = true;
        }
        if (isAdjacentToSymbol) res += val;
    });
    return res;
}

int part2(const std::vector<std::string>& input) {
    int res{};
    Matrix mat{input};
    mat.forEachCell([&](int i, int j, char cellValue) {
        if (cellValue != '*') return;
        std::unordered_set<int64_t> adjacentCount;
        int gearValue = 1;
        mat.forEachNeighborCell(i, j, [&](int ni, int nj, int nval) {
            if (!isdigit(nval)) return;
            int k = nj;
            for (; k >= 0 && isdigit(mat(ni, k));) --k;
            const auto kval = mat.cid(ni, ++k);
            if (adjacentCount.count(kval) > 0) return;
            adjacentCount.emplace(kval);
            if (adjacentCount.size() > 2) return; // should be break
            int val = 0;
            for (; k < mat.cols && isdigit(mat(ni, k)); ++k) val = 10 * val + mat(ni, k) - '0';
            gearValue *= val;
        });
        if (adjacentCount.size() == 2) res += gearValue;
    });
    return res;
}

std::vector<std::string> parseInput(std::istream& in);
bool test();

int main() {
    if (!test()) return 1;
    if (auto in = std::ifstream("day3.txt"); !in) {
        fmt::print("Cannot open 'day3.txt'\n");
        return 2;
    } else {
        const auto input = parseInput(in);
        fmt::print("Part 1: {}\n", fmt::styled(part1(input), fmt::fg(fmt::color::yellow)));
        fmt::print("Part 2: {}\n", fmt::styled(part2(input), fmt::fg(fmt::color::yellow)));
    }
}


std::vector<std::string> parseInput(std::istream& in) {
    std::vector<std::string> res;
    for (std::string line; in >> line;) res.push_back(line);
    return res;
}

bool test() {
    std::istringstream iss1{R"(
467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
)"};
    iss1.ignore();
    auto input = parseInput(iss1);

    const int part1CorrectAnswer = 4361;
    const int part1Answer = part1(input);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    const int part2CorrectAnswer = 467835;
    const int part2Answer = part2(input);
    const bool part2Correct = part2Answer == part2CorrectAnswer;
    fmt::print("Part 2: expected {}, got {}\n", part2CorrectAnswer,
               fmt::styled(part2Answer, fmt::fg(part2Correct ? fmt::color::green : fmt::color::red)));

    return part1Correct && part2Correct;
}