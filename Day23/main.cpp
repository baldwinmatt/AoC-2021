#include "aoc21/helpers.h"
#include <array>
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <vector>

namespace {
  constexpr int RoomCount = 4;
  constexpr int RoomDepth = 2;
  constexpr int HallwaySize = 4 + (RoomCount * 2) - 1;

  using Hallway = std::array<char, HallwaySize>;
  using Room = std::string;
  using Rooms = std::array<Room, RoomCount>;

  using CharIntMap = std::map<char, int>;
  CharIntMap EnergyCost = { {'A', 1 }, { 'B', 10 }, { 'C', 100 }, { 'D', 1000 } };
  CharIntMap RoomAssignments = { { 'A', 0 }, { 'B', 1 }, { 'C', 2 }, { 'D', 3 } };

  const auto IsFish = [](const char f) {
    return f >= 'A' && f <= 'D';
  };
#if !defined (NDEBUG)
  const auto IsRoom = [](const int room) {
      return room >= 0 && room < RoomCount;
  };
#endif

  const auto TargetRoom = [](const char f) {
    assert(IsFish(f));
    return f - 'A';
  };

  class MapState {
  public:
    int energy;
    int roomDepth;
    Hallway hallway;
    Rooms rooms;
    std::vector<int>doorways;

    MapState(int depth)
      : energy(0)
      , roomDepth(depth)
    {
      for (auto& c : hallway) {
        c = '.';
      }
    }

    MapState()
      : MapState(RoomDepth)
    { }

    void insertFish(const std::vector<std::string> fish) {
      int i = 0;
      for (auto& r : rooms) {
        std::string new_r(&r[0], 1);
        new_r += fish[i++];
        new_r += (r.substr(1));
        r = new_r;
      }
      roomDepth = rooms[0].size();
    }

    std::string hash() const {
      std::string s;
      for (const auto&c : hallway) {
        s.append(&c, 1);
      }
      for (const auto& r : rooms) {
        s.append(",", 1);
        s.append(r);
      }
      return s;
    }

    bool isDesiredRoom(char fish, int room) const {
      assert(IsFish(fish));
      assert(IsRoom(room));

      return RoomAssignments[fish] == room;
    }

    bool isRoomFinal(int room) const {
      assert(IsRoom(room));
      if (rooms[room].size() < static_cast<size_t>(roomDepth)) {
        return false;
      }

      for (const auto& f : rooms[room]) {
        if (!isDesiredRoom(f, room)) {
          return false;
        }
      }
      return true;
    }

    bool final() const {
      for (int i = 0; i < RoomCount; i++) {
        if (!isRoomFinal(i)) {
          return false;
        }
      }
      return true;
    }

    bool isDoorway(const int p) const {
      return std::find(doorways.begin(), doorways.end(), p) != doorways.end();
    }

    int getRoom(const int p) const {
      for (size_t i = 0; i < doorways.size(); i++) {
        if (doorways[i] == p) {
          return i;
        }
      }

      return -1;
    }

    MapState move(int from, int to) {
      MapState next(*this);

      auto fish = next.hallway[from];
      int steps = 0;
      const int from_room = getRoom(from);
      const int to_room = getRoom(to);
      if (from_room != -1) {
        fish = next.rooms[from_room].front();

        // remove the first element from the room
        next.rooms[from_room].erase(0, 1);
        // steps to get out of room
        steps += (next.roomDepth - next.rooms[from_room].size());
      } else {
        next.hallway[from] = '.';
      }

      // steps between spaces
      steps += ::abs(to - from);

      if (to_room != -1) {
        // steps to emplace in the room
        steps += (next.roomDepth - next.rooms[to_room].size());
        next.rooms[to_room] = fish + next.rooms[to_room];
      } else {
        next.hallway[to] = fish;
      }

      next.energy += steps * EnergyCost[fish];

      return next;
    }

    bool isHallwayClear(const int from, const int to) const {
      assert(std::min(from, to) == from);
      assert(std::max(from, to) == to);

      for (int k = from; k < to; k++) {
        if (isDoorway(k)) { // nothing can be directly above a room
          continue;
        }
        // if a hallway spot is occupied, no bueno
        if (hallway[k] != '.') {
          return false;
        }
      }

      return true;
    }

    bool isTargetRoomClear(const int to) const {
      assert(IsRoom(to));
      if (rooms[to].size() == static_cast<size_t>(roomDepth)) { // room is full
        return false;
      }

      for (const auto& c : rooms[to]) { // room has non-correct occupant
        if (!isDesiredRoom(c, to)) {
          return false;
        }
      }

      return true;
    }

    bool canMoveToHall(const int from, const int to) const {
      assert(IsRoom(from));
      assert(to >= 0 && to < HallwaySize);
      const int door_from = doorways[from];
      const int i = std::min(door_from, to);
      const int j = std::max(door_from, to);

      if (isDoorway(to)) {
        return false;
      }

      return isHallwayClear(i, j);
    }

    bool canMoveToRoom(const int from, const int to) const {
      assert(IsRoom(from));
      assert(IsRoom(to));
      const int door_from = doorways[from];
      const int door_to = doorways[to];
      const int i = std::min(door_from, door_to);
      const int j = std::max(door_from, door_to);

      return isTargetRoomClear(to) && isHallwayClear(i, j);
    }

    bool canMoveFromHall(const int from, const int to) const {
      assert(from >= 0 && from < HallwaySize);
      assert(IsRoom(to));
      const int door_to = doorways[to];
      const int i = std::min(from, door_to);
      const int j = std::max(from, door_to);

      return isTargetRoomClear(to) && isHallwayClear(i + 1, j);
    }

    bool valid() const {
      int fish = 0;
      for (const auto& r : rooms) {
        fish += r.size();

        if (r.size() > static_cast<size_t>(roomDepth)) {
          return false;
        }
      }
      for (const auto& c : hallway) {
        fish += IsFish(c);
      }

      return fish == roomDepth * RoomCount;
    }

    friend std::ostream& operator<<(std::ostream& os, const MapState& m) {
      os << aoc::cls;

      os << "#";
      for (size_t i = 0; i < m.hallway.size(); i++) {
        os << "#";
      }
      os << "#";
      os << std::endl;
      os << "#";
      for (const auto& c : m.hallway) {
        os << c;
      }
      os << "#";
      os << std::endl;
      os << "#";
      for (size_t i = 0; i < m.hallway.size(); i++) {
          if (m.isDoorway(i)) {
            os << '.';
          } else {
            os << "#";
          }
      }
      os << "#";

      return os;
    }
  };

  struct HeapComparator {
    bool operator()(const MapState& lhs, const MapState& rhs) {
      return lhs.energy > rhs.energy;
    }
  };

  using StateList = std::vector<MapState>;

  const auto HeapPop = [](auto& heap) {
    auto s = heap.front();
    std::pop_heap(heap.begin(), heap.end(), HeapComparator());
    heap.pop_back();

    return s;
  };

  const auto HeapPush = [](auto& heap, auto& s) {
    heap.emplace_back(s);
    std::push_heap(heap.begin(), heap.end(), HeapComparator());
  };

  int solve(MapState map) {
    aoc::AutoTimer __t;
    
    // Maintain a heap of states
    StateList sq;
    sq.push_back(map);
    std::make_heap(sq.begin(), sq.end(), HeapComparator());

    // Maintain a list of visited states, so we can quickly eliminate them from our A*
    int result = INT_MAX;
    std::set<std::string> seen;

    while (result == INT_MAX && !sq.empty()) {
      auto s = HeapPop(sq);

      const auto r = seen.emplace(s.hash());
      if (!r.second) {
        continue;
      }

      if (s.final()) {
        result = s.energy;
        break;
      }

      for (int i = 0; i < RoomCount; i++) {
        if (!s.isRoomFinal(i)) {
          const auto fish = s.rooms[i].front();
          if (!IsFish(fish)) { continue; }

          const auto t = TargetRoom(fish);
          if (s.canMoveToRoom(i, t)) {
            const auto next = s.move(s.doorways[i], s.doorways[t]);
            assert(next.valid());
            HeapPush(sq, next);
          }
        }

        for (int j = 0; j < HallwaySize; j++) {
          const auto fish = s.rooms[i].front();
          if (!IsFish(fish)) {
            continue;
          }

          const auto tf = s.hallway[j];
          if (IsFish(tf)) {
            continue;
          }

          if (s.canMoveToHall(i, j)) {
            const auto next = s.move(s.doorways[i], j);
            assert(next.valid());

            HeapPush(sq, next);
          }
        }
      }

      for (int j = 0; j < HallwaySize; j++) {
        const char fish = s.hallway[j];
        if (!IsFish(fish)) {
          continue;
        }
        const auto t = TargetRoom(fish);
        if (s.canMoveFromHall(j, t)) {
          const auto next = s.move(j, s.doorways[t]);
          assert(next.valid());

          HeapPush(sq, next);
        }
      }
    }

    return result;
  }
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);
  MapState map;
  std::string line;
  while (aoc::getline(f, line)) {
    constexpr int MinLineSize = 2 + (RoomCount * 2);
    constexpr int RoomOffset = 3;
    assert(line.size() > RoomOffset);
    const char c = line[RoomOffset];
    assert(c == '#' || c == '.' || (c >= 'A' && c <= 'D'));
    if (c < 'A' || c > 'D') {
      continue;
    }
    assert(line.size() > MinLineSize);
    int room = 0;
    for (int i = RoomOffset; i < MinLineSize; i += 2) {
      map.rooms[room].push_back(line[i]);
      map.doorways.push_back(i - 1);
      room++;
    }
  }
  f.close();

  const auto part1 = solve(map);
  aoc::print_result(1, part1);

  map.insertFish({ "DD", "CB", "BA", "AC" });

  const auto part2 = solve(map);
  aoc::print_result(2, part2);

  return 0;
}

