#include "RSReduction.hpp"

#include <unordered_map>

namespace rmq {
vector<ui>
rank_space_reduction(vector<ui> &vals) {
  unordered_map<ui, ui> map;
  for (auto &val : vals) {
    if (auto const found{map.find(val)}; found != map.end()) {
      val = found->second;
    } else {
      val = map[val] = map.size();
    }
  }

  vector<ui> rev_map(map.size());
  for (auto const [from, to] : map) {
    rev_map[to] = from;
  }
  return rev_map;
}

vector<ui>
rank_space_reduction(vector<ui> &vals, size_t nuniq) {
  vector<ui> map(nuniq, nuniq);
  ui counter{0};
  for (ui &val : vals) {
    if (ui const found{map[val]}; found != nuniq) {
      val = found;
    } else {
      val = map[val] = counter++;
    }
  }

  vector<ui> rev_map(counter);
  for (size_t i{0}; i < map.size(); ++i) {
    if (map[i] != nuniq) {
      rev_map[map[i]] = i;
    }
  }

  return rev_map;
}
} // namespace rmq
