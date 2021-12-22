#include "aoc21/helpers.h"
#include <vector>

namespace {
  using Point = aoc::Point;

  class Cube;
  using CubeList = std::vector<Cube>;

  class Cube {
  public:
    Point range_x;
    Point range_y;
    Point range_z;

    Cube(const std::string& line) {
      std::vector<int> vals;
      int val = INT_MIN;
      bool neg = false;
      vals.reserve(6);
      for (const auto& c : line) {
        if (c != '-' && (c < '0' || c > '9')) {
          if (val != INT_MIN) {
            if (neg) { val = -val; }
            vals.push_back(val);
            val = INT_MIN;
            neg = false;
          }
          continue;
        }
        if (c == '-') {
          neg = true;
        } else {
          if (val == INT_MIN) { val = 0; }
          val *= 10;
          val += c - '0';
        }
      }
      if (val != INT_MIN) {
        if (neg) { val = -val; }
        vals.push_back(val);
        val = 0;
        neg = false;
      }

      assert(vals.size() == 6);
      range_z.second = vals.back(); vals.pop_back();
      range_z.first = vals.back(); vals.pop_back();
      range_y.second = vals.back(); vals.pop_back();
      range_y.first = vals.back(); vals.pop_back();
      range_x.second = vals.back(); vals.pop_back();
      range_x.first = vals.back(); vals.pop_back();
    }

    Cube(const Point& x, const Point& y, const Point& z)
      : range_x(x)
      , range_y(y)
      , range_z(z)
    { }

    size_t volume() const {
      size_t width = range_x.second - range_x.first + 1;
      size_t height = range_y.second - range_y.first + 1;
      size_t depth = range_z.second - range_z.first + 1;

      return width * height * depth;
    }

    size_t volume(const Point& bounds_x, const Point& bounds_y, const Point& bounds_z) const {
      if (range_x.first <= bounds_x.first || range_x.second >= bounds_x.second ||
          range_y.first <= bounds_y.first || range_y.second >= bounds_y.second ||
          range_z.first <= bounds_z.first || range_z.second >= bounds_z.second) {
        return 0;
      }
      return volume();
    }

    bool intersects(const Cube& o) const {
      if (range_x.second < o.range_x.first || o.range_x.second < range_x.first ||
          range_y.second < o.range_y.first || o.range_y.second < range_y.first ||
          range_z.second < o.range_z.first || o.range_z.second < range_z.first)
      {
        return false;
      }
      return true;
    }

    void split_into(const Cube& rhs, CubeList& out) const {
      const auto span_points = [](const Point& a, const Point&b) {
        return Point(std::max(a.first, b.first), std::min(a.second, b.second));
      };

      // Exclude the intersecting region
      Cube exclude(span_points(range_x, rhs.range_x),
        span_points(range_y, rhs.range_y),
        span_points(range_z, rhs.range_z));

      // now split into new cubes
      out.reserve(out.size() + 6);
      if (range_z.first < exclude.range_z.first) {
        out.emplace_back(range_x, range_y, std::make_pair(range_z.first, exclude.range_z.first - 1));
      }
      if (exclude.range_z.second <range_z.second) {
        out.emplace_back(range_x, range_y, std::make_pair(exclude.range_z.second + 1, range_z.second));
      }
      if (range_x.first < exclude.range_x.first) {
        out.emplace_back(std::make_pair(range_x.first, exclude.range_x.first - 1), range_y, exclude.range_z);
      }
      if (exclude.range_x.second < range_x.second) {
        out.emplace_back(std::make_pair(exclude.range_x.second + 1, range_x.second), range_y, exclude.range_z);
      }
      if (range_y.first < exclude.range_y.first) {
        out.emplace_back(exclude.range_x, std::make_pair(range_y.first, exclude.range_y.first - 1), exclude.range_z);
      }
      if (exclude.range_y.second < range_y.second) {
        out.emplace_back(exclude.range_x, std::make_pair(exclude.range_y.second + 1, range_y.second), exclude.range_z);
      }
    }

    friend std::ostream& operator<<(std::ostream& os, const Cube& c) {
      os << "{ " << c.range_x.first << ".." << c.range_x.second << ", " <<
         c.range_y.first << ".." << c.range_y.second << ", " <<
          c.range_z.first << ".." << c.range_z.second << " }";
      return os;
    }
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);
  std::string line;
  CubeList cubes;
  while (aoc::getline(f, line)) {
    Cube c(line);

    CubeList new_cubes;
    for (const auto& i : cubes) {
      if (i.intersects(c)) { // split existing cube if it intersects
        i.split_into(c, new_cubes);
      } else { // else leave untouched
        new_cubes.emplace_back(i);
      }
    }
    // If this line starts with on, then add this cube
    if (line[1] == 'n') {
      DEBUG_PRINT(c);
      new_cubes.emplace_back(c);
    }

    std::swap(cubes,new_cubes);
  }
  f.close();

  size_t part1 = 0;
  size_t part2 = 0;
  Cube part_1_bounds("x=-50..50,y=-50..50,z=-50..50");
  for (const auto& c : cubes) {
    part1 += c.volume(part_1_bounds.range_x, part_1_bounds.range_y, part_1_bounds.range_z);
    part2 += c.volume();
  }

  aoc::print_results(part1, part2);

  return 0;
}

