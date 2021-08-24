#include "Algorithms.hpp"
#include "Math.hpp"
#include "UI.hpp"

#include <iostream>
using namespace std;

namespace rmq {

SFRmq::SFRmq(vector<ui> const &vals, size_t nuniq)
    : vals_(vals), nuniq_(nuniq),
      sf_mem_((1 + div_ceil(vals.size(), nuniq)) * nuniq), sf_(sf_mem_.data()) {
  auto sf_it{sf_ + nuniq};
  for (size_t i{0}; i < vals.size(); sf_it += nuniq) {
    copy(sf_it - nuniq, sf_it, sf_it);
    size_t const j{min(i + nuniq, vals.size())};
    for (; i < j; ++i) {
      ++sf_it[vals[i]];
    }
  }
}

FreqValue
SFRmq::query(size_t first, size_t last) const {
  assert(first < last);
  assert(last <= vals_.size());
  --last;

  size_t const fmod{first % nuniq_};
  size_t bf{first - fmod + nuniq_};
  size_t const flen{min(nuniq_, vals_.size() + nuniq_ - bf)};
  bool const bf_sub{2 * fmod + 2 < flen};
  bf -= (bf_sub * nuniq_);

  size_t const lmod{last % nuniq_};
  size_t bl{last - lmod + nuniq_};
  size_t const llen{min(nuniq_, vals_.size() + nuniq_ - bl)};
  bool const bl_add{2 * lmod + 2 < llen};
  bl -= (bl_add * nuniq_);

  vector<size_t> count_mem(nuniq_);
  size_t *count{count_mem.data()};

  if (bf != bl) {
    ui_sub(count, sf_ + bl, sf_ + bf, nuniq_);
  }

  if (bf_sub) {
    for (size_t i{0}; i < fmod; ++i) {
      --count[vals_[bf + i]];
    }
  } else {
    bf -= nuniq_;
    for (size_t i{fmod}; i < flen; ++i) {
      ++count[vals_[bf + i]];
    }
  }

  if (bl_add) {
    for (size_t i{0}; i <= lmod; ++i) {
      ++count[vals_[bl + i]];
    }
  } else {
    bl -= nuniq_;
    for (size_t i{lmod + 1}; i < llen; ++i) {
      --count[vals_[bl + i]];
    }
  }

  FreqValue mode{0, 0};
  for (size_t i{0}; i < nuniq_; ++i) {
    if (size_t const cnt{count[i]}; cnt > mode.freq) {
      mode = {cnt, ui(i)};
    }
  }

  return mode;
}
} // namespace rmq