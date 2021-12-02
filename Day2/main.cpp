#include <iostream>
#include <fstream>
#include <string_view>
#include <set>

namespace {
    class line {
        std::string data;
    public:
        friend std::istream &operator>>(std::istream &is, line &l) {
            std::getline(is, l.data);
            return is;
        }
        operator std::string() const { return data; }    
    }; 

    using Movement = std::pair<char, int>;

    constexpr std::string_view DIRECTION_FORWARD("forward");
    constexpr std::string_view DIRECTION_UP("up");
    constexpr std::string_view DIRECTION_DOWN("down");

    std::set<std::string_view> DIRECTIONS = {
      DIRECTION_FORWARD,
      DIRECTION_UP,
      DIRECTION_DOWN,
    };
};

int main(int argc, char** argv) {
  if (argc < 2) {
    return -1;
  }

  std::fstream in;
  in.open(argv[1], std::ios::in);

  // Part 1
  int virt = 0;
  int horiz = 0;

  // Part 2
  int aim = 0;
  int depth = 0;

  const auto parse_movement = [](const std::string &s) {
    const auto pos = s.find(' ');
    assert(pos != std::string::npos);

    const std::string_view direction(&s[0], pos);
    const int value = ::atoi(&s[pos + 1]);

    assert(DIRECTIONS.find(direction) != DIRECTIONS.end());
    
    Movement m = std::make_pair(direction[0], value);
    return m;
  };

  const auto walk_path = [&](const std::string& line) {
    const Movement m = parse_movement(line);
    switch (m.first) {
      case 'd':
        virt += m.second;
        aim += m.second;
        break;
      case 'u':
        virt -= m.second;
        aim -= m.second;
        break;
      case 'f':
        horiz += m.second;
        depth += (m.second * aim);
        break;
      default:
        break;
    }
  };

  std::for_each(std::istream_iterator<line>(in), std::istream_iterator<line>(), walk_path);

  std::cout << "Part 1: " << (virt * horiz) << std::endl;
  std::cout << "Part 2: " << (horiz * depth) << std::endl;

  return 0;
}

