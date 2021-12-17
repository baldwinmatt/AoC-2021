#include "aoc21/helpers.h"
#include <vector>
#include <numeric>

namespace {
  using TargetArea = std::pair<aoc::Point, aoc::Point>;

  const auto test_launch = [](const TargetArea& target, aoc::Point velocity, int& max_height) {
    aoc::Point pos{0, 0};
    int height = max_height;
    while (true) {
      pos.first += velocity.first;
      pos.second += velocity.second;

      height = std::max(height, pos.second);

      // in target, done
      if (pos.first >= target.first.first && pos.first <= target.second.first &&
          pos.second >= target.first.second && pos.second <= target.second.second) {
        max_height = height;
        return true;
      }

      // undershot if y is less than target minimum y
      if (pos.second < target.first.second) {
        return false;
      }
      // overshot if x if we are moving away
      else if (pos.first > target.second.first) {
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

  const auto abs_min_y = std::abs(target.first.second);
  const auto closed_max_height = (abs_min_y - 1) * (abs_min_y / 2);
  DEBUG_PRINT("Closed form: " << closed_max_height);

  int hits = 0;
  int max_height = 0;
  size_t tested = 0;
  DEBUG_PRINT("X Range: 1.." << target.second.first);
  DEBUG_PRINT("Y Range: " << target.first.second << ".." << abs_min_y);
  for (int y = target.first.second; y <= abs_min_y; y++) {
    // for each x from 1..targetmax.x
    for (int x = 1; x <= target.second.first; x++) {
      const auto card_x = (x * x + x) / 2;
      if (card_x < target.first.first) {
        continue;
      }
      tested++;
      const bool hit = test_launch(target, {x, y}, max_height);
      if (hit) {
        DEBUG_PRINT("Hit { " << x << ", " << y << " }");
        hits++;
      }
    }
  }
  DEBUG_PRINT("Long form: " << max_height);
  DEBUG_PRINT("tested: " << tested);
  assert(closed_max_height == max_height);
  aoc::print_results(closed_max_height, hits);

  return 0;
}

