#ifndef BIT_ARRAY_HPP_INCLUDED
#define BIT_ARRAY_HPP_INCLUDED

#include "RSSTraits.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rmq {

using namespace std;

struct BitArray {
  using traits = RSSTraits<uint64_t>;
  using Int = traits::type;

  struct ref {
    ref &operator=(bool b) noexcept;
    operator bool() const noexcept;

    Int *ptr;
    size_t off;
  };

  BitArray(size_t nbits);

  ref operator[](size_t i) noexcept;
  bool operator[](size_t i) const noexcept;

  Int *data();
  Int const *data() const;

  size_t size() const noexcept;

private:
  size_t const nbits_;
  vector<Int> mem_;
};

template <bool Bit>
BitArray make_summary_ba(BitArray const &ba);

template <bool Bit>
BitArray make_delimiter_ba(BitArray const &ba, size_t nones);
template <bool Bit>
BitArray make_delimiter_ba(BitArray const &ba);

template <bool Bit>
size_t count_ba(BitArray const &ba) noexcept;

} // namespace rmq

#endif
