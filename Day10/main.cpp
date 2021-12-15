#include "aoc21/helpers.h"
#include <stack>
#include <vector>

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string line;
  int64_t score = 0;
  std::vector<int64_t> completion_scores;
  while (aoc::getline(f, line)) {
    std::stack<char> st;
    bool valid = true;
    size_t pos = 0;
    while (valid && pos < line.size()) {
      const auto c = line[pos];
      switch (c) {
        case '(':
        case '[':
        case '{':
        case '<':
          st.push(c);
          break;
        case ')':
          valid = st.top() == '(';
          if (valid) { st.pop(); }
          else { score += 3; }
          break;
        case ']':
          valid = st.top() == '[';
          if (valid) { st.pop(); }
          else { score += 57; }
          break;
        case '}':
          valid = st.top() == '{';
          if (valid) { st.pop(); }
          else { score += 1197; }
          break;
        case '>':
          valid = st.top() == '<';
          if (valid) { st.pop(); }
          else { score += 25137; }
          break;
        default:
          throw std::runtime_error("Invalid character");
      }
      pos++;
    }

    if (valid) {
      // auto complete
      int64_t ac = 0;
      while (!st.empty()) {
        switch (st.top()) {
        case '(':
          ac *= 5;
          ac += 1;
          break;
        case '[':
          ac *= 5;
          ac += 2;
          break;
        case '{':
          ac *= 5;
          ac += 3;
          break;
        case '<':
          ac *= 5;
          ac += 4;
          break;
        default:
          throw std::runtime_error("Invalid character");
        }
        st.pop();
      }

      completion_scores.push_back(ac);
    }
  }

  const auto m = completion_scores.begin() + completion_scores.size() / 2;
  std::nth_element(completion_scores.begin(), m, completion_scores.end());
  aoc::print_results(score, completion_scores[completion_scores.size() / 2]);

  return 0;
}

