#include "aoc21/helpers.h"
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <memory>

namespace {
  using NodeList = std::vector<std::string>;
  using Graph = std::map<std::string, NodeList, std::less<>>;
  using VisitedSet = std::vector<std::string>;

  constexpr std::string_view Start("start");
  constexpr std::string_view End("end");

  bool has_no_duplicates(VisitedSet v, const size_t max_small) {
    std::sort(v.begin(), v.end());

    VisitedSet::iterator it = v.begin();
    size_t count = 0;
    while (it != v.end() && count < max_small) {
      it = std::adjacent_find(it, v.end());
      if (it != v.end()) {
        count++;
        v.erase(it);
        it = v.begin();
      }
    }

    return count < max_small;
  }

  bool can_visit(const VisitedSet& v, std::string_view c, const size_t max_small) {
    if (c == Start) {
      return false;
    }
    if (!std::islower(c[0])) {
      return true;
    }

    if (std::find(v.begin(), v.end(), c) == v.end()) {
      return true;
    }

    return has_no_duplicates(v, max_small);
  }

  void walk_paths(const Graph& graph, VisitedSet& v, std::string_view c, size_t& paths, const size_t max_small) {
    DEBUG_PRINT(c);
    if (c == End) {
      paths++;
      return;
    }

    const auto cn = graph.find(c);
    assert(cn != graph.end());

    for (const auto& n: cn->second) {
      if (!can_visit(v, n, max_small)) {
        continue;
      }

      if (std::islower(n[0])) {
        v.push_back(n);
      }

      walk_paths(graph, v, n, paths, max_small);

      if (std::islower(n[0])) {
        v.pop_back();
      }
    }
  }
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  Graph graph;

  std::string s;
  while (aoc::getline(f, s)) {
    const std::string_view sv(s);
    const auto p = sv.find('-');
    assert(p != std::string_view::npos);

    auto n1 = std::string(sv.substr(0, p));
    auto n2 = std::string(sv.substr(p + 1));

    NodeList children{};
    auto r = graph.emplace(std::pair<std::string, NodeList>(n1, children));
    r.first->second.emplace_back(n2);
    r = graph.emplace(std::pair<std::string, NodeList>(n2, children));
    r.first->second.emplace_back(n1);
  }

  {
    size_t paths = 0;
    VisitedSet v{};
    walk_paths(graph, v, Start, paths, 0);
    std::cout << "Part 1: " << paths << std::endl;
  }

  {
    size_t paths = 0;
    VisitedSet v{};
    walk_paths(graph, v, Start, paths, 1);
    std::cout << "Part 2: " << paths << std::endl;
  }

  return 0;
}

