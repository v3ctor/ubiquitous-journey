#include "FrequencyTable.hpp"
#include <assert.h>

namespace rmq {

FrequencyTable::FrequencyTable(vector<ui> const &vals, size_t nuniq)
    : vals_(vals), nuniq_(nuniq), base_(nuniq), count_(nuniq),
      freq_(vals.size()) {
  for (ui val : vals) {
    ++count_[val];
  }

  size_t offset{0};
  for (size_t i{0}; i < nuniq; ++i) {
    base_[i] = freq_.data() + offset;
    offset += count_[i];
  }

  fill(count_.begin(), count_.end(), 0);
  for (size_t i{0}; i < vals.size(); ++i) {
    ui const val{vals[i]};
    base_[val][count_[val]++] = i;
  }
}

size_t
FrequencyTable::operator()(ui val, size_t k) const noexcept {
  return base_[val][k];
}

size_t
FrequencyTable::getNumOccurances(ui val, size_t first,
                                 size_t last) const noexcept {
  assert(first < last);
  assert(last <= vals_.size());
  size_t const *b{base_[val]};
  size_t const len{count_[val]};
  auto const fit{lower_bound(b, b + len, first)};
  auto const lit{lower_bound(fit, b + len, last)};
  return size_t(lit - fit);
}

size_t
FrequencyTable::size(ui val) const noexcept {
  return count_[val];
}

FrequencyRankTable::FrequencyRankTable(vector<ui> const &vals, size_t nuniq)
    : FrequencyTable(vals, nuniq), rank_(vals.size()) {
  for (size_t i{0}; i < nuniq; ++i) {
    size_t const len{size(i)};
    for (size_t j{0}; j < len; ++j) {
      rank_[operator()(i, j)] = j;
    }
  }
}

size_t
FrequencyRankTable::operator()(size_t i) const noexcept {
  return rank_[i];
}
} // namespace rmq
