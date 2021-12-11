#include "aoc21/helpers.h"
#include <vector>
#include <deque>
#include <set>

namespace {
  using Row = std::vector<int>;
  using Grid = std::vector<Row>;
  using Point = std::pair<size_t, size_t>;
  using FlashedQueue = std::deque<Point>;
  using FlashedSet = std::set<Point>;

  constexpr int DEAD_OCT = -1;

  const FlashedSet Neighbors{
    { -1, -1 }, 
    { -1, 0 }, 
    { -1, 1 }, 
    { 0, -1 }, 
    { 0, 1 }, 
    { 1, -1 }, 
    { 1, 0 }, 
    { 1, 1 }, 
  };

  const auto step = [](Grid& grid) {
    FlashedQueue q;
    FlashedSet s;

    for (size_t y = 0; y < grid.size(); y++) {
      for (size_t x = 0; x < grid[0].size(); x++) {
        if (grid[y][x] == DEAD_OCT) {
          continue; // Padding point
        }

        assert( y > 0 && y < grid.size() - 1 );
        assert( x > 0 && x < grid[y].size() - 1 );

        grid[y][x]++;

        if (grid[y][x] > 9) {
          q.push_back(Point(y, x));
          s.insert(Point(y, x));
          grid[y][x] = 0;
        }
      }
    }

    while (!q.empty()) {
      auto fp = q.front(); q.pop_front();

      for (const auto &n : Neighbors) {
        size_t y = fp.first + n.first;
        size_t x =  fp.second + n.second;

        if (grid[y][x] == DEAD_OCT) {
          continue; // Padding point
        }
        else if (grid[y][x] == 0) {
          continue; // Already flashed this round
        }

        assert( y > 0 && y < grid.size() - 1 );
        assert( x > 0 && x < grid[y].size() - 1 );

        grid[y][x]++;

        if (grid[y][x] > 9) {
          q.push_back(Point(y, x));
          s.insert(Point(y, x));
          grid[y][x] = 0;
        }
      }
    }

    return s.size();
  };

  const auto DisplayGrid = [](const Grid& g) {
    for (const auto& r : g) {
      for (const auto c : r) {
        if (c == DEAD_OCT) {
          continue;
        }
        if (c == 0) {
          std::cout << aoc::bold_on << c << aoc::bold_off << " ";
        } else {
          std::cout << c << " ";
        }
      }
      std::cout << std::endl;
    }
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  Grid grid;
  std::string line;
  std::size_t total_oct = 0;
  while (aoc::getline(f, line)) {
    Row r;
    r.push_back(DEAD_OCT);
    for (const char c : line) {
      if (!(c >= '0' && c <= '9')) {
        throw std::runtime_error("Invalid input");
      }
      r.push_back(c - '0');
      total_oct++;
    }

    r.push_back(DEAD_OCT);
    assert(grid.empty() || r.size() == grid[0].size());
    if (grid.empty()) {
      Row pad(r.size(), DEAD_OCT);
      grid.push_back(pad);
    }
    grid.push_back(r);
  };

  Row pad(grid[0].size(), DEAD_OCT);
  grid.push_back(pad);

  // 100 steps
  DEBUG(std::cout << "Total octupus: " << total_oct << std::endl);

  size_t flash_count = 0;
  size_t first_syncd = SIZE_T_MAX;
  for (size_t i = 0; i < 100 || first_syncd == SIZE_T_MAX; i++) {
    const auto flashed = step(grid);
    if (flashed == total_oct) {
      DisplayGrid(grid);
      first_syncd = std::min(first_syncd, i + 1);
    }
    if (i < 100) {
      flash_count += flashed;
    }
  }

  std::cout << "Part 1: " << flash_count << std::endl;
  std::cout << "Part 2: " << first_syncd << std::endl;

  return 0;
}

