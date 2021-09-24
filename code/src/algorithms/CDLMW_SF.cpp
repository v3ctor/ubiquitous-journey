#include "Algorithms.hpp"
#include "Math.hpp"
#include "RSReduction.hpp"

#include <algorithm>
#include <cmath>

namespace rmq {
detail::CountSplitData
count_split(vector<ui> const &vals, size_t nuniq, size_t threshold) {
  vector<size_t> count(nuniq);
  size_t greater{0};
  for (auto const val : vals) {
    greater += (++count[val] == threshold + 1);
  }

  detail::CountSplitData data{};
  data[0].vals.reserve(vals.size() - greater);
  data[1].vals.reserve(greater);

  for (auto const val : vals) {
    data[count[val] > threshold].vals.push_back(val);
  }

  for (size_t i{0}; i < 2; ++i) {
    data[i].I.resize(vals.size(), vals.size() + 1);
    data[i].J.resize(vals.size());
  }

  ui itmp[2]{0, 0}, seen[2]{0, 0};
  for (size_t i{0}; i < vals.size(); ++i) {
    ui const val{vals[i]};
    size_t const cmp{count[val] > threshold};

    for (size_t j{itmp[cmp]}; j <= i; ++j) {
      data[cmp].I[j] = seen[cmp];
    }
    itmp[cmp] = i + 1;
    ++seen[cmp];
    for (size_t j{0}; j < 2; ++j) {
      data[j].J[i] = seen[j];
    }
  }

  for (size_t i{0}; i < 2; ++i) {
    if (data[i].vals.empty()) {
      data[i].vals = {0};
      data[i].rev_map = {0};
    } else {
      data[i].rev_map = rank_space_reduction(data[i].vals, nuniq);
    }
  }

  return data;
}

CDLMW_SFRmq::CDLMW_SFRmq(vector<ui> const &vals, size_t nuniq)
    : data_(count_split(
          vals, nuniq,
          ceill(sqrtl(BitArray::traits::bits * vals.size())))),
      cdlmw_(data_[0].vals, data_[0].rev_map.size()),
      sf_(data_[1].vals, data_[1].rev_map.size()) {}

FreqValue
CDLMW_SFRmq::query(size_t first, size_t last) const noexcept {
  assert(first < last);
  assert(last <= data_[0].vals.size() + data_[1].vals.size());
  --last;

  FreqValue mode{0, 0};

  {
    size_t const f{data_[0].I[first]}, l{data_[0].J[last]};
    if (f < l) {
      FreqValue const inner{cdlmw_.query(f, l)};
      if (inner.freq > mode.freq) {
        mode = {inner.freq, data_[0].rev_map[inner.value]};
      }
    }
  }

  {
    size_t const f{data_[1].I[first]}, l{data_[1].J[last]};
    if (f < l) {
      FreqValue const inner{sf_.query(f, l)};
      if (inner.freq > mode.freq) {
        mode = {inner.freq, data_[1].rev_map[inner.value]};
      }
    }
  }

  return mode;
}

} // namespace rmq