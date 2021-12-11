#include "aoc21/helpers.h"
#include <vector>
#include <queue>
#include <utility>

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

--- Part Two ---
Next, you need to find the largest basins so you know what areas are most important to avoid.

A basin is all locations that eventually flow downward to a single low point. Therefore, every low point has a basin, although some basins are very small. Locations of height 9 do not count as being in any basin, and all other locations will always be part of exactly one basin.

The size of a basin is the number of locations within the basin, including the low point. The example above has four basins.

The top-left basin, size 3:

2199943210
3987894921
9856789892
8767896789
9899965678
The top-right basin, size 9:

2199943210
3987894921
9856789892
8767896789
9899965678
The middle basin, size 14:

2199943210
3987894921
9856789892
8767896789
9899965678
The bottom-right basin, size 9:

2199943210
3987894921
9856789892
8767896789
9899965678
Find the three largest basins and multiply their sizes together. In the above example, this is 9 * 14 * 9 = 1134.

What do you get if you multiply together the sizes of the three largest basins?

*/

namespace {
  using Row = std::vector<int>;
  using HeightMap = std::vector<Row>;
  using VisitedRow = std::vector<bool>;
  using VisitedMap = std::vector<VisitedRow>;

  using Point = std::pair<int, int>;
  using PointQueue = std::queue<Point>;

  const auto ParseRow = [](const auto& line) {
    Row r;

    // We know how big the row will be
    r.reserve(line.size() + 2);
    r.push_back(9);

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
    r.push_back(9);

    return r;
  };

  // Flood fill a basin, and return its size
  const auto FillBasin = [](const HeightMap& map, size_t x, size_t y, VisitedMap& done) {
    size_t size = 0;

    PointQueue q;
    q.emplace(y, x);
    while (!q.empty()) {
      const auto p = q.front(); q.pop();

      if (done[p.first][p.second]) {
        continue;
      }

      done[p.first][p.second] = true;

      const auto height = map[p.first][p.second];
      if (height == 9) {
        continue;
      }

      if (!done[p.first - 1][p.second]) { q.emplace(p.first - 1, p.second); }
      if (!done[p.first + 1][p.second]) { q.emplace(p.first + 1, p.second); }
      if (!done[p.first][p.second - 1]) { q.emplace(p.first, p.second - 1); }
      if (!done[p.first][p.second + 1]) { q.emplace(p.first, p.second + 1); }
      size++;
    }

    return size;
  };

  // Determine if a point is a low point, that is, it is lower than all its neighbours
  const auto IsLowPoint = [](const HeightMap& map, size_t x, size_t y) {
    const auto height = map[y][x];

    if (height == 9) {
      return false;
    }

    assert(x > 0 && x < map[0].size() - 1);
    assert(y > 0 && y < map.size() - 1);

    return height < map[y][x - 1] && height < map[y][x + 1] &&
           height < map[y - 1][x] && height < map[y + 1][x];
  };
};

int main(int argc, char** argv) {

  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  HeightMap map;
  VisitedMap done;
  std::string line;
  while (aoc::getline(f, line)) {
    const auto r = ParseRow(line);
    VisitedRow vr(r.size(), false);
    assert (map.empty() || map[0].size() == r.size());
    if (map.empty()) {
      Row pad(r.size(), 9);
      map.push_back(pad);

      done.push_back(vr);
      done.push_back(vr);
    }

    map.push_back(r);
    done.push_back(vr);
  }

  Row pad(map[0].size(), 9);
  map.push_back(pad);

  size_t risk_level = 0;
  std::vector<size_t> basin_sizes;
  for (size_t y = 1; y < map.size() - 1; y++) {
    for (size_t x = 1; x < map[y].size() - 1; x++) {
      if (IsLowPoint(map, x, y)) {
        // Risk level is hieght + 1 of a low point
        risk_level += map[y][x] + 1;

        size_t size = FillBasin(map, x, y, done);
        if (size > 0) {
          basin_sizes.push_back(size);
        }
      }
    }
  }

  std::sort(basin_sizes.begin(), basin_sizes.end(), std::greater<>());
  assert(basin_sizes.size() >= 3);

  std::cout << "Part 1: " << risk_level << std::endl;
  std::cout << "Part 2: " << (basin_sizes[0] * basin_sizes[1] * basin_sizes[2]) << std::endl;

  return 0;
}

