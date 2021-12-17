#include "aoc21/helpers.h"
#include <vector>
#include <numeric>

namespace {
  using TargetArea = std::pair<aoc::Point, aoc::Point>;

  const auto test_launch = [](const TargetArea& target, aoc::Point velocity) {
    aoc::Point pos{0, 0};
    while (true) {
      pos.first += velocity.first;
      pos.second += velocity.second;

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
  {
    aoc::AutoTimer part1("part 1");
    const auto max_height = (abs_min_y - 1) * (abs_min_y / 2);
    aoc::print_result(1, max_height);
  }

  {
    aoc::AutoTimer part2("part 2");
    int hits = 0;
    for (int y = target.first.second; y <= abs_min_y; y++) {
      // for each x from 1..targetmax.x
      for (int x = 1; x <= target.second.first; x++) {
        const auto card_x = (x * x + x) / 2;
        if (card_x < target.first.first) {
          continue;
        }
        const bool hit = test_launch(target, {x, y});
        if (hit) { // if we hit, record the max height
          hits++;
        }
      }
    }
    aoc::print_result(2, hits);
  }

  return 0;
}

