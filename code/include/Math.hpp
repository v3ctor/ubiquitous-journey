#ifndef MATH_HPP_INCLUDED
#define MATH_HPP_INCLUDED

namespace rmq {
template <typename T>
T
div_ceil(T x, T y) {
  assert(y && "Denominator must not be 0.");
  return (x / y) + (x % y > 0);
}

template <typename T>
bool
is_pow2(T x) {
  return x && !(x & (x - 1));
}

} // namespace rmq

#endif
