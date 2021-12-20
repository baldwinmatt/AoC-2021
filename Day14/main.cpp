#include "aoc21/helpers.h"

#include <array>
#include <list>
#include <map>

namespace {
  using PolymerPair = std::array<char, 2>;
  using InsertionRules = std::map<PolymerPair, char>;
  using Frequencies = std::map<char, size_t>;
  using PolymerPairCounts = std::map<PolymerPair, size_t>;

  const auto step = [](const PolymerPairCounts& pc, const InsertionRules& rules, Frequencies& f) {
    PolymerPairCounts new_pairs;
    for (const auto& [ k, v]: pc) {
      if (!v) {
        continue;
      }

      const auto r = rules.find(k);
      if (r == rules.end()) {
        continue;
      }

      // account for new first pair
      {
        PolymerPair p{ k[0], r->second };
        auto np = new_pairs.insert({ p, 0 });
        np.first->second += v;
      }

      // account for new second pair
      {
        PolymerPair p{ r->second, k[1] };
        auto np = new_pairs.insert({ p, 0 });
        np.first->second += v;
      }

      // increment frequencies of newly inserted element
      auto fit = f.insert(std::pair(r->second, 0));
      fit.first->second += v;
    }
    return new_pairs;
  };

  const auto calculate_score = [](const Frequencies& f) {
    size_t min = SIZE_MAX;
    size_t max = 0;
    for (const auto& [_, v] : f) {
        (void)_;
        min = std::min(v, min);
        max = std::max(v, max);
    }

    return max - min;
  };

}

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string line;
  aoc::getline(f, line);

  Frequencies freq;
  PolymerPair pair{ 0, 0 };
  PolymerPairCounts pc;

  // Initialize the chain as a count of every polymer pair and get initial frequencies
  for (const auto& c : line) {
    pair[0] = pair[1];
    pair[1] = c;
    if (pair[0]) {
      auto cit = pc.insert({ pair, 0 });
      cit.first->second++;
    }
    auto fit = freq.insert(std::pair(c, 0));
    fit.first->second++;
  }

  // Read the rules
  InsertionRules rules;
  while (aoc::getline(f, line)) {
    assert(line.size() == 7);
    assert(line[2] == ' ');
    assert(line[3] == '-');
    assert(line[4] == '>');
    assert(line[5] == ' ');
    PolymerPair p{line[0], line[1]};
    rules.insert(std::pair(p, line[6]));
  }
  f.close();

  for (int i = 0; i < 40; i++) {
    pc = step(pc, rules, freq);
    if (i == 9) {
      aoc::print_result(1, calculate_score(freq));
    }
  }

  aoc::print_result(2, calculate_score(freq));
  return 0;
}

