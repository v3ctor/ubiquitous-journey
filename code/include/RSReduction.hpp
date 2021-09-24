#ifndef RS_REDUCTION_HPP_INCLUDED
#define RS_REDUCTION_HPP_INCLUDED

#include "UI.hpp"

#include <cstddef>
#include <vector>

namespace rmq {
using namespace std;
vector<ui> rank_space_reduction(vector<ui> &vals);
vector<ui> rank_space_reduction(vector<ui> &vals, size_t nuniq);
} // namespace rmq

#endif
