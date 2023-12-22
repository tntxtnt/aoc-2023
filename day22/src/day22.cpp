#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <ranges>
namespace ranges = std::ranges;
namespace views = std::views;

static constexpr std::string_view kInputFilename = "day22.txt";

struct Point3i {
    int x{};
    int y{};
    int z{};
    Point3i() = default;
    Point3i(int x, int y, int z) : x{x}, y{y}, z{z} {}
    bool operator==(const Point3i& other) const { return x == other.x && y == other.y && z == other.z; }
};

struct Brick {
    int id{};
    Point3i from;
    Point3i to;
    template <class Fn>
    void forEachBlock(Fn&& yield) const {
        for (int x = std::min(from.x, to.x); x <= std::max(from.x, to.x); ++x)
            for (int y = std::min(from.y, to.y); y <= std::max(from.y, to.y); ++y)
                for (int z = std::min(from.z, to.z); z <= std::max(from.z, to.z); ++z) yield(Point3i(x, y, z), id);
    }
    int getMinZ() const { return std::min(from.z, to.z); }
};

size_t hashCombine(size_t seed, int val) {
    return seed ^= std::hash<int>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
template <>
struct hash<Point3i> {
    size_t operator()(const Point3i& p) const { return hashCombine(hashCombine(hashCombine(0, p.x), p.y), p.z); }
};
} // namespace std

using Input = std::vector<Brick>;

Input parseInput(std::istream& in) {
    Input res;
    int brickId{};
    for (std::string line; std::getline(in, line);) {
        std::istringstream iss{line};
        Brick brick;
        brick.id = brickId++;
        char ignore;
        iss >> brick.from.x >> ignore >> brick.from.y >> ignore >> brick.from.z >> ignore //
            >> brick.to.x >> ignore >> brick.to.y >> ignore >> brick.to.z;
        res.push_back(std::move(brick));
    }
    return res;
}

std::pair<std::vector<std::unordered_set<int>>, std::vector<std::unordered_set<int>>> getBrickEqulibrium(Input input) {
    std::unordered_map<Point3i, int> spaces;
    for (auto& brick : input) brick.forEachBlock([&](const Point3i& p, int id) { spaces[p] = id; });
    ranges::sort(input, std::less{}, [](const Brick& brick) { return brick.getMinZ(); });
    std::vector<std::unordered_set<int>> supportBy(input.size());
    std::vector<std::unordered_set<int>> supports(input.size());
    for (auto& brick : input) {
        brick.forEachBlock([&](const Point3i& p, int) { spaces.erase(p); }); // temp removal
        for (std::unordered_set<int> supp; brick.getMinZ() > 1;) {
            --brick.from.z;
            --brick.to.z;
            brick.forEachBlock([&](const Point3i& p, int) {
                if (auto it = spaces.find(p); it != end(spaces)) supp.insert(it->second);
            });
            if (!supp.empty()) {
                for (int id : supp) supports[id].insert(brick.id);
                supportBy[brick.id] = std::move(supp);
                ++brick.from.z;
                ++brick.to.z;
                break;
            }
        }
        brick.forEachBlock([&](const Point3i& p, int id) { spaces[p] = id; }); // restore
    }
    return std::make_pair(supports, supportBy);
}

int part1(const Input& input) {
    const auto [supports, supportBy] = getBrickEqulibrium(input);
    return (int)ranges::count_if(input, [&](auto& brick) {
        return ranges::all_of(supports[brick.id], [&](int id) { return supportBy[id].size() != 1; });
    });
}

int part2(const Input& input) {
    const auto [supports, supportBy] = getBrickEqulibrium(input);
    int res{};
    for (int brickId : views::iota(0, (int)input.size())) {
        std::stack<int> st;
        st.push(brickId);
        auto supportBy2 = supportBy;
        int cnt{-1};
        while (!st.empty()) {
            int id = st.top();
            st.pop();
            ++cnt;
            for (int supportee : supports[id]) {
                supportBy2[supportee].erase(id);
                if (supportBy2[supportee].empty()) st.push(supportee);
            }
        }
        res += cnt;
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
1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9
)",
                                                                5}};
    bool part1Correct = true;
    for (auto [sv, correctAnswer] : part1Cases) {
        std::istringstream iss{sv.data()};
        iss.ignore();
        part1Correct &= testPart1(iss, correctAnswer);
    }

    auto testPart2 = [](std::istream& is, int correctAnswer) {
        const auto input = parseInput(is);
        const int answer = part2(input);
        const bool correct = answer == correctAnswer;
        fmt::print("Part 2: expected {}, got {}\n", correctAnswer,
                   fmt::styled(answer, fmt::fg(correct ? fmt::color::green : fmt::color::red)));
        return correct;
    };
    constexpr std::pair<std::string_view, int> part2Cases[] = {{R"(
1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9
)",
                                                                7}};
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
    fmt::print("Part 1: {}\n", fmt::styled(part1(input), fmt::fg(fmt::color::yellow)));
    if (!test2) return 2;
    fmt::print("Part 2: {}\n", fmt::styled(part2(input), fmt::fg(fmt::color::yellow)));
}
