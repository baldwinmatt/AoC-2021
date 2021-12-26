#include "aoc21/helpers.h"
#include <thread>
#include <vector>
#include <queue>

namespace {
  using Row = std::vector<char>;
  using Grid = std::vector<Row>;

  const auto canMoveEast = [](const Grid& g, size_t x, size_t y) {
    assert(y < g.size());
    const auto& row = g[y];
    assert(x < row.size());
    const size_t next_x = (x + 1) % row.size();

    return (row[x] == '>' && row[next_x] == '.');
  };

  const auto canMoveSouth = [](const Grid& g, size_t x, size_t y) {
    assert(y < g.size());
    const auto& row = g[y];
    assert(x < row.size());
    const size_t next_y = (y + 1) % g.size();
    const auto& next_row = g[next_y];

    return (row[x] == 'v' && next_row[x] == '.');
  };

  const auto moveEast = [](Grid& g, size_t x, size_t y) {
    assert(canMoveEast(g, x, y));

    auto& row = g[y];
    const size_t next_x = (x + 1) % row.size();
    row[next_x] = row[x];
    row[x] = '.';
  };
  const auto moveSouth = [](Grid& g, size_t x, size_t y) {
    assert(canMoveSouth(g, x, y));

    auto& row = g[y];
    const size_t next_y = (y + 1) % g.size();
    auto& next_row = g[next_y];
    next_row[x] = row[x];
    row[x] = '.';
  };

  std::ostream& operator<<(std::ostream& os, const Grid& g) {
    for (size_t y = 0; y < g.size(); y++) {
      const auto& row = g[y];
      for (size_t x = 0; x < row.size(); x++) {
        os << row[x];
      }
      os << std::endl;
    }

    return os;
  }
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  const bool display = argc > 2 && argv[2][0] == '1';

  std::string line;
  Grid grid;
  size_t width = 0;
  while (aoc::getline(f, line)) {
    grid.emplace_back();
    auto& r = grid.back();
    for (const auto& c: line) {
      assert(c == '.' || c == '>' || c == 'v');
      r.push_back(c);
    }
    if (width == 0) {
      width = line.size();
    }

    assert(r.size() == width);
  }
  f.close();

  const size_t height = grid.size();

  size_t part1 = 0;
  bool didMove = false;
  if (display) { std::cout << aoc::cls; };
  do {
    std::queue<std::pair<size_t, size_t>> moves;
    for (size_t x = 0; x < width; x++) {
      for (size_t y = 0; y < height; y++) {
        if (canMoveEast(grid, x, y)) {
          moves.emplace(std::make_pair(x, y));
        }
      }
    }
    didMove = !moves.empty();
    while (!moves.empty()) {
      moveEast(grid, moves.front().first, moves.front().second);
      moves.pop();
    }
    for (size_t x = 0; x < width; x++) {
      for (size_t y = 0; y < height; y++) {
        if (canMoveSouth(grid, x, y)) {
          moves.emplace(std::make_pair(x, y));
        }
      }
    }
    didMove |= !moves.empty();
    while (!moves.empty()) {
      moveSouth(grid, moves.front().first, moves.front().second);
      moves.pop();
    }
    if (display) {
      std::this_thread::sleep_for (std::chrono::milliseconds(10));
      std::cout << aoc::cls;
      std::cout << grid << std::endl;
    }

    part1++;
  } while (didMove);

  aoc::print_result(1, part1);

  return 0;
}

