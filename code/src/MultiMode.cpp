#include "MultiMode.hpp"

#include <assert.h>

namespace rmq {
MultiModeBST::MultiModeBST(size_t /*size*/, size_t nuniq)
    : freqTbl_(nuniq) {
  modeSet_.insert({0, 0});
}

void
MultiModeBST::insert(ui val) noexcept {
  size_t const freq{freqTbl_[val]++};
  if (freq) {
    modeSet_.erase(modeSet_.find({freq, val}));
  }
  modeSet_.insert({freq + 1, val});
}

void
MultiModeBST::remove(ui val) noexcept {
  size_t const freq{--freqTbl_[val]};
  modeSet_.erase(modeSet_.find({freq + 1, val}));
  if (freq) {
    modeSet_.insert({freq, val});
  }
}

FreqValue
MultiModeBST::query() const noexcept {
  auto const [freq, value] = *modeSet_.rbegin();
  return {freq, value};
}

MultiModeList::MultiModeList(size_t size, size_t nuniq)
    : layers_(size), iters_(nuniq, {0, {}}), /*nuniq_(nuniq),*/
      mfreq_(0) {}
struct layer {
  size_t freq;
  list<ui> vals;
};

void
MultiModeList::insert(ui val) noexcept {
  auto &iter{iters_[val]};
  auto &layer{layers_[iter.freq]};
  layer.push_front(val);
  if (iter.freq) {
    layers_[iter.freq - 1].erase(iter.vit);
  }
  ++iter.freq;
  mfreq_ = max<size_t>(mfreq_, iter.freq);
  iter.vit = layer.begin();
}

void
MultiModeList::remove(ui val) noexcept {
  auto &iter{iters_[val]};
  assert(iter.freq);
  auto &layer{layers_[iter.freq - 1]};
  layer.erase(iter.vit);
  mfreq_ -= (iter.freq == mfreq_) && layer.empty();
  if (--iter.freq) {
    layers_[iter.freq - 1].push_front(val);
    iter.vit = layers_[iter.freq - 1].begin();
  }
}

FreqValue
MultiModeList::query() const noexcept {
  return (mfreq_ ? FreqValue{mfreq_, layers_[mfreq_ - 1].front()}
                 : FreqValue{0, 0});
}
} // namespace rmq
