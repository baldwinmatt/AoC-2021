#include "aoc21/helpers.h"
#include <vector>

namespace {

  using Row = std::vector<bool>;
  using Sheet = std::vector<Row>;

  enum class Axis {
    X,
    Y,
  };

  class Paper {
  public:
    Paper()
      : _width(0)
      , _height(0)
    {

    }

    void mark_point(size_t x, size_t y) {
      if (y >= _height) {
        _sheet.resize(y + 1);
        _height = y + 1;
      }

      auto& r = _sheet[y];
      
      if (x >= r.size()) {
        _width = std::max(x + 1, _width);
        r.resize(_width, false);
      }

      r[x] = true;
    }

    void fold_along(Axis axis, size_t point) {
      switch (axis) {
        case Axis::X:
          fold_across(point);
          return;
        case Axis::Y:
          fold_up(point);
          return;
      }
      throw std::runtime_error("Invalid fold");
    }

    friend std::ostream& operator<<(std::ostream& os, const Paper p) {
      os << "+";
      for (size_t i = 0; i < p._width; i++) {
        os << "-";
      }
      os << "+" << std::endl;

      for (size_t h = 0; h < p._height; h++) {
        os << "|";
        const auto& r = p._sheet[h];
        for (size_t x = 0; x < p._width; x++) {
          const auto set = x < r.size() && r[x];
          if (set) {
            os << "#";
          } else {
            os << " ";
          }
        }
        os << "|" << std::endl;
      }
      os << "+";
      for (size_t i = 0; i < p._width; i++) {
        os << "-";
      }
      os << "+" << std::endl;

      return os;
    }

    size_t count_marks() const {
      size_t marks = 0;
      for (size_t h = 0; h < _height; h++) {
        const auto& r = _sheet[h];
        for (size_t x = 0; x < _width; x++) {
          const auto set = x < r.size() && r[x];
          marks += set;
        }
      }

      return marks;
    }

  private:
    void fold_up(size_t point) {
      DEBUG_PRINT("Fold up @ " << point);
      // Every point below point gets folded up and marked in the top half
      // such that _sheet[point + 1] is transposed onto _sheet[point - 1], and so forth

      assert(point <= _height / 2);

      const auto rows_to_transpose = _height - point;
      for (size_t i = 1; i < rows_to_transpose; i++) {
        if (i + point >= _height) {
          break;
        }
        const auto& r = _sheet[i + point];
        assert(static_cast<ssize_t>(point - i) >= 0);
        auto& t_r = _sheet[point - i];
        for (size_t x = 0; x < _width; x++) {
          if (x >= t_r.size()) {
            t_r.resize(_width, false);
          }
          if (x >= r.size()) {
            break;
          }

          t_r[x] = (t_r[x] | r[x]);
        }
      }

      _height = point;
    }

    void fold_across(size_t point) {
      DEBUG_PRINT("Fold across @ " << point);
      // Every point below point gets folded across and marked in the left half
      // such that _sheet[*][point + 1] is transposed onto _sheet[*][point - 1], and so forth

      assert(point <= _width / 2);

      const auto cols_to_transpose = _width - point;
      for (auto &r : _sheet) {
        for (size_t i = 1; i < cols_to_transpose; i++) {
          const size_t t_x = point - i;
          const size_t x = point + i;
          if (x >= _width) {
            break;
          }
          assert(static_cast<ssize_t>(point - i) >= 0);

          if (t_x >= r.size()) {
            r.resize(_width, false);
          }

          // No more to fold
          if (x >= r.size()) {
            break;
          }

          r[t_x] = (r[t_x] | r[x]);
        }
      }

      _width = point;
    }

  private:
    size_t _width;
    size_t _height;

    Sheet _sheet;
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  Paper s;
  std::string line;
  // Parse the points
  while (aoc::getline(f, line)) {
    const auto p = line.find(',');

    if (line.empty() ||
      p == std::string::npos) {
      break;
    }

    const auto x = line.substr(0, p);
    const auto y = line.substr(p + 1);

    s.mark_point(std::stoull(x), std::stoull(y));
  }

  size_t part1 = 0;
  do {
    const auto p = line.find('=');
    if (p == std::string::npos) {
      continue;
    }
    assert(p > 0 && p < line.size());
    
    const char c = line[p - 1];
    assert(c == 'x' || c == 'y');

    Axis axis;
    switch (c) {
      case 'x':
        axis = Axis::X;
        break;
      case 'y':
        axis = Axis::Y;
        break;
      default:
        throw std::runtime_error("Invalid fold");
    }

    const auto pt = line.substr(p + 1);
    s.fold_along(axis, std::stoull(pt));

    if (part1 == 0) {
      part1 = s.count_marks();
    }
  } while (aoc::getline(f, line));

  std::cout << "Part 1: " << part1 << std::endl;

  std::cout << s << std::endl;

  return 0;
}

