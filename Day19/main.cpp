#include "aoc21/helpers.h"
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>

namespace {
  class Point {
  public:
    int x;
    int y;
    int z;

    Point()
      : x(0)
      , y(0)
      , z(0)
    { }

    Point(int x, int y, int z)
      : x(x)
      , y(y)
      , z(z)
    { }

    void zero() {
      x = y = z = 0;
    }

    size_t distance(const Point& rhs) const {
      return ::abs(x - rhs.x) +
        ::abs(y - rhs.y) +
        ::abs(z - rhs.z);
    }

    bool operator==(const Point& b) const { return b.x == x && b.y == y && b.z == z; }
    bool operator<(const Point& b) const { return x < b.x; }
    Point& operator+=(const Point& rhs) {
      x += rhs.x;
      y += rhs.y;
      z += rhs.z;
      return *this;
    }
    friend Point operator+(Point lhs, const Point& rhs) {
      lhs += rhs;
      return lhs;
    }
    Point& operator-=(const Point& rhs) {
      x -= rhs.x;
      y -= rhs.y;
      z -= rhs.z;
      return *this;
    }
    friend Point operator-(Point lhs, const Point& rhs) {
      lhs -= rhs;
      return lhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point& b) {
      os << "{ " << b.x << ", " << b.y << ", " << b.z << " }"; 
      return os;
    }
  };

  struct PointHash {
    std::size_t operator() (const Point& pt) const {
      // [ x | y | z]
      // assume they are all less than 21 bits
      assert (pt.x < (1 << 22));
      assert (pt.y < (1 << 22));
      assert (pt.z < (1 << 22));
      size_t v = pt.x;
      v <<= 21;
      v |= pt.y;
      v <<= 21;
      v |= pt.z;
      return v;
    }
  };
  using Beacons = std::vector<Point>;
  using BeaconSet = std::unordered_set<Point, PointHash>;

  const auto rotate = [](int i, const Point& b) {
    switch (i) {
      case 0: return Point(b.x, b.y, b.z); case 1: return Point(b.x,-b.z, b.y); case 2: return Point(b.x,-b.y,-b.z); case 3: return Point(b.x, b.z,-b.y);
      case 4: return Point(-b.x,-b.z,-b.y); case 5: return Point(-b.x, b.y,-b.z); case 6: return Point(-b.x, b.z, b.y); case 7: return Point(-b.x,-b.y, b.z);
      case 8: return Point(b.y, b.z, b.x); case 9: return Point(b.y,-b.x, b.z); case 10: return Point(b.y,-b.z,-b.x); case 11: return Point(b.y, b.x,-b.z);
      case 12: return Point(-b.y,-b.x,-b.z); case 13: return Point(-b.y, b.z,-b.x); case 14: return Point(-b.y, b.x, b.z); case 15: return Point(-b.y,-b.z, b.x);
      case 16: return Point(b.z, b.x, b.y); case 17: return Point(b.z,-b.y, b.x); case 18: return Point(b.z,-b.x,-b.y); case 19: return Point(b.z, b.y,-b.x);
      case 20: return Point(-b.z,-b.y,-b.x); case 21: return Point(-b.z, b.x,-b.y); case 22: return Point(-b.z, b.y, b.x); case 23: return Point(-b.z,-b.x, b.y);
    }
    return Point();
  };

  class Scanner {
  public:
    const size_t id;
    Point pos;
    int orientation;
    Beacons beacons;

    Scanner(size_t id)
      : id(id)
      , pos()
      , orientation(0)
    { }

    Point abs_beacon_location(const Point& b) const {
      // rotate the beacon about our orientation
      Point out = rotate(orientation, b);

      // Now apply our position to the beacon to get its absolute location
      out += pos;

      return out;
    }

    Beacons get_beacons() const {
      // make all beacons absolute relative to our position
      Beacons out;
      std::transform(beacons.begin(), beacons.end(), std::back_inserter(out),
        [this](const auto& b) { return this->abs_beacon_location(b); });
      return out;
    }

    BeaconSet get_beacon_set() const {
      // make all beacons absolute relative to our position
      BeaconSet out;
      std::transform(beacons.begin(), beacons.end(), std::inserter(out, out.end()),
        [this](const auto& b) { return this->abs_beacon_location(b); });
      return out;
    }

    // set our position relative to the beacon with index and offset
    void set_beacon_abs(size_t beacon, const Point& offset) {
      assert(beacon < beacons.size());
      const auto& b = beacons[beacon];
      pos.zero();
      const auto loc = abs_beacon_location(b);
      pos = offset - loc;

      assert(offset == abs_beacon_location(b));
    }
  };

  using ScannerList = std::list<Scanner>;
  using Scanners = std::vector<Scanner>;

  const auto count_matches = [](const Scanner& base, Scanner& s) {
    const auto base_beacons = base.get_beacon_set();

    for (int o = 0 ; o < 24; o++) {
      s.orientation = o;

      for (const auto& pos : base_beacons) {
        for (size_t i = 0; i < s.beacons.size(); i++) {
          s.set_beacon_abs(i, pos);
          size_t match = 0;
          for (const auto& c : s.get_beacons()) {
            match += (base_beacons.find(c) != base_beacons.end());
          }
          if (match >= 12) {
            DEBUG_PRINT("Scanner " << s.id << " pos: " << s.pos << " @ " << s.orientation);
            return true;
          }
        }
      }
    }

    return false;
  };

  const auto build_map = [](ScannerList& input) {
    Scanners out;

    // first scanner is reference scanner
    out.emplace_back(input.front());
    input.pop_front();

    std::set<std::pair<size_t, size_t>> no_match;

    while (!input.empty()) {
      for (const auto& base : out) {
        bool matched = false;
        auto it = input.begin();
        for (size_t i = 0; i < input.size(); i++) {
          auto& s = *it;
          std::pair<size_t, size_t> id_pair{base.id, s.id};

          // optimizations from SalusaSecondus
          if (no_match.find(id_pair) != no_match.end()) {
            it++;
            continue;
          }

          const auto does_match = count_matches(base, s);
          // need at least 12 matches
          if (does_match) {
            DEBUG_PRINT("Matched: " << base.id << " with " << s.id);
            out.emplace_back(s);
            input.erase(it);
            matched = true;
            break;
          } else {
            // optimizations from SalusaSecondus
            no_match.emplace(id_pair);
            no_match.emplace(s.id, base.id);
          }
          it++;
        }

        if (matched) {
          DEBUG_PRINT("[ left: " << input.size() << " done: " << out.size() << " ]");
          break;
        }
      }
    }

    return out;
  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string line;
  ScannerList sq;
  while (aoc::getline(f, line)) {
    if (line.find("scanner") != std::string::npos) {
      DEBUG_PRINT("Scanner: " << sq.size());
      sq.emplace_back(sq.size());
    } else {

      Scanner& s = sq.back();
      int x = INT_MAX;
      int y = INT_MAX;
      int z = INT_MAX;
      
      aoc::parse_as_integers(line, ',', [&x, &y, &z](const int v) {
        if (x == INT_MAX) { x = v; }
        else if (y == INT_MAX) { y = v; }
        else { z = v; }
      });
      s.beacons.emplace_back(x, y, z);
      DEBUG_PRINT("Beacon(" << s.beacons.size() << "): " << s.beacons.back());
    }
  }
  f.close();

  BeaconSet beacons;
  const auto& scanners = build_map(sq);
  for (const auto& s : scanners) {
    for (const auto& b : s.get_beacons()) {
      beacons.emplace(b);
    }
  }

  size_t max_dist = 0;
  for (const auto& s1 : scanners) {
    for (const auto& s2 : scanners) {
      const auto dist = s1.pos.distance(s2.pos);
      max_dist = std::max(dist, max_dist);
    }
  }

  aoc::print_results(beacons.size(), max_dist);

  return 0;
}

