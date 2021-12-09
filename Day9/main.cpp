#include "aoc21/helpers.h"
#include <vector>

/*
--- Day 9: Smoke Basin ---
These caves seem to be lava tubes. Parts are even still volcanically active; small hydrothermal vents release smoke into the caves that slowly settles like rain.

If you can model how the smoke flows through the caves, you might be able to avoid it and be that much safer. The submarine generates a heightmap of the floor of the nearby caves for you (your puzzle input).

Smoke flows to the lowest point of the area it's in. For example, consider the following heightmap:

2199943210
3987894921
9856789892
8767896789
9899965678
Each number corresponds to the height of a particular location, where 9 is the highest and 0 is the lowest a location can be.

Your first goal is to find the low points - the locations that are lower than any of its adjacent locations. Most locations have four adjacent locations (up, down, left, and right); locations on the edge or corner of the map have three or two adjacent locations, respectively. (Diagonal locations do not count as adjacent.)

In the above example, there are four low points, all highlighted: two are in the first row (a 1 and a 0), one is in the third row (a 5), and one is in the bottom row (also a 5). All other locations on the heightmap have some lower adjacent location, and so are not low points.

The risk level of a low point is 1 plus its height. In the above example, the risk levels of the low points are 2, 1, 6, and 6. The sum of the risk levels of all low points in the heightmap is therefore 15.

Find all of the low points on your heightmap. What is the sum of the risk levels of all low points on your heightmap?
*/

namespace {
  using Row = std::vector<int>;
  using HeightMap = std::vector<Row>;

  const auto ParseRow = [](const auto& line) {
    Row r;

    // We know how big the row will be
    r.reserve(line.size());

    for (const auto c : line) {
      switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          r.push_back(c - '0');
          break;
        default:
          break;
      }
    }

    return r;
  };

  const auto IsLowPoint = [](const HeightMap& map, size_t x, size_t y) {
    const size_t max_y = map.size() - 1;
    const size_t max_x = map[0].size() - 1;
    const auto height = map[y][x];

    if (x == 0) {
      if (y == 0) {
        return height < map[y][x + 1] && height < map[y + 1][x];
      } else if (y == max_y) {
        return height < map[y][x + 1] && height < map[y - 1][x];
      }
      return height < map[y - 1][x] && height < map[y][x + 1] && height < map[y + 1][x];
    } else if (x == max_x) {
      if (y == 0) {
        return height < map[y][x - 1] && height < map[y + 1][x];
      } else if (y == max_y) {
        return height < map[y][x - 1] && height < map[y - 1][x];
      }
      return height < map[y][x - 1] && height < map[y - 1][x] && height < map[y + 1][x];
    } else if (y == 0) {
      // We already checked x == 0 and x == max_x
      return height < map[y][x - 1] && height < map[y + 1][x] && height < map[y][x + 1];
    } else if (y == max_y) {
      // We already checked x == 0 and x == max_x
      return height < map[y][x - 1] && height < map[y - 1][x] && height < map[y][x + 1];
    }
    return height < map[y][x - 1] && height < map[y][x + 1] &&
           height < map[y - 1][x] && height < map[y + 1][x];
  };
};

int main(int argc, char** argv) {

  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  HeightMap map;
  std::string line;
  while (aoc::getline(f, line)) {
    const auto r = ParseRow(line);
    assert (map.empty() || map[0].size() == r.size());
    map.push_back(r);
  }

  size_t risk_level = 0;
  for (size_t y = 0; y < map.size(); y++) {
    for (size_t x = 0; x < map[y].size(); x++) {
      if (IsLowPoint(map, x, y)) {
        // Risk level is hieght + 1 of a low point
        risk_level += map[y][x] + 1;
      }
    }
  }
  
  std::cout << "Part 1: " << risk_level << std::endl;

  return 0;
}

