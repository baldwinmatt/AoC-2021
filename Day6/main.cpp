#include "aoc21/helpers.h"
#include <sstream>
#include <array>
#include <numeric>
#include <vector>

constexpr int MaxLifetime = 8;
using FishCounts = std::array<int64_t, MaxLifetime + 1>;

int main(int argc, char** argv) {
  auto f = aoc::open_argv_1(argc, argv);

  std::string input;
  std::getline(f, input);
  f.close();
  
  FishCounts fish { };
  int64_t count = 0;

  aoc::parse_as_integers(input, ',', [&fish, &count](int i) -> void {
    assert(i <= MaxLifetime);
    fish[i]++;
    count++;
  });

  const auto start = std::chrono::high_resolution_clock::now();
  constexpr int pt_1_end = 80;
  constexpr int pt_2_end = 256;

  int head = 0;
  int tail = MaxLifetime;
  int rebirth_at = 6;

  for (int generation = 0; generation < pt_1_end; generation++) {
    const int64_t recycled = fish[head];
    // shift left
    head = (head + 1) % fish.size();
    tail = (tail + 1) % fish.size();
    rebirth_at = (rebirth_at + 1) % fish.size();

    fish[rebirth_at] += recycled;
    count += recycled;
    fish[tail] = recycled;
  }
  std::cout << "Generation: " << pt_1_end << " count: " << count << std::endl;

  for (int generation = pt_1_end; generation < pt_2_end; generation++) {
    const int64_t recycled = fish[head];
    // shift left
    head = (head + 1) % fish.size();
    tail = (tail + 1) % fish.size();
    rebirth_at = (rebirth_at + 1) % fish.size();

    fish[rebirth_at] += recycled;
    count += recycled;
    fish[tail] = recycled;
  }
  std::cout << "Generation: " << pt_2_end << " count: " << count << std::endl;

  aoc::calculate_time(start);

  return 0;
}

