#include "aoc21/helpers.h"

int main(int argc, char** argv) {

  auto f = aoc::open_argv_1(argc, argv);

  const auto start = std::chrono::high_resolution_clock::now();

  aoc::calculate_time(start);
  
  return 0;
}

