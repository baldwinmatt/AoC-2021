#include "aoc21/helpers.h"
#include <numeric>
#include <vector>
#include <cmath>
#include <numeric>

using InitialPositions = std::vector<int>;

int main(int argc, char** argv) {
  aoc::AutoTimer t;

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
  const int mean_up = ::ceil(static_cast<double>(std::accumulate(pos.begin(), pos.end(), 0)) / static_cast<double>(count));
  const int mean_down = ::floor(static_cast<double>(std::accumulate(pos.begin(), pos.end(), 0)) / static_cast<double>(count));

  size_t fuel = std::accumulate(pos.begin(), pos.end(), 0, [&median](size_t s, const auto& i) {
    const int distance_to_median = (i > median) ? (i - median) : (median - i);
    return s + distance_to_median;
  });
  DEBUG_PRINT(median << ", " << fuel);
  aoc::print_result(1, fuel);

  size_t fuel_up = std::accumulate(pos.begin(), pos.end(), 0, [&mean_up](size_t s, const auto& i) {
    const int distance_to_mean = (i > mean_up) ? (i - mean_up) : (mean_up - i);
    const size_t fuel_to_mean = (distance_to_mean * distance_to_mean + distance_to_mean) / 2;
    return s + fuel_to_mean;
  });
  DEBUG_PRINT(mean_up << ", " << fuel_up);

  size_t fuel_down = std::accumulate(pos.begin(), pos.end(), 0, [&mean_down](size_t s, const auto& i) {
    const int distance_to_mean = (i > mean_down) ? (i - mean_down) : (mean_down - i);
    const size_t fuel_to_mean = (distance_to_mean * distance_to_mean + distance_to_mean) / 2;
    return s + fuel_to_mean;
  });
  DEBUG_PRINT(mean_down << ", " << fuel_down);

  aoc::print_result(2, std::min(fuel_up, fuel_down));

  return 0;
}

