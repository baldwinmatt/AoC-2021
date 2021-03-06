#include "aoc21/helpers.h"
#include <vector>
#include <set>
#include <chrono>
#include <thread>

namespace {
  using Point = aoc::Point;
  using Algorithm = std::vector<bool>;     // Light points are true
  using Grid = std::set<Point>;            // Only lit points are stored

  const std::vector<Point> offsets{
    { -1, -1 },
    {  0, -1 },
    {  1, -1 },
    { -1,  0 },
    {  0,  0 },
    {  1,  0 },
    { -1,  1 },
    {  0,  1 },
    {  1,  1 },
  };
  
  class Image {
  protected:
    Grid grid_;
    Point top_left_;
    Point bottom_right_;
    bool is_back_lit_;

  public:

    Image()
      : top_left_(0, 0)
      , bottom_right_(0, 0)
      , is_back_lit_(false)
    {
    }

    bool get(const Point& pt) const {
      return get(pt, is_back_lit_);
    }

    bool get(const Point& pt, bool is_back_lit) const {
      if (pt.first < top_left_.first ||
        pt.second < top_left_.second ||
        pt.first > bottom_right_.first ||
        pt.second > bottom_right_.second) {
        return is_back_lit;
      }

      const auto r = grid_.find(pt);
      if (r != grid_.end()) {
        return true;
      }
      return false;
    }

    void light_up(const Point& pt) {
      light_up(grid_, pt, top_left_, bottom_right_);
    }

    void light_up(Grid& g, const Point& pt, Point& top_left, Point& bottom_right) const {
#if !defined(NDEBUG)
      const auto r =
#endif
        g.emplace(pt);
      assert(r.second);

      top_left.first = std::min(top_left.first, pt.first);
      top_left.second = std::min(top_left.second, pt.second);
      bottom_right.first = std::max(bottom_right.first, pt.first);
      bottom_right.second = std::max(bottom_right.second, pt.second);
    }

    int value_of(const Point& p) const {
      int v = 0;
      for (const auto& o : offsets) {
        Point pt{p.first + o.first, p.second + o.second};
        const auto c = get(pt);
        v <<= 1;
        v |= (c == true);
      }
      assert(v < 512);
      return v;
    }

    size_t enhance(const Algorithm& alg) {
      size_t lit = 0;
      
      const int min_x = top_left_.first;
      const int min_y = top_left_.second;
      size_t height = bottom_right_.second - top_left_.second + 1;
      size_t width = bottom_right_.first - top_left_.first + 1;

      Grid new_grid;
      // First, set the new grids background as it is infinite space
      // and may toggle
      bool is_back_lit;
      {
        const auto v = value_of({min_x - 3, min_y - 3});
        is_back_lit = alg[v];
      }
      
      // to enhance, get the binary value which represents the 9 pixels surrounding it
      Point tl{0, 0};
      Point br{0, 0};
      for (size_t y = 0; y <= height + 2; y++) {
        for (size_t x = 0; x <= width + 2; x++) {
          const Point p{x + min_x - 1, y + min_y - 1};
          const auto v = value_of(p);
          if (alg[v]) {
            light_up(new_grid, p, tl, br);
          }
          lit += alg[v];
        }
      }
      grid_ = new_grid;
      top_left_ = tl;
      bottom_right_ = br;
      is_back_lit_ = is_back_lit;

      return lit;
    }

    friend std::ostream& operator<<(std::ostream& os, Image&im) {
      const size_t height = im.bottom_right_.second - im.top_left_.second + 1;
      const size_t width = im.bottom_right_.first - im.top_left_.first + 1;

      for (size_t y = 0; y <= height; y++) {
        for (size_t x = 0; x <= width; x++) {
          const Point p{x + im.top_left_.first, y + im.top_left_.second};
          if (im.get(p)) {
              os << "#";
          } else {
              os << ".";
          }
        }
        os << std::endl;
      }
      return os;
    }
  };

};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  // first line is algorithm
  std::string line;
  aoc::getline(f, line);
  assert(line.size() == 512);
  Algorithm alg;
  alg.reserve(line.size());
  for (const auto& c: line) {
    assert(c == '.' || c == '#');
    alg.push_back(c == '#');
  }
  Image image;
  Point pt{0, 0};
  while (aoc::getline(f, line)) {
    pt.first = 0;
    for (const auto& c : line) {
      assert(c == '.' || c == '#');
      if (c == '#') {
        image.light_up(pt);
      }
      pt.first++;
    }
    pt.second++;
  }
  f.close();

  DEBUG(std::cout << aoc::cls << image << std::endl);

  size_t lit1 = 0;
  for (int i = 0; i < 2; i++) {
    lit1 = image.enhance(alg);
    if (i % 2) {
      DEBUG(std::this_thread::sleep_for (std::chrono::milliseconds(10)));
      DEBUG(std::cout << aoc::cls << image << std::endl);
    }
  }

  size_t lit2 = 0;
  for (int i = 2; i < 50; i++) {
    lit2 = image.enhance(alg);
    if (i % 2) {
      DEBUG(std::this_thread::sleep_for (std::chrono::milliseconds(10)));
      DEBUG(std::cout << aoc::cls << image << std::endl);
    }
  }
  aoc::print_results(lit1, lit2);

  DEBUG(
    bool show = true;
    while(1) {
      image.enhance(alg);
      show = !show;
      if (show) {
        DEBUG(std::this_thread::sleep_for (std::chrono::milliseconds(10)));
        DEBUG(std::cout << aoc::cls << image << std::endl);
      }
    }
  );

  return 0;
}

