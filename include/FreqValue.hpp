#ifndef FREQ_VALUE_HPP_INCLUDED
#define FREQ_VALUE_HPP_INCLUDED

#include "UI.hpp"

#include <cstddef>

namespace rmq {
using namespace std;
struct FreqValue {
  size_t freq;
  ui value;
};
} // namespace rmq

#endif
