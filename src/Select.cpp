#include "Select.hpp"
#include "Math.hpp"

namespace rmq {
template <bool Bit>
SelectTablePacked<Bit>::SelectTablePacked(BitArray const &ba, size_t nones)
    : pos_(nones != size_t(-1) ? nones : count_ba<Bit>(ba),
           traits::hi_bit(ba.size()) - is_pow2(ba.size())) {
  for (size_t idx{0}, i{0}; i < ba.size(); ++i) {
    if (ba[i] == Bit) {
      pos_[idx++] = i;
    }
  }
}

template <bool Bit>
size_t
SelectTablePacked<Bit>::select(size_t i) const noexcept {
  assert(0 < i);
  assert(i <= pos_.size());
  return pos_[i - 1];
}

template struct SelectTablePacked<false>;
template struct SelectTablePacked<true>;

template <bool Bit>
Select<Bit>::Select(BitArray const &ba, Rank9 const &ba_rank)
    : ba_(ba), summary_(make_summary_ba<Bit>(ba_)),
      delimiter_(make_delimiter_ba<Bit>(ba_, ba_rank.rank<Bit>(ba.size()))),
      ba_rank_(ba_rank), summ_sel1_(summary_), deli_rank_(delimiter_) {}

template <bool Bit>
size_t
Select<Bit>::select(size_t idx) const noexcept {
  assert(0 < idx);
  assert(idx <= delimiter_.size());
  size_t const numNonEmpty{deli_rank_.rank<1>(idx)};
  size_t const numBlock{summ_sel1_.select(numNonEmpty)};
  size_t const blockPos{traits::bits * numBlock};
  size_t const numOnes{ba_rank_.rank<Bit>(blockPos)};
  Int const val{ba_.data()[numBlock]};
  return blockPos + traits::select<Bit>(val, idx - numOnes);
}

template struct Select<false>;
template struct Select<true>;

} // namespace rmq
