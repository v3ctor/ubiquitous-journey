#ifndef UI_HPP_INCLUDED
#define UI_HPP_INCLUDED

#include <cstddef>
#include <cstdint>

namespace rmq {
using ui = std::uint64_t;
void ui_sub(ui *dst, ui const *lhs, ui const *rhs, size_t size);
} // namespace rmq

#endif