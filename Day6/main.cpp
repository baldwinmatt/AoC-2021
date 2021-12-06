#include "aoc21/helpers.h"
#include <sstream>
#include <array>
#include <numeric>
#include <vector>

constexpr int MaxLifetime = 8;
using FishLifetimes = std::vector<int>;
using FishCounts = std::array<int64_t, MaxLifetime + 1>;

int main(int argc, char** argv) {
  auto f = open_argv_1(argc, argv);

  std::string input;
  std::getline(f, input);
  f.close();
  
  FishLifetimes fish_ages;
  {
    std::stringstream ss(input);
    std::string segment;
    int num;
    while (aoc::getline(ss, segment, ',')) {
      num = std::stoi(segment);
      assert(num < 9);
      fish_ages.push_back(num);
    }
    num = std::stoi(segment);
    assert(num < 9);
    fish_ages.push_back(num);
  }

  FishCounts fish { };
  for (const auto f : fish_ages) {
    fish[f]++;
  }

  int generation = 0;
  while (generation < 256) {
    const int64_t recycled = fish[0];
    int64_t count = 0;
    for (size_t i = 1; i < fish.size(); i ++) {
      count += fish[i];
      fish[i - 1] = fish[i];
    }
    count += recycled;
    fish[6] += recycled;
    count += recycled;
    fish[8] = recycled;

    generation++;
    DEBUG(std::cout << "Generation: " << generation << " count: " << count << std::endl);
    if (generation == 80 || generation == 256) {
      std::cout << "Generation: " << generation << " count: " << count << std::endl;
    }
  }


  return 0;
}

