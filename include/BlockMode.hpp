#ifndef BLOCK_MODE_HPP_INCLUDED
#define BLOCK_MODE_HPP_INCLUDED

#include "BitArray.hpp"
#include "FreqValue.hpp"
#include "FrequencyTable.hpp"
#include "Rank9.hpp"
#include "Select.hpp"

#include <vector>

namespace rmq {
using namespace std;
struct BlockModeTable {
  BlockModeTable(vector<ui> const &vals, size_t nuniq);
  BlockModeTable(vector<ui> const &vals, size_t nuniq,
                 FrequencyRankTable const &);
  FreqValue operator()(size_t i, size_t j) const noexcept;

public:
  size_t const nuniq, bsize, nblocks;

private:
  // vector<ui> const &vals_;
  vector<vector<FreqValue>> blocks_;
};

struct BlockModeUnary {
  BlockModeUnary(vector<ui> const &vals, size_t nuniq,
                 FrequencyRankTable const &fr);
  FreqValue operator()(size_t i, size_t j) const noexcept;

public:
  size_t const nuniq, bsize, nblocks;

private:
  vector<ui> const &vals_;
  FrequencyRankTable const &fr_;
  vector<BitArray> blocks_;
  vector<Rank9> rank_;
  vector<Select<0>> sel0_;
  vector<Select<1>> sel1_;
};
} // namespace rmq

#endif
