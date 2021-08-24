#include "Algorithms.hpp"

#include <algorithm>
#include <cmath>
#include <list>
#include <set>

namespace rmq {
template <typename MultiMode>
MoRmq<MultiMode>::MoRmq(vector<ui> const &vals, size_t nuniq)
    : vals_(vals), nuniq_(nuniq) {}

template <typename MultiMode>
void
MoRmq<MultiMode>::query(vector<MoQuery> &&queries, vector<FreqValue> &results) {
  long double const _bsize{log2l(vals_.size() / sqrtl(queries.size()))};
  size_t const bsize{max<size_t>(1, floor(_bsize))};

  sort(queries.begin(), queries.end(),
       [bsize](MoQuery const &lhs, MoQuery const &rhs) {
         size_t const lfb_idx{lhs.first >> bsize};
         return lfb_idx != rhs.first >> bsize ? lhs.first < rhs.first
                : lfb_idx & 1                 ? lhs.last > rhs.last
                                              : lhs.last < rhs.last;
       });

  size_t first{0}, last{0};
  MultiMode mm(vals_.size(), nuniq_);

  for (MoQuery query : queries) {
    while (first > query.first) {
      mm.insert(vals_[--first]);
    }
    while (last > query.last) {
      mm.remove(vals_[--last]);
    }
    while (last < query.last) {
      mm.insert(vals_[last++]);
    }
    while (first < query.first) {
      mm.remove(vals_[first++]);
    }
    results[query.idx] = mm.query();
  }
}

template struct MoRmq<MultiModeBST>;
template struct MoRmq<MultiModeList>;
} // namespace rmq
