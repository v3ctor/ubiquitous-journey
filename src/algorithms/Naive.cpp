#include "Algorithms.hpp"

#include <unordered_map>

namespace rmq {
NaiveRmq::NaiveRmq(vector<ui> const &vals, size_t /* nuniq*/)
    : vals_(vals) /*, nuniq_(nuniq)*/ {}

FreqValue
NaiveRmq::query(size_t first, size_t last) const noexcept {
  assert(first < last);
  assert(last <= vals_.size());

  FreqValue mode{0, 0};
  unordered_map<ui, size_t> map;
  for (; first < last; ++first) {
    ui const val{vals_[first]};
    if (size_t const freq{++map[val]}; freq > mode.freq) {
      mode = {freq, val};
    }
  }

  return mode;
}

NaiveFreqTblRmq::NaiveFreqTblRmq(vector<ui> const &vals, size_t nuniq)
    : vals_(vals), /*nuniq_(nuniq),*/ freqTbl_(nuniq) {}

FreqValue
NaiveFreqTblRmq::query(size_t first, size_t last) const noexcept {
  assert(first < last);
  assert(last <= vals_.size());

  FreqValue mode{0, 0};
  for (size_t i{first}; i < last; ++i) {
    ui const val{vals_[i]};
    if (size_t const freq{++freqTbl_[val]}; freq > mode.freq) {
      mode = {freq, val};
    }
  }

  for (; first < last; ++first) {
    ui const val{vals_[first]};
    freqTbl_[val] = 0;
  }

  return mode;
}
} // namespace rmq
