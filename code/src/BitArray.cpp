#include "BitArray.hpp"
#include "Math.hpp"

namespace rmq {

BitArray::ref &
BitArray::ref::operator=(bool b) noexcept {
  *ptr &= ~(Int(1) << off);
  *ptr |= (Int(b) << off);
  return *this;
}

BitArray::ref::operator bool() const noexcept {
  return *ptr >> off & 1;
}

BitArray::BitArray(size_t nbits)
    : nbits_(nbits), mem_(div_ceil(nbits_, traits::bits)) {}

BitArray::ref
BitArray::operator[](size_t i) noexcept {
  Int *const ptr{&mem_.data()[i / traits::bits]};
  size_t const off{i % traits::bits};
  return ref{ptr, off};
}

bool
BitArray::operator[](size_t i) const noexcept {
  Int const val{mem_.data()[i / traits::bits]};
  size_t const off{i % traits::bits};
  return val >> off & 1;
}

BitArray::Int *
BitArray::data() {
  return mem_.data();
}
BitArray::Int const *
BitArray::data() const {
  return mem_.data();
}

size_t
BitArray::size() const noexcept {
  return nbits_;
}

template <bool Bit>
BitArray
make_summary_ba(BitArray const &ba) {
  using traits = BitArray::traits;
  BitArray summary(div_ceil(ba.size(), traits::bits));
  auto bait{ba.data()};
  for (size_t i{0}; i < summary.size(); ++i) {
    summary[i] = (traits::rank<Bit>(*bait++) > 0);
  }
  return summary;
}

template BitArray make_summary_ba<false>(BitArray const &ba);
template BitArray make_summary_ba<true>(BitArray const &ba);

template <bool Bit>
BitArray
make_delimiter_ba(BitArray const &ba, size_t nones) {
  using traits = BitArray::traits;
  size_t const size{div_ceil(ba.size(), traits::bits)};
  assert(count_ba<Bit>(ba) == nones);

  BitArray delimiter(nones);
  auto bait{ba.data()}, baend{bait + size};
  for (size_t didx{0}; didx < nones && bait != baend; ++bait) {
    if (size_t const rnk{traits::rank<Bit>(*bait)}; rnk) {
      delimiter[didx] = 1;
      didx += rnk;
    }
  }
  return delimiter;
}

template <bool Bit>
BitArray
make_delimiter_ba(BitArray const &ba) {
  using traits = BitArray::traits;
  size_t const size{div_ceil(ba.size(), traits::bits)};

  size_t nones{0};
  auto bait{ba.data()}, baend{bait + size};
  while (bait != baend) {
    nones += traits::rank<Bit>(*bait++);
  }

  return make_delimiter_ba<Bit>(ba, nones);
}

template BitArray make_delimiter_ba<false>(BitArray const &ba,
                                           size_t nones);
template BitArray make_delimiter_ba<true>(BitArray const &ba,
                                          size_t nones);
template BitArray make_delimiter_ba<false>(BitArray const &ba);
template BitArray make_delimiter_ba<true>(BitArray const &ba);

template <bool Bit>
size_t
count_ba(BitArray const &ba) noexcept {
  using traits = BitArray::traits;

  size_t const lo_size{ba.size() / traits::bits};
  size_t count{0};
  auto bait{ba.data()}, baend{bait + lo_size};
  for (; bait != baend; ++bait) {
    count += traits::rank<Bit>(*bait);
  }

  if (size_t const offset{ba.size() % traits::bits}; offset) {
    count += traits::rank<Bit>(*bait, offset);
  }

  return count;
}
template size_t count_ba<false>(BitArray const &ba);
template size_t count_ba<true>(BitArray const &ba);

} // namespace rmq
