#ifndef MULTI_MODE_HPP_INCLUDED
#define MULTI_MODE_HPP_INCLUDED

#include "FreqValue.hpp"

#include <list>
#include <set>
#include <vector>

namespace rmq {
using namespace std;
struct MultiModeBST {
  MultiModeBST(size_t size, size_t nuniq);
  void insert(ui val) noexcept;
  void remove(ui val) noexcept;
  FreqValue query() const noexcept;

private:
  set<pair<size_t, ui>> modeSet_;
  vector<size_t> freqTbl_;
};

struct MultiModeList {
  MultiModeList(size_t size, size_t nuniq);

  struct iter {
    size_t freq;
    list<ui>::iterator vit;
  };

  void insert(ui val) noexcept;
  void remove(ui val) noexcept;
  FreqValue query() const noexcept;

private:
  vector<list<ui>> layers_;
  vector<iter> iters_;
  size_t const nuniq_;
  size_t mfreq_;
};
} // namespace rmq

#endif
