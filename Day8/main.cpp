#include "aoc21/helpers.h"
#include <string_view>
#include <array>
#include <vector>
#include <map>
#include <chrono>


namespace {
  STRING_CONSTANT(Zero, "abcefg");
  STRING_CONSTANT(One, "cf");
  STRING_CONSTANT(Two, "acdeg");
  STRING_CONSTANT(Three, "acdfg");
  STRING_CONSTANT(Four, "bcdf");
  STRING_CONSTANT(Five, "abdfg");
  STRING_CONSTANT(Six, "abdefg");
  STRING_CONSTANT(Seven, "acf");
  STRING_CONSTANT(Eight, "abcdefg");
  STRING_CONSTANT(Nine, "abcdfg");

  using InputPattern = std::array<std::string, 10>;
  using OutputPattern = std::array<std::string, 4>;
  using InputOutputPair = std::pair<InputPattern, OutputPattern>;
  
  std::vector<std::string_view> NormalizedInputs {
    Zero,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine
  };

  const auto parse_pair = [](const std::string& input) {
    InputOutputPair p;
    std::string segment;
    int pos = 0;
    std::stringstream ss(input);
    // tokenize the line by spaces
    // first 10 parts are for the input
    // then a |
    // then 4 more for the output
    while (aoc::getline(ss, segment, ' ')) {
      if (pos < 10) {
        p.first[pos] = segment;
      } else if (pos < 11) {
        assert(segment == std::string("|"));
      } else if (pos < 15) {
        p.second[pos - 11] = segment;
      }
      pos++;
    }

    return p;
  };

  using DecoderMap = std::map<int, int>;
  using SignalMap = std::map<char, int>;

  const auto DecodeOutput = [](const DecoderMap& decoded, const InputOutputPair& io_pair) {
    SignalMap counts{
      { 'a', 0 },
      { 'b', 0 },
      { 'c', 0 },
      { 'd', 0 },
      { 'e', 0 },
      { 'f', 0 },
      { 'g', 0 },
    };

    // Count the number of times each signal wire is used
    for (const auto& i : io_pair.first) {
      for (const char c : i) {
        counts[c]++;
      }
    }
    // replace signal wire with count and sum for each digit
    for (const auto& i : io_pair.first) {
      int val = 0;
      for (const char c : i) {
        val += counts[c];
      }
    }

    int output = 0;
    for (const auto& d : io_pair.second) {
      int digit = 0;
      switch (d.size()) {
        case 2:
          digit = 1;
          break;
        case 3:
          digit = 7;
          break;
        case 4:
          digit = 4;
          break;
        case 7:
          digit = 8;
          break;
        default:
          int val = 0;
          for (const char c : d) {
            val += counts[c];
          }
          const auto it = decoded.find(val);
          assert(it != decoded.end());
          digit = it->second;
          break;
      }
      output *= 10;
      output += digit;
    }
    return output;
  };
};

int main(int argc, char** argv) {
  auto f = aoc::open_argv_1(argc, argv);

  std::string line;
  size_t is_decodable = 0;
  size_t sum = 0;


  const auto MakeDecoder = [](const auto inputs) {
    SignalMap counts{
      { 'a', 0 },
      { 'b', 0 },
      { 'c', 0 },
      { 'd', 0 },
      { 'e', 0 },
      { 'f', 0 },
      { 'g', 0 },
    };
    
    // Count the number of times each signal wire is used
    for (const auto& i : inputs) {
      for (const char c : i) {
        counts[c]++;
      }
    }
    // replace signal wire with count and sum for each digit
    DecoderMap map;
    for (size_t i = 0; i < inputs.size(); i++) {
      int val = 0;
      for (const char c : inputs[i]) {
        val += counts[c];
      }
      DEBUG(std::cout << i << " = " << val << std::endl);
      map.insert(std::pair<int, int>(val, i));
    }
    return map;
  };
  const auto decoder = MakeDecoder(NormalizedInputs);

  const auto start = std::chrono::high_resolution_clock::now();
  while (aoc::getline(f, line)) {
    const auto io_pair = parse_pair(line);

    for (const auto& d : io_pair.second) {
      const auto len = d.size();
      is_decodable += (len == One.size() ||
        len == Four.size() ||
        len == Seven.size() ||
        len == Eight.size());
    }

    sum += DecodeOutput(decoder, io_pair);
  }

  std::cout << "Part 1: " << is_decodable << std::endl;
  std::cout << "Part 2: " << sum << std::endl;
  // Part 1: 532
  // Part 2: 1011284

  aoc::calculate_time(start);

  return 0;
}

