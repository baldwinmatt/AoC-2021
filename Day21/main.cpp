#include "aoc21/helpers.h"
#include <array>
#include <vector>
#include <map>

namespace {

  using Player = std::pair<int, int>;

  class Die {
  protected:
    int value;
    size_t rolls;
    const int sides;

  public:
    Die()
      : value(1)
      , rolls(0)
      , sides(100)
    { }

    int roll() {
      const auto r = value;
      value++;
      if (value > sides) { value -= sides; }
      rolls++;
      return r;
    }

    size_t get_rolls() const {
      return rolls;
    }
  };

  using State = std::tuple<int, int, int, int>;
  // Pair of Outcome + Ways to get there from 3 rolls
  const std::vector<std::pair<int, int64_t>> QuantumRolls{
    {6,7}, {5,6}, {7,6}, {4,3}, {8,3}, {3,1}, {9,1},
  };

  // map of State (finite) -> wins
  using Universe = std::map<State, int64_t>;
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::array<Player, 2> players{ };
  {
    size_t i = 0;
    std::string line;
    while (i < 2 && aoc::getline(f, line)) {
      const auto p = line.size() - 1;
      assert(line[p] >= '1' && line[p] <= '9');
      const int pos = line[p] - '0';
      players[i].first = pos;
      players[i].second = 0;
      i++;
    }

    assert(i == 2);
  }

  // Initial state for part 2
  Universe u{
    { std::make_tuple(players[0].first, players[0].second, players[1].first, players[1].second), 1 }
  };

  bool turn = 0;
  Die die{};
  while (players[0].second < 1000 &&
      players[1].second < 1000)
  {
    const auto move = die.roll() + die.roll() + die.roll();
    players[turn].first += move;
    while (players[turn].first > 10) { players[turn].first -= 10; }
    players[turn].second += players[turn].first;

    DEBUG_PRINT("Player " << (static_cast<int>(turn) + 1) << " Score: " << players[turn].second << " Space: " << players[turn].first);
    turn = !turn;
  }

  aoc::print_result(1, die.get_rolls() * players[turn].second);

  // Generate all possible set of outcomes - this is limited, and for each one, either 1 or 2 can win
  std::pair<int64_t, int64_t>wins{0, 0};

  // universe is a map of all non-terminal states with their frequencies, we loop until there are no non-terminal states
  while (!u.empty()) {
    DEBUG_PRINT("Non-terminal states: " << u.size());
    Universe t;

    for (auto& [state, count] : u) {
      // unpack the state
      std::tie(players[0].first, players[0].second, players[1].first, players[1].second) = state;
      for (const auto& [ roll1, ways1 ] : QuantumRolls) {
        // advance player 1
        Player p1(players[0]);
        p1.first = p1.first + roll1;
        while (p1.first > 10) { p1.first -= 10; }
        p1.second += p1.first;
        int64_t outcomes = count * ways1;
        if (p1.second >= 21) { // player 1 wins
          wins.first += outcomes;
          continue;
        }

        for (const auto& [ roll2, ways2 ] : QuantumRolls) {
          // advance player 2
          Player p2(players[1]);
          p2.first = p2.first + roll2;
          while (p2.first > 10) { p2.first -= 10; }
          p2.second += p2.first;
          if (p2.second >= 21) { // player 2 wins
            wins.second += outcomes * ways2;
            continue;
          }

          // else, new state
          auto r = t.emplace(std::make_tuple(p1.first, p1.second, p2.first, p2.second), 0);
          r.first->second += outcomes * ways2;
        }
      }
    }

    std::swap(t, u);
  }

  aoc::print_result(2, std::max(wins.first, wins.second));

  return 0;
}

