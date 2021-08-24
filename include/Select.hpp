#ifndef SELECT_HPP_INCLUDED
#define SELECT_HPP_INCLUDED

#include "BitArray.hpp"
#include "PackedArray.hpp"
#include "Rank9.hpp"

namespace rmq {
template <bool Bit>
struct SelectTablePacked {
  using traits = BitArray::traits;
  SelectTablePacked(BitArray const &ba, size_t nones = -1);
  size_t select(size_t i) const noexcept;

private:
  PackedArray pos_;
};

template <bool Bit>
struct Select {
  using traits = BitArray::traits;
  using Int = traits::type;

  Select(BitArray const &ba, Rank9 const &ba_rank);
  size_t select(size_t idx) const noexcept;

private:
  BitArray const &ba_;
  BitArray summary_, delimiter_;
  Rank9 ba_rank_;
  SelectTablePacked<1> summ_sel1_;
  Rank9 deli_rank_;
};
} // namespace rmq

#endif
