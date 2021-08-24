#include "UI.hpp"

#include <cassert>
#include <immintrin.h>
#include <iostream>
using namespace std;
namespace rmq {
namespace {
#ifdef __AVX512F__
#  define _USE_VECTOR
#  define _LOAD(x) _mm512_loadu_epi64((__m512i const *)(x))
#  define _SUB(x, y) _mm512_sub_epi64(x, y)
#  define _STORE(x, y) _mm512_storeu_epi64((__m512 *)(x), y)
constexpr size_t byte_align{64};
#elif __AVX__ && __AVX2__
#  define _USE_VECTOR
#  define _LOAD(x) _mm256_loadu_si256((__m256i const *)(x))
#  define _SUB(x, y) _mm256_sub_epi64(x, y)
#  define _STORE(x, y) _mm256_storeu_si256((__m256i *)(x), y)
constexpr size_t byte_align{32};
#else
constexpr size_t byte_align{sizeof(ui)};
#endif
constexpr size_t el_align{byte_align / sizeof(ui)};
} // namespace

void
ui_sub_slow(ui *dst, ui const *lhs, ui const *rhs, size_t size) {
  for (size_t i{0}; i < size; ++i) {
    dst[i] = lhs[i] - rhs[i];
  }
}

#ifdef _USE_VECTOR
void
ui_sub(ui *dst, ui const *lhs, ui const *rhs, size_t size) {
  // vectorization implemented only for 64bit values ¯\(-_-)/¯, sue me
  if constexpr (sizeof(ui) != 8) {
    _ui_sub(dst, lhs, rhs, size);
  } else {
    while (el_align <= size) {
      auto l{_LOAD(lhs)};
      auto r{_LOAD(rhs)};
      l = _SUB(l, r);
      _STORE(dst, l);
      lhs += el_align;
      rhs += el_align;
      dst += el_align;
      size -= el_align;
    }
    ui_sub_slow(dst, lhs, rhs, size);
  }
}

#else

void
ui_sub(ui *dst, ui const *lhs, ui const *rhs, size_t size) {
  return ui_sub_slow(dst, lhs, rhs, size);
}

#endif

} // namespace rmq
