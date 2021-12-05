#include "aoc21/helpers.h"

#include <vector>
#include <list>
#include <numeric>

namespace {
  using Square = std::pair<int, bool>;
  using Row = std::vector<Square>;
  using Board = std::vector<Row>;
  using BoardList = std::list<Board>;

  const auto IsBoardComplete = [](const Board& b) {
    // All visited in row
    for (const auto& row : b) {
      size_t sum = std::accumulate(row.begin(), row.end(), 0, [](size_t sum, auto& i) { sum += (i.second ? 1 : 0); return sum; });
      if (sum == row.size()) {
        return true;
      }
    }

    // Or all visited in each column
    size_t cols = b[0].size();
    for (size_t i = 0; i < cols; i++) {
      size_t col_sum = 0;
      for (size_t j = 0; j < b.size(); j++) {
        col_sum += (b[j][i].second ? 1 : 0);
      }
      if (col_sum == b.size()) {
        return true;
      }
    }

    return false;
  };

  const auto GetBoardScore = [](const Board& b) {
    size_t score = 0;
    for (const auto& row : b) {
      for (const auto& sq : row) {
        score += (!sq.second ? sq.first : 0);
      }
    }
    return score;
  };

  const auto PlayMove = [](Board& b, int num) {
    bool did_set = false;
    for (auto &row : b) {
      for (auto &sq : row) {
        if (sq.first == num) {
          did_set = !sq.second;
          sq.second = true;
        }
      }
    }
    return did_set;
  };

  const auto DisplayRow = [](const Row& row) {
    for (auto &sq : row) {
      if (sq.second) {
        std::cout << bold_on << sq.first << bold_off << " ";
      } else {
        std::cout << sq.first << " ";

      }
    }
    std::cout << std::endl;
  };

  const auto DisplayBoard = [](const Board& b) {
    for (auto &row : b) {
      DisplayRow(row);
    }
    std::cout << std::endl;
  };
};

int main(int argc, char** argv) {

  auto f = open_argv_1(argc, argv);

  // Parse the drawing order
  std::vector<int>bingo_drawer;
  int num = 0;
  std::string line;
  std::getline(f, line);
  for (const char c : line) {
    if (c == ',') {
      bingo_drawer.push_back(num);
      num = 0;
    } else if (c >= '0' || c <= '9') {
      num *= 10;
      num += (c - '0');
    }
  }
  bingo_drawer.push_back(num);

  // Parse the game boards
  BoardList boards;
  Board board;
  while (f.good()) {
    line.resize(0);
    std::getline(f, line);
    if (line.empty() && !board.empty()) {
      boards.emplace_back(std::move(board));
    } else if (!line.empty()) {
      Row row;
      row.emplace_back(0, false);
      size_t pos = 0;
      bool in_num = false;
      
      for (const char c : line) {
        if (c == ' ') {
          if (in_num) {
            row.emplace_back(0, false);
            pos++;
          }
          in_num = false;
        } else if (c >= '0' || c <= '9') {
          in_num = true;
          row[pos].first *= 10;
          row[pos].first += (c - '0');
        }
      }
      board.emplace_back(std::move(row));
    }
  }

  if (!board.empty()) {
      boards.emplace_back(std::move(board));
  }

  // Play games
  bool has_won = false;
  for (const auto move : bingo_drawer) {

    auto it = boards.begin();
    while (it != boards.end()) {
      if (PlayMove((*it), move) && IsBoardComplete(*it)) {
        const auto score = GetBoardScore(*it);
        if (!has_won) {
          DisplayBoard(*it);
          std::cout << "1st: called: " << move << " score: " << score << " result: " << (score * move) << std::endl;
          has_won = true;
        }
        if (boards.size() == 1) {
          DisplayBoard(*it);
          std::cout << "Last: called: " << move << " score: " << score << " result: " << (score * move) << std::endl;
          return 0;
        }
        auto tmp = it;
        it++;
        boards.erase(tmp);
      } else {
        it++;
      }
    }

  }

  return 0;
}

