#include "Algorithms.hpp"

namespace rmq {
EmptyRmq::EmptyRmq(vector<ui> const & /*vals*/, size_t /* nuniq*/)
/*: vals_(vals), nuniq_(nuniq)*/ {}

FreqValue
EmptyRmq::query(size_t, size_t) const noexcept {
  return {0, 0};
}
} // namespace rmq
