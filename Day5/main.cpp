#include "aoc21/helpers.h"
#include <array>
#include <numeric>
#include <map>

/*
--- Day 5: Hydrothermal Venture ---
You come across a field of hydrothermal vents on the ocean floor! These vents constantly produce large, opaque clouds, so it would be best to avoid them if possible.

They tend to form in lines; the submarine helpfully produces a list of nearby lines of vents (your puzzle input) for you to review. For example:

0,9 -> 5,9
8,0 -> 0,8
9,4 -> 3,4
2,2 -> 2,1
7,0 -> 7,4
6,4 -> 2,0
0,9 -> 2,9
3,4 -> 1,4
0,0 -> 8,8
5,5 -> 8,2
Each line of vents is given as a line segment in the format x1,y1 -> x2,y2 where x1,y1 are the coordinates of one end the line segment and x2,y2 are the coordinates of the other end. These line segments include the points at both ends. In other words:

An entry like 1,1 -> 1,3 covers points 1,1, 1,2, and 1,3.
An entry like 9,7 -> 7,7 covers points 9,7, 8,7, and 7,7.
For now, only consider horizontal and vertical lines: lines where either x1 = x2 or y1 = y2.

So, the horizontal and vertical lines from the above list would produce the following diagram:

.......1..
..1....1..
..1....1..
.......1..
.112111211
..........
..........
..........
..........
222111....
In this diagram, the top left corner is 0,0 and the bottom right corner is 9,9. Each position is shown as the number of lines which cover that point or . if no line covers that point. The top-left pair of 1s, for example, comes from 2,2 -> 2,1; the very bottom row is formed by the overlapping lines 0,9 -> 5,9 and 0,9 -> 2,9.

To avoid the most dangerous areas, you need to determine the number of points where at least two lines overlap. In the above example, this is anywhere in the diagram with a 2 or larger - a total of 5 points.

Consider only horizontal and vertical lines. At how many points do at least two lines overlap?
*/

namespace {

using Point = std::pair<int, int>;
using PointPair = std::pair<Point, Point>;
using Grid = std::map<Point, int>;

const auto ParsePointPair = [](const std::string& line) {
  std::array<int, 4>points;
  size_t point_num = 0;
  int* cp = &points[point_num];
  *cp = 0;

  for (const char c : line) {
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
        *cp *= 10;
        *cp += (c - '0');
        break;
      case ',':
      case '>':
        point_num++;
        cp = &points[point_num];
        *cp = 0;
        break;
      default:
        break;
    }
  }

  DEBUG(std::cout << points[0] << "," << points[1] << " -> " << points[2] << "," << points[3] << std::endl);
  return std::make_pair<Point, Point>(
      { points[0], points[1] },
      { points[2], points[3] }
    );
};

};

int main(int argc, char** argv) {
  auto f = open_argv_1(argc, argv);
  bool diaganols = false;
  if (argc == 3 && argv[2][0] == '2') {
    diaganols = true;
  }

  Grid grid;
  const auto read_lines = [&grid, &diaganols](const std::string& line) {
    const auto line_points = ParsePointPair(line);

    const int x_diff = line_points.second.first - line_points.first.first;
    const int x_step = x_diff == 0 ? 0 :
      x_diff / ::abs(x_diff);
    const int y_diff = line_points.second.second - line_points.first.second;
    const int y_step = y_diff == 0 ? 0 :
      y_diff / ::abs(y_diff);

    if (x_step && y_step && !diaganols) {
      DEBUG(std::cout << "Skipping diagonal" << std::endl);
      return;
    }

    Point cp = line_points.first;
    const Point& ep = line_points.second;
    while (cp != ep) {
      auto r = grid.emplace(cp, 1);
      if (!r.second) {
        r.first->second++;
      }
      DEBUG(std::cout << r.first->first.first << "," << r.first->first.second << " -> " << r.first->second << std::endl);
      if (cp.first != ep.first) {
        cp.first += x_step;
      }
      if (cp.second != ep.second) {
        cp.second += y_step;
      }
    }
    // Mark the end point
    {
      auto r = grid.emplace(cp, 1);
      if (!r.second) {
        r.first->second++;
      }
    }
  };

  std::for_each(std::istream_iterator<line>(f), std::istream_iterator<line>(), read_lines);

  int points = 0;
  points = std::reduce(grid.begin(), grid.end(), 0, [](int p, const auto& c) -> int {
    p += (c.second >= 2);
    return p;
  });

  std::cout << points << std::endl;

  return 0;
}

