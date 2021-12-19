#include "aoc21/helpers.h"
#include <vector>

namespace {
  class Number {
  protected:
    int value;
    std::vector<Number> parts;

    Number()
      : value(0)
    { }

  public:
    Number(int v)
      : value(v)
    { }

    Number(const Number& l, const Number&r)
      : Number()
    {
      parts.emplace_back(l);
      parts.emplace_back(r);
    }

    // recursively build our tree
    static Number from_string(const std::string& s, size_t& p) {
      const auto c = s[p++];
      switch (c) {
        case '[':
        {
          Number n;
          n.parts.push_back(Number::from_string(s, p));
          p++;
          n.parts.push_back(Number::from_string(s, p));
          p++;
          return n;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          return Number(c - '0');
        default:
          throw std::runtime_error("Bad input");
      }
    }

  protected:

    class Splitter {
    public:
      bool did_split;

      Splitter()
        : did_split(false)
      { }

      bool split(Number& n) {
        // already done
        if (did_split) { return did_split; }

        if (n.parts.empty()) {
          // split
          if (n.value >= 10) {
            n.parts.push_back(n.value / 2); // round down
            n.parts.push_back((n.value + 1) / 2); // round up
            n.value = 0;
            did_split = true;
          }

          return did_split;
        }

        split(n.parts[0]);
        split(n.parts[1]);

        return did_split;
      }

      
    };

    bool split() {
      Splitter s;

      return s.split(*this);
    }

    class Exploder {
    public:
      bool exploded;
      int lv;
      int rv;
      Number* l;
      Number* r;

      Exploder()
        : exploded(false)
        , lv(0)
        , rv(0)
        , l(nullptr)
        , r(nullptr)
      { }

      void explode(Number& n, int depth) {
        // if we are a composite number
        if (!n.parts.empty()) {
          // and haven't exploded at depth 4, then explode
          if (depth >= 4 && !exploded) {
            // we must be at a leaf
            assert(n.parts.size() == 2);
            lv = n.parts[0].value;
            rv = n.parts[1].value;

            // value becomes 0, and no more parts
            n.value = 0;
            n.parts.clear();
            exploded = true;
            return;
          }

          // else explode our parts
          explode(n.parts[0], depth + 1);
          explode(n.parts[1], depth + 1);
          return;
        }

        // else if we exploded, get the right value to add
        if (exploded) {
          if (!r) { r = &n; }
          return;
        }

        // else, this is the left node
        l = &n;
      }

      static bool explode(Number& n) {
        Exploder e;
        e.explode(n, 0);
        if (e.l) { e.l->value += e.lv; }
        if (e.r) { e.r->value += e.rv; }
        return e.exploded;
      }
    };

    bool explode() {
      return Exploder::explode(*this);
    }

    void reduce() {
      bool did_reduce = false;
      do {
        // explode...
        did_reduce = this->explode();
        if (did_reduce) { continue; }

        // then split
        did_reduce = this->split();

      } while(did_reduce);
    }

  public:
    Number& operator+=(const Number& rhs) {
      Number l(this->value);
      l.parts = std::move(this->parts);

      this->parts.push_back(l);
      this->parts.push_back(rhs);
      this->reduce();
      return *this;
    }

    Number operator+(const Number& rhs) {
      Number n(*this, rhs);
      n.reduce();
      return n;
    }

    int magnitude() {
      if (parts.empty()) {
        return value;
      }

      return (3 * parts[0].magnitude()) + (2 * parts[1].magnitude());
    }

    friend std::ostream& operator<<(std::ostream& os, const Number& n) {
      if (n.parts.empty()) {
        os << n.value;
      } else {
        os << '[' << n.parts[0] << ',' << n.parts[1] << ']';
      }
      return os;
    }
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string line;
  size_t pos = 0;
  // First line is left most number
  aoc::getline(f, line);
  Number l = Number::from_string(line, pos);
  assert(pos == line.size());
  DEBUG_PRINT(l);

  std::vector<Number> numbers;
  numbers.push_back(l);
  // Keep adding each line
  while (aoc::getline(f, line)) {
    pos = 0;
    const Number r = Number::from_string(line, pos);
    numbers.push_back(r);
    DEBUG_PRINT(r);
    l += r;
    DEBUG_PRINT(l);
    assert(pos == line.size());
  }
  // then return the magnitude
  aoc::print_result(1, l.magnitude());
  t.elapsed();

  int max_mag = INT_MIN;
  for (size_t i = 0; i < numbers.size(); i++) {
    for (size_t j = i + 1; j < numbers.size(); j++) {
      max_mag = std::max(max_mag, (numbers[i] + numbers[j]).magnitude());
      max_mag = std::max(max_mag, (numbers[j] + numbers[i]).magnitude());
    }
  }
  aoc::print_result(2, max_mag);

  return 0;
}

