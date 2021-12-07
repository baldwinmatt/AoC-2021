#include "aoc21/helpers.h"
#include <numeric>
#include <vector>

using InitialPositions = std::vector<int>;

int main(int argc, char** argv) {
  auto f = aoc::open_argv_1(argc, argv);

  std::string input;
  std::getline(f, input);
  f.close();
  
  InitialPositions pos { };
  aoc::parse_as_integers(input, ',', [&pos](int i) -> void { pos.push_back(i); });

  std::sort(pos.begin(), pos.end());
  const size_t count = pos.size();
  const size_t mid = count / 2;
  const int median = (count % 2 == 0) ? ((pos[mid] + pos[mid - 1]) / 2) : pos[mid];
  const int mean_up = std::ceil(static_cast<double>(std::accumulate(pos.begin(), pos.end(), 0)) / static_cast<double>(count));
  const int mean_down = std::floor(static_cast<double>(std::accumulate(pos.begin(), pos.end(), 0)) / static_cast<double>(count));

  size_t fuel = std::reduce(pos.begin(), pos.end(), 0, [&median](size_t s, const auto& i) {
    const int distance_to_median = (i > median) ? (i - median) : (median - i);
    return s + distance_to_median;
  });
  std::cout << "Part 1: " << median << ", " << fuel << std::endl;

  size_t fuel_up = std::reduce(pos.begin(), pos.end(), 0, [&mean_up](size_t s, const auto& i) {
    const int distance_to_mean = (i > mean_up) ? (i - mean_up) : (mean_up - i);
    const size_t fuel_to_mean = (distance_to_mean * distance_to_mean + distance_to_mean) / 2;
    return s + fuel_to_mean;
  });
  DEBUG(std::cout << mean_up << ", " << fuel_up << std::endl);

  size_t fuel_down = std::reduce(pos.begin(), pos.end(), 0, [&mean_down](size_t s, const auto& i) {
    const int distance_to_mean = (i > mean_down) ? (i - mean_down) : (mean_down - i);
    const size_t fuel_to_mean = (distance_to_mean * distance_to_mean + distance_to_mean) / 2;
    return s + fuel_to_mean;
  });
  DEBUG(std::cout << mean_down << ", " << fuel_down << std::endl);

  std::cout << "Part 2: ";
  fuel = std::min(fuel_up, fuel_down);
  if (fuel == fuel_down) {
    std::cout << mean_down << ", " << fuel_down << std::endl;
  } else {
    std::cout << mean_up << ", " << fuel_up << std::endl;
  }

  return 0;
}

