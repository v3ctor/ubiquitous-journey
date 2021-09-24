#include "Rank9.hpp"
#include "Math.hpp"

namespace rmq {
Rank9::Rank9(Rank9::Int const *bits, size_t size)
    : bits_(bits),
      blks_(div_ceil<size_t>(size, 8) + (size % 8 == 0)) {
  auto bitit{bits_};
  auto blkit{blks_.data()};
  block blk{0, 0};

  size_t idx{0}, pcnt{0};
  for (; idx + 7 < size; idx += 8) {
    blk.rel = pcnt = 0;
    for (size_t i{0}; i < 7; ++i) {
      pcnt += traits::rank<1>(*bitit++);
      blk.rel |= pcnt << shift[i];
    }
    pcnt += traits::rank<1>(*bitit++);
    *blkit++ = blk;
    blk.cum += pcnt;
  }

  size -= idx;
  for (blk.rel = idx = pcnt = 0; idx < size; ++idx) {
    pcnt += traits::rank<1>(*bitit++);
    blk.rel |= pcnt << shift[idx];
  }
  *blkit++ = blk;
}

Rank9::Rank9(BitArray const &ba)
    : Rank9(ba.data(), div_ceil(ba.size(), traits::bits)) {}

template <bool Bit>
size_t
Rank9::rank(size_t idx) const noexcept {
  if constexpr (Bit) {
    return _rank1(idx);
  } else {
    return idx - _rank1(idx);
  }
}

template size_t Rank9::rank<false>(size_t idx) const noexcept;
template size_t Rank9::rank<true>(size_t idx) const noexcept;

size_t
Rank9::_rank1(size_t idx) const noexcept {
  auto const [b_rnk, b_rel] = blks_[idx / 512];
  size_t const sb_shift{shift[(idx % 512 / 64 - 1) % 8]};
  size_t const sb_rnk{(b_rel >> sb_shift) % 512};
  if (size_t const off{idx % 64}; off) {
    return b_rnk + sb_rnk + traits::rank<1>(bits_[idx / 64], off);
  }
  return b_rnk + sb_rnk;
}

} // namespace rmq
