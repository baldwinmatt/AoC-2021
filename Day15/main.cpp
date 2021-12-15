#include "aoc21/helpers.h"

#include <vector>
#include <map>

namespace {
  class Grid {
  private:
    std::vector<int> _g;
    size_t _width;

  public:
    Grid() 
      : _width(SIZE_T_MAX)
    { }

    void set_width(size_t w) {
      assert(!is_width_set());
      _width = w;
    }

    size_t get_width() const {
      assert(is_width_set());
      return _width;
    }

    size_t get_height() const {
      assert(is_width_set());
      return _g.size() / _width;
    }

    bool is_width_set() const {
      return _width != SIZE_T_MAX;
    }

    void push_back(int v) {
      _g.push_back(v);
    }

    int get(size_t x, size_t y) const {
      x = x % get_width();
      y = y % get_height();
      assert((x + y * _width) < _g.size());

      return _g[x + y * _width];
    }
  };

  using Point = std::pair<size_t, size_t>;
  using CostMap = std::map<Point, int64_t>;
  using VisitMap = std::multimap<size_t, Point>;

  const std::vector<Point>Directions{
    {  0,  1 },
    {  0, -1 },
    {  1,  0 },
    { -1,  0 },
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  Grid grid;
  std::string line;
  while (aoc::getline(f, line)) {
    if (!grid.is_width_set()) {
      grid.set_width(line.size());
    }

    assert(line.size() == grid.get_width());
    for (const auto& c: line) {
      assert(c >= '0' && c <= '9');
      grid.push_back(c - '0');
    }
  }
  f.close();

  constexpr size_t mult[] = { 1, 5 };
  constexpr size_t n_mult = sizeof(mult) / sizeof(mult[0]);

  for (size_t i = 0; i < n_mult; i++) {

    VisitMap to_visit;
    CostMap costs;

    to_visit.emplace(0, std::make_pair<size_t, size_t>(0, 0));

    const size_t total_width = mult[i] * grid.get_width();
    const size_t total_height = mult[i] * grid.get_height();

    while (!to_visit.empty()) {
      auto it = to_visit.begin();
      size_t cost = it->first;
      size_t x = it->second.first;
      size_t y = it->second.second;
      to_visit.erase(it);

      auto r = costs.emplace(std::make_pair(x, y), cost);
      if (!r.second) {
        // not a new node
        continue;
      }

      for (const auto& dx : Directions) {
        const size_t new_x = dx.first + x;
        const size_t new_y = dx.second + y;

        if (new_x < 0 || new_x >= total_width) {
          continue;
        } else if (new_y < 0 || new_y >= total_height) {
          continue;
        }

        const size_t init_cost = grid.get(new_x, new_y);
        const size_t added_cost = (new_x / grid.get_width()) + (new_y / grid.get_height());
        size_t new_cost = init_cost + added_cost;
        if (new_cost > 9) { // wrap back to 1
          new_cost -= 9;
        }

        DEBUG_PRINT("[ " << new_x << ", " << new_y << " ] -> " << (cost + new_cost)); 
        to_visit.emplace(cost + new_cost, std::make_pair(new_x, new_y));
      }
    }

    aoc::print_result(i + 1, costs.at(std::make_pair(total_width - 1, total_height - 1)));
  }

  return 0;
}

