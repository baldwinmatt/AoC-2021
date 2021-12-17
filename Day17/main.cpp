#include "aoc21/helpers.h"
#include <vector>
#include <numeric>

namespace {
  using TargetArea = std::pair<aoc::Point, aoc::Point>;

  const auto test_launch = [](const TargetArea& target, aoc::Point velocity, int& max_height) {
    aoc::Point pos{0, 0};
    max_height = 0;
    
    while (true) {
      pos.first += velocity.first;
      pos.second += velocity.second;
      max_height = std::max(pos.second, max_height);

      // in target, done
      if (pos.first >= target.first.first && pos.first <= target.second.first &&
          pos.second >= target.first.second && pos.second <= target.second.second) {
        return true;
      }

      // undershot if y is less than target minimum y
      if (pos.second < target.first.second) {
        return false;
      }

      // overshot if x if we are moving away
      if (pos.first > target.second.first) {
        return false;
      }

      if (velocity.first > 0) {
        velocity.first -= 1;
      } else if (velocity.first < 0) {
        velocity.first += 1;
      }
      velocity.second--;
    }

    return false;
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  TargetArea target;
  std::string tok;
  aoc::getline(f, tok, " =.");
  assert(tok.compare("target") == 0);
  aoc::getline(f, tok, " =.");
  assert(tok.compare("area:") == 0);
  aoc::getline(f, tok, " =.");
  assert(tok.compare("x") == 0);
  aoc::getline(f, tok, " =.");
  // min
  target.first.first = std::stoi(tok);
  aoc::getline(f, tok, " =.");
  // max
  target.second.first = std::stoi(tok);
  aoc::getline(f, tok, " =.");
  assert(tok.compare("y") == 0);
  aoc::getline(f, tok, " =.");
  // min
  target.first.second = std::stoi(tok);
  aoc::getline(f, tok, " =.");
  // max
  target.second.second = std::stoi(tok);
  assert(!aoc::getline(f, tok, " =."));
  f.close();

  // Generate y velocity values in the range miny -> minx * 5 (arbitrary)
  std::vector<int> y_values(target.first.first * 5);
  std::iota(y_values.begin(), y_values.end(), target.first.second);

  const auto result = std::transform_reduce(y_values.begin(), y_values.end(), std::make_pair(INT_MIN, 0),
    // reduce to [max, hits]
    [](const auto& lhs, const auto& rhs) { return std::make_pair(std::max(lhs.first, rhs.first), lhs.second + rhs.second); },
    // transform to max height for given y velocity
    [&target](const int& y) {
      int max_height = 0;
      int hits = 0;
      // for each x from 1..targetmax.x
      for (int x = 1; x <= target.second.first; x++) {
        int height = 0;
        const bool hit = test_launch(target, {x, y}, height);
        if (hit) { // if we hit, record the max height
          max_height = std::max(max_height, height);
          hits++;
        }
      }
      return std::make_pair(max_height, hits);
    });

  aoc::print_results(result.first, result.second);

  return 0;
}

