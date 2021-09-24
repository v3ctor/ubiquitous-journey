#include "PackedArray.hpp"
#include "Math.hpp"

namespace rmq {
PackedArray::ref &
PackedArray::ref::operator=(Int val) noexcept {
  traits::pack(ptr, off, val, width);
  return *this;
}

PackedArray::ref::operator Int() const noexcept {
  return traits::unpack(ptr, off, width);
}

PackedArray::PackedArray(size_t npacked, size_t width)
    : npacked_(npacked), width_(max<size_t>(1, width)),
      mem_(div_ceil(npacked_ * width_, traits::bits)) {}

PackedArray::ref
PackedArray::operator[](size_t i) noexcept {
  Int *const ptr{&mem_.data()[i * width_ / traits::bits]};
  size_t const off{i * width_ % traits::bits};
  return ref{ptr, off, width_};
}

PackedArray::Int
PackedArray::operator[](size_t i) const noexcept {
  Int const *ptr{mem_.data() + (i * width_ / traits::bits)};
  size_t const off{i * width_ % traits::bits};
  return traits::unpack(ptr, off, width_);
}

size_t
PackedArray::width() const noexcept {
  return width_;
}

PackedArray::Int *
PackedArray::data() noexcept {
  return mem_.data();
}
PackedArray::Int const *
PackedArray::data() const noexcept {
  return mem_.data();
}

size_t
PackedArray::size() const noexcept {
  return npacked_;
}
} // namespace rmq
