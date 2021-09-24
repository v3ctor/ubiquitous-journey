#ifndef PACKED_ARRAY_HPP_INCLUDED
#define PACKED_ARRAY_HPP_INCLUDED

#include "RSSTraits.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rmq {
using namespace std;

struct PackedArray {
  using traits = RSSTraits<uint64_t>;
  using Int = traits::type;

  struct ref {
    ref &operator=(Int val) noexcept;
    operator Int() const noexcept;

    Int *ptr;
    size_t off, width;
  };

  PackedArray(size_t npacked, size_t width);

  ref operator[](size_t i) noexcept;
  Int operator[](size_t i) const noexcept;
  size_t width() const noexcept;
  Int *data() noexcept;
  Int const *data() const noexcept;
  size_t size() const noexcept;

private:
  size_t const npacked_, width_;
  vector<Int> mem_;
};

} // namespace rmq

#endif
