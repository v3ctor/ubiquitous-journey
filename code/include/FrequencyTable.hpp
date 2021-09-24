#ifndef FREQUENCY_TABLE_HPP_INCLUDED
#define FREQUENCY_TABLE_HPP_INCLUDED

#include "FreqValue.hpp"

#include <cstddef>
#include <vector>

namespace rmq {
using namespace std;
struct FrequencyTable {
  FrequencyTable(vector<ui> const &vals, size_t nuniq);

  size_t operator()(ui val, size_t k) const noexcept;
  size_t getNumOccurances(ui val, size_t first,
                          size_t last) const noexcept;
  size_t size(ui val) const noexcept;

private:
  // currently used only in asserts
  vector<ui> const &vals_ __attribute__((unused));
  size_t const nuniq_ __attribute__((unused));

  vector<size_t *> base_;
  vector<size_t> count_;
  vector<size_t> freq_;
};

struct FrequencyRankTable : FrequencyTable {
  using FrequencyTable::operator();

  FrequencyRankTable(vector<ui> const &vals, size_t nuniq);
  size_t operator()(size_t i) const noexcept;

private:
  vector<size_t> rank_;
};
} // namespace rmq

#endif
