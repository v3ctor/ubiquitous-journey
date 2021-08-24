#ifndef RSS_TRAITS_HPP_INCLUDED
#define RSS_TRAITS_HPP_INCLUDED

#include <array>
#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <immintrin.h>

namespace rmq {
using namespace std;

template <typename T>
struct RSSTraits;

template <>
struct RSSTraits<uint64_t> {
  using self = RSSTraits<uint64_t>;
  using type = uint64_t;

  static constexpr size_t bits{64};

  static constexpr auto lomsk{[] {
    array<size_t, bits + 1> msk{0};
    for (size_t i{1}; i <= bits; ++i) {
      msk[i] = 2 * msk[i - 1] + 1;
    }
    return msk;
  }()};

  template <bool Bit>
  static constexpr size_t rank(type x) noexcept {
    size_t const nones{size_t(__builtin_popcountll(x))};
    if constexpr (Bit) {
      return nones;
    } else {
      return bits - nones;
    }
  }

  static constexpr size_t hi_bit(type x) noexcept {
    size_t pos{0};
    for (; x; x /= 2) {
      ++pos;
    }
    return pos;
  }

  template <bool Bit>
  static constexpr size_t rank(type x, type i) noexcept {
    assert(0 < i);
    assert(i <= bits);
    size_t const nones{self::rank<1>(x << (bits - i))};
    if constexpr (Bit) {
      return nones;
    } else {
      return i - nones;
    }
  }

  template <bool Bit>
  static size_t select(type x, size_t i) noexcept {
    assert(0 < i);
    assert(i <= bits);
    if constexpr (!Bit) {
      x = ~x;
    }
    assert(self::rank<1>(x) >= i);
#ifdef __BMI2__
    return __builtin_ctzll(_pdep_u64(type(1) << (i - 1), x));
#else
#  warning "_pdep_u64 instruction is not supported."
    size_t pos{0};
    for (; i; x /= 2) {
      i -= x & 1;
      ++pos;
    }
    return pos - 1;
#endif
  }

  static void pack(type *mem, size_t offset, type val, size_t width) {
    assert(val <= lomsk[width]);
    mem[0] &= ~(lomsk[width] << offset);
    mem[0] |= val << offset;
    if (width + offset > bits) {
      mem[1] &= ~lomsk[width] >> (bits - offset);
      mem[1] |= val >> (bits - offset);
    }
  }

  static type unpack(type const *mem, size_t offset, size_t width) {
    type lo{mem[0] >> offset & lomsk[width]};
    if (width + offset > bits) {
      type hi{mem[1] & (lomsk[width] >> (bits - offset))};
      lo |= hi << (bits - offset);
    }
    return lo;
  }
};
}; // namespace rmq

#endif
