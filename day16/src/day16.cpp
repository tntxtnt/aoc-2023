#include <fmt/format.h>
#include <fmt/color.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <stack>
#include <ranges>

static constexpr std::string_view kInputFilename = "day16.txt";

using Input = std::vector<std::string>;

Input parseInput(std::istream& in) {
    Input res;
    for (std::string line; std::getline(in, line);) res.push_back(line);
    return res;
}

struct Dir {
    int row;
    int col;
    char rep;
    Dir(int r, int c, char rep) : row{r}, col{c}, rep{rep} {}
    int getBin() const {
        if (rep == 'T') return 1;
        if (rep == 'B') return 2;
        if (rep == 'L') return 4;
        if (rep == 'R') return 8;
        return 0;
    }
    Dir next(int rows, int cols) const {
        int r = row;
        int c = col;
        if (rep == 'T') --r;
        if (rep == 'B') ++r;
        if (rep == 'L') --c;
        if (rep == 'R') ++c;
        if (!(0 <= r && r < rows && 0 <= c && c < cols)) r = c = -1;
        return {r, c, rep};
    }
    Dir bend(char lens, int rows, int cols) const {
        int r = row;
        int c = col;
        char dir = rep;
        if (lens == '/') {
            if (rep == 'T') ++c, dir = 'R';
            if (rep == 'B') --c, dir = 'L';
            if (rep == 'L') ++r, dir = 'B';
            if (rep == 'R') --r, dir = 'T';
        } else {
            if (rep == 'T') --c, dir = 'L';
            if (rep == 'B') ++c, dir = 'R';
            if (rep == 'L') --r, dir = 'T';
            if (rep == 'R') ++r, dir = 'B';
        }
        if (!(0 <= r && r < rows && 0 <= c && c < cols)) r = c = -1;
        return {r, c, dir};
    }
    std::pair<Dir, Dir> split(char lens, int rows, int cols) const {
        int r1 = row;
        int c1 = col;
        char dir1 = rep;
        int r2 = row;
        int c2 = col;
        char dir2 = rep;
        if (lens == '|') {
            if (rep == 'T' || rep == 'B') return {next(cols, rows), {-1, -1, '.'}};
            --r1, dir1 = 'T', ++r2, dir2 = 'B';
        } else {
            if (rep == 'L' || rep == 'R') return {next(cols, rows), {-1, -1, '.'}};
            --c1, dir1 = 'L', ++c2, dir2 = 'R';
        }
        if (!(0 <= r1 && r1 < rows && 0 <= c1 && c1 < cols)) r1 = c1 = -1;
        if (!(0 <= r2 && r2 < rows && 0 <= c2 && c2 < cols)) r2 = c2 = -1;
        return {{r1, c1, dir1}, {r2, c2, dir2}};
    }
};

int part1(const Input& input, int sr = 0, int sc = 0, char sdir = 'R') {
    std::vector<std::vector<uint8_t>> visited(input.size(), std::vector<uint8_t>(input[0].size()));
    std::stack<Dir> st;
    st.emplace(sr, sc, sdir);
    visited[sr][sc] |= st.top().getBin();
    Input inp = input;
    inp[sr][sc] = '#';
    while (!st.empty()) {
        auto curr = st.top();
        st.pop();
        Dir next{-1, -1, '.'};
        Dir next2{-1, -1, '.'};
        if (char lens = input[curr.row][curr.col]; lens == '.') {
            next = curr.next((int)input.size(), (int)input[0].size());
        } else if (lens == '|' || lens == '-') {
            std::tie(next, next2) = curr.split(lens, (int)input.size(), (int)input[0].size());
        } else {
            next = curr.bend(lens, (int)input.size(), (int)input[0].size());
        }
        if (next.row != -1) {
            if ((visited[next.row][next.col] & next.getBin()) == 0) {
                visited[next.row][next.col] |= next.getBin();
                st.push(next);
                inp[next.row][next.col] = '#';
            }
        }
        if (next2.row != -1) {
            if ((visited[next2.row][next2.col] & next2.getBin()) == 0) {
                visited[next2.row][next2.col] |= next2.getBin();
                st.push(next2);
                inp[next2.row][next2.col] = '#';
            }
        }
    }
    int res{};
    for (auto& line : inp) res += (int)std::ranges::count(line, '#');
    return res;
}

int part2(const Input& input) {
    int res{};
    for (int sc = 0; sc < (int)input[0].size(); ++sc) {
        res = std::max(res, part1(input, 0, sc, 'B'));
        res = std::max(res, part1(input, (int)input.size() - 1, sc, 'T'));
    }
    for (int sr = 0; sr < (int)input.size(); ++sr) {
        res = std::max(res, part1(input, sr, 0, 'R'));
        res = std::max(res, part1(input, sr, (int)input[0].size() - 1, 'L'));
    }
    return res;
}

std::pair<bool, bool> test() {
    std::istringstream iss1{R"(
.|...\....
|.-.\.....
.....|-...
........|.
..........
.........\
..../.\\..
.-.-/..|..
.|....-|.\
..//.|....
)"};
    iss1.ignore();
    const auto input1 = parseInput(iss1);

    constexpr int part1CorrectAnswer = 46;
    const int part1Answer = part1(input1);
    const bool part1Correct = part1Answer == part1CorrectAnswer;
    fmt::print("Part 1: expected {}, got {}\n", part1CorrectAnswer,
               fmt::styled(part1Answer, fmt::fg(part1Correct ? fmt::color::green : fmt::color::red)));

    constexpr int part2CorrectAnswer = 51;
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
