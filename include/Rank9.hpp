#ifndef RANK9_HPP_INCLUDED
#define RANK9_HPP_INCLUDED

#include "BitArray.hpp"

#include <cstddef>
#include <vector>

namespace rmq {
struct Rank9 {
  using traits = BitArray::traits;
  using Int = traits::type;
  struct block {
    Int cum, rel;
  };

  static constexpr auto shift{[] {
    array<size_t, 9> shift{0};
    for (size_t i{1}; i <= 8; ++i) {
      shift[i] = shift[i - 1] + 9;
    }
    return shift;
  }()};

  Rank9(Int const *bits, size_t size);
  Rank9(BitArray const &ba);

  template <bool Bit>
  size_t rank(size_t idx) const noexcept;

private:
  size_t _rank1(size_t idx) const noexcept;

  Int const *bits_;
  vector<block> blks_;
};

} // namespace rmq

#endif
