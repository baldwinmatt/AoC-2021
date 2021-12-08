#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <numeric>
#include "aoc21/helpers.h"

namespace {
  using BinaryNumber = std::vector<bool>;
  using BinaryList = std::list<BinaryNumber>;

  const auto to_binary = [](const size_t bits, const auto v) {
    size_t bit = 0;
    std::vector<bool>vb;
    vb.resize(bits);
    while (bit < bits) {
      vb[bits - bit - 1] = ((1 << bit) & v);
      bit++;
    }
    return vb;
  };

  const auto from_binary = [](const BinaryNumber& vb) {
    int num = 0;
    size_t shift = 0;
    for (auto it = vb.rbegin(); it < vb.rend(); it++) {
      num += ((*it) << shift);
      shift++;
    }
    return num;
  };

  const auto count_frequency = [](const auto & l, size_t bit) {
    size_t f = 0;
    size_t c = 0;
    for(const auto vb : l) {
        f += vb[bit];
        c ++;
      }
      return std::pair<size_t, size_t>(c, f);
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto in = aoc::open_argv_1(argc, argv);

  size_t pos = 0;
  std::vector<size_t>frequencies;
  std::set<int>numbers;
  char c;
  size_t count = 0;
  std::vector<bool> number;
  while ((c = in.get()) && in.good()) {
    switch (c) {
      case '1':
        number.push_back(1);
        if (frequencies.size() <= pos) {
          frequencies.push_back(1);
        } else {
          frequencies[pos]++;
        }
        pos++;
        break;
      case '\n':
      case '\r':
        {
          size_t shift = 0;
          int val = 0;
          for (size_t i = number.size(); i > 0; i--) {
            val += (number[i - 1] << shift);
            shift++;
          }
          numbers.emplace(val);
          number.resize(0);
        }
        count += (pos == frequencies.size());
        pos = 0;
        break;
      case '0':
      default:
        number.push_back(0);
        if (frequencies.size() <= pos) {
          frequencies.push_back(0);
        }
        pos++;
        break;
    }
  }

  size_t shift = 0;
  int gamma = 0;
  int epsilon = 0;
  const auto half_count = count / 2;
  const size_t bits = frequencies.size();
  for (pos = frequencies.size(); pos > 0; pos--) {
    const auto val = (frequencies[pos - 1] > half_count);
    gamma += (val << shift);
    epsilon += ((!val) << shift);
    shift++;
  }
  std::cout << "gamma: " << gamma << " epsilon: " << epsilon << " power: " << (gamma * epsilon) << std::endl;

  std::list<std::vector<bool>> oxygen_list;
  std::list<std::vector<bool>> co2_list;
  for (const auto n : numbers) {
    oxygen_list.push_back(to_binary(bits, n));
    co2_list.push_back(to_binary(bits, n));
  }

  size_t bit = 0;
  while (bit <= bits) {
    {
      const auto c_f = count_frequency(oxygen_list, bit);

      auto it = oxygen_list.begin();
      const auto tie = (c_f.second == (c_f.first / 2)) && ((c_f.second * 2) == c_f.first);
      const auto check = c_f.second > (c_f.first / 2);
      while (it != oxygen_list.end() && oxygen_list.size() > 1) {
        if (((*it)[bit] == check && !tie) || (tie && (*it)[bit])) {
          // keep
          it++;
        } else {
          // discard
          auto tmp = it;
          tmp++;
          oxygen_list.erase(it);
          it = tmp;
        }
      }
    }
    {
      const auto c_f = count_frequency(co2_list, bit);

      auto it = co2_list.begin();
      const auto tie = (c_f.second == (c_f.first / 2)) && ((c_f.second * 2) == c_f.first);
      const auto check = c_f.second > (c_f.first / 2);
      it = co2_list.begin();
      while (it != co2_list.end() && co2_list.size() > 1) {
        if (((*it)[bit] != check && !tie) || (tie && !(*it)[bit])) {
          // keep
          it++;
        } else {
          // discard
          auto tmp = it;
          tmp++;
          co2_list.erase(it);
          it = tmp;
        }
      }
    }
    bit++;
  }

  if (oxygen_list.size() != 1 ||
    co2_list.size() != 1) {
    std::cerr << "Oops: " <<  oxygen_list.size() << " : " << co2_list.size() << std::endl;
    return -1;
  }

  const auto oxygen = from_binary(oxygen_list.front());
  const auto scrubber = from_binary(co2_list.front());

  std::cout << "oxygen: " << oxygen << " scrubber: " << scrubber << " life_support: " << (oxygen * scrubber) << std::endl;

  return 0;
}

