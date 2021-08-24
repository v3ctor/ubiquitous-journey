#include "Benchmark.hpp"

static constexpr bool test_correctness{true};
static constexpr bool test_init{true};
static constexpr bool test_query{true};

int
main() {
  if constexpr (test_correctness) {
    rmq::bench::test_correctness_all();
  }

  if constexpr (test_init) {
    rmq::bench::bench_init_all();
  }

  if constexpr (test_query) {
    rmq::bench::bench_query_all();
  }

  return 0;
}
