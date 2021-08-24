#include "Algorithms.hpp"

namespace rmq {

KMSRmq::KMSRmq(vector<ui> const &vals, size_t nuniq)
    : vals_(vals), /*nuniq_(nuniq),*/ bm_(vals, nuniq), fr_(vals, nuniq) {}

FreqValue
KMSRmq::query(size_t first, size_t last) const noexcept {
  assert(first < last);
  assert(last <= vals_.size());
  FreqValue mode{0, 0};
  if (last - first < bm_.bsize) {
    for (; first < last; ++first) {
      ui const val{vals_[first]};
      size_t const freq{fr_.getNumOccurances(val, first, last)};
      if (freq > mode.freq) {
        mode = {freq, val};
      }
    }
    return mode;
  }

  size_t const bf{first / bm_.bsize};
  size_t const bl{last / bm_.bsize};
  assert(bf < bl);
  if (bf + 1 < bl) {
    mode = bm_(bf + 1, bl);
  }

  size_t prefix{(bf + 1) * bm_.bsize};
  for (size_t i{first}; i < prefix; ++i) {
    ui const val{vals_[i]};
    size_t const freq{fr_.getNumOccurances(val, i, last)};
    if (freq > mode.freq) {
      mode = {freq, val};
    }
  }

  size_t const suffix{bl * bm_.bsize};
  for (size_t i{suffix}; i < last; ++i) {
    ui const val{vals_[i]};
    size_t const freq{fr_.getNumOccurances(val, first, i + 1)};
    if (freq > mode.freq) {
      mode = {freq, val};
    }
  }

  return mode;
}
} // namespace rmq
