#include "Algorithms.hpp"
#include "Math.hpp"

namespace rmq {
template <typename BlockMode>
CDLMWRmq<BlockMode>::CDLMWRmq(vector<ui> const &vals, size_t nuniq)
    : vals_(vals), nuniq_(nuniq), fr_(vals, nuniq), bm_(vals, nuniq, fr_) {}
template <typename BlockMode>

FreqValue
CDLMWRmq<BlockMode>::query(size_t first, size_t last) const noexcept {
  size_t const bf = div_ceil(first, bm_.bsize);
  size_t const bl = max(bf, last / bm_.bsize);
  size_t const prefix = min(bf * bm_.bsize, last);
  size_t const suffix = max(bl * bm_.bsize, first);
  FreqValue mode{bf < bl ? bm_(bf, bl) : FreqValue{0, 0}};

  for (size_t i{first}; i < prefix; ++i) {
    ui const value{vals_[i]};
    size_t const size{fr_.size(value)};
    size_t const rnk{fr_(i)};
    size_t cur{min(size, rnk + mode.freq)};
    for (; cur < size && fr_(value, cur) < last; ++cur)
      ;
    if (size_t const freq = cur - rnk; freq > mode.freq) {
      mode = {freq, value};
    }
  }

  for (size_t i{suffix}; i < last; ++i) {
    ui const value{vals_[i]};
    size_t const rnk{fr_(i)};
    size_t cur{rnk > mode.freq ? rnk - mode.freq + 1 : 1};
    while (cur-- > 0 && fr_(value, cur) >= first)
      ;
    if (size_t const freq = rnk - cur; freq > mode.freq) {
      mode = {freq, value};
    }
  }

  return mode;
}

template struct CDLMWRmq<BlockModeTable>;
template struct CDLMWRmq<BlockModeUnary>;
} // namespace rmq
