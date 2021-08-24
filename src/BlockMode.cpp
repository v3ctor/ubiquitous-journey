#include "BlockMode.hpp"
#include "Math.hpp"

#include <cmath>
#include <iostream>
namespace rmq {

BlockModeTable::BlockModeTable(vector<ui> const &vals, size_t nuniq)
    : nuniq(nuniq), bsize(ceill(sqrtl(vals.size()))),
      nblocks(div_ceil(vals.size(), bsize)), /*vals_(vals),*/
      blocks_(nblocks, vector<FreqValue>(nblocks)) {
  vector<size_t> count(nuniq);

  for (size_t bi{0}; bi < nblocks; ++bi) {
    fill(count.begin(), count.end(), 0);
    FreqValue mode{0, 0};

    size_t first{bi * bsize}, last{first};
    for (size_t bj{bi}; bj < nblocks; ++bj) {
      last = min(last + bsize, vals.size());
      for (; first < last; ++first) {
        ui const value{vals[first]};
        size_t const freq{++count[value]};
        if (freq > mode.freq) {
          mode = {freq, value};
        }
      }
      blocks_[bi][bj] = mode;
    }
  }
}

BlockModeTable::BlockModeTable(vector<ui> const &vals, size_t nuniq,
                               FrequencyRankTable const &)
    : BlockModeTable(vals, nuniq) {}

FreqValue
BlockModeTable::operator()(size_t i, size_t j) const noexcept {
  return blocks_[i][j - 1];
}

BlockModeUnary::BlockModeUnary(vector<ui> const &vals, size_t nuniq,
                               FrequencyRankTable const &fr)
    : nuniq(nuniq),
      bsize(ceill(sqrtl(vals.size()) / sqrtl(BitArray::traits::bits))),
      nblocks(div_ceil(vals.size(), bsize)), vals_(vals), fr_(fr) {
  blocks_.reserve(nblocks);
  rank_.reserve(nblocks);
  sel0_.reserve(nblocks);
  sel1_.reserve(nblocks);

  vector<size_t> count(nuniq), freqs(nblocks);
  for (size_t bi{0}; bi < nblocks; ++bi) {
    fill(count.begin(), count.end(), 0);
    size_t curr_freq{0}, first{bi * bsize}, last{first};
    for (size_t bj{bi}; bj < nblocks; ++bj) {
      last = min(last + bsize, vals.size());
      for (; first < last; ++first) {
        curr_freq = max(curr_freq, ++count[vals[first]]);
      }
      freqs[bj] = curr_freq;
    }

    blocks_.push_back(freqs[nblocks - 1] + (nblocks - bi));
    for (size_t bj{bi}; bj < nblocks; ++bj) {
      blocks_.back()[freqs[bj] + bj - bi] = 1;
    }

    // Rank9  -- ~0.25x overhead
    // Select -- ~2.0x overhead
    // Total  -- ~4.25x overhead
    rank_.push_back(Rank9(blocks_.back()));
    sel0_.push_back(Select<0>(blocks_.back(), rank_.back()));
    sel1_.push_back(Select<1>(blocks_.back(), rank_.back()));
  }
}

FreqValue
BlockModeUnary::operator()(size_t i, size_t j) const noexcept {
  size_t const first{i * bsize};
  size_t const pos_bj{sel1_[i].select(j - i)};
  // size_t const mode_freq{rank_[i].rank<0>(pos_bj)};
  size_t const mode_freq{pos_bj - (j - i - 1)};
  size_t const pos{sel0_[i].select(mode_freq)};
  // i + ((j - i) - (pos_bj - pos))
  // size_t const bl{i + rank_[i].rank<1>(pos)};
  size_t const bl{j + pos - pos_bj};
  size_t const bl_first{bl * bsize};
  size_t const bl_last{min(bl_first + bsize, vals_.size())};

  for (size_t k{bl_first}; k < bl_last; ++k) {
    ui const val{vals_[k]};
    size_t const rnk{fr_(k)};
    if (rnk + 1 >= mode_freq && fr_(val, rnk + 1 - mode_freq) >= first) {
      return {mode_freq, val};
    }
  }

  assert(false);
  return {0, 0};
}
} // namespace rmq
