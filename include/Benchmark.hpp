#ifndef BENCHMARK_HPP_INCLUDED
#define BENCHMARK_HPP_INCLUDED

#include "Algorithms.hpp"
#include "RSReduction.hpp"
#include "UI.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <random>
#include <string_view>
#include <vector>

#ifndef RMQ_BENCH_FIRST_TMPL_NAME
#  define RMQ_BENCH_FIRST_TMPL_NAME                                            \
    ({                                                                         \
      std::string_view view(__PRETTY_FUNCTION__);                              \
      auto const eq_pos{view.find("=")};                                       \
      auto const semi_pos{view.find(";", eq_pos)};                             \
      auto const comma_pos{view.find(",", eq_pos)};                            \
      auto const brace_pos{view.find("]", eq_pos)};                            \
      auto const first{eq_pos};                                                \
      auto const last{std::min(std::min(semi_pos, comma_pos), brace_pos)};     \
      view.substr(first + 2, last - first - 2);                                \
    })
#endif

namespace rmq::bench {
using namespace std;
vector<ui> generate_data(size_t size, size_t nuniq);
vector<MoQuery> generate_queries(size_t nqueries, size_t size);

namespace detail {
static inline void
escape(void *p) {
  asm volatile("" ::"g"(p) : "memory");
}

static inline void
clobber() {
  asm volatile("" ::: "memory");
}
} // namespace detail

template <typename RMQ>
chrono::nanoseconds
bench_query(size_t size, size_t nuniq, size_t nqueries) {
  using clock = chrono::steady_clock;
  vector<ui> vals(generate_data(size, nuniq));
  vector<MoQuery> queries{generate_queries(nqueries, size)};
  vector<FreqValue> modes((1 - is_online_algorithm_v<RMQ>)*queries.size());
  rank_space_reduction(vals);
  RMQ rmq(vals, nuniq);

  auto const start{clock::now()};
  if constexpr (is_online_algorithm_v<RMQ>) {
    for (MoQuery query : queries) {
      FreqValue mode{rmq.query(query.first, query.last)};
      detail::escape(&mode);
      detail::clobber();
    }
  } else {
    rmq.query(std::move(queries), modes);
    detail::escape(modes.data());
    detail::clobber();
  }
  auto const end{clock::now()};
  auto const query_time{(end - start) / queries.size()};
  // auto const query_time{(end - start) /*/ queries.size()*/};
  return query_time;
}

void bench_query_all();

template <typename RMQ>
chrono::nanoseconds
bench_init(size_t size, size_t nuniq, size_t iters) {
  using clock = chrono::steady_clock;
  chrono::nanoseconds duration(0);
  for (size_t i{0}; i < iters; ++i) {
    vector<ui> vals(generate_data(size, nuniq));
    rank_space_reduction(vals);
    auto const start{clock::now()};
    RMQ rmq(vals, nuniq);
    detail::escape(&rmq);
    detail::clobber();
    duration += (clock::now() - start);
  }
  return duration / max<size_t>(1, iters);
}

void bench_init_all();

template <typename RMQ>
void
test_correctness(size_t iters, size_t size, size_t nuniq) {
  using clock = chrono::steady_clock;
  auto const start{clock::now()};
  constexpr bool is_offline{!is_online_algorithm_v<RMQ>};
  for (size_t iter{0}; iter < iters; ++iter) {
    vector<ui> vals{generate_data(size, nuniq)};
    rank_space_reduction(vals);
    RMQ rmq(vals, nuniq);
    for (size_t i{0}; i < size; ++i) {
      vector<size_t> count(nuniq);
      FreqValue mode{0, 0};
      vector<MoQuery> queries(is_offline * (size - i));
      vector<FreqValue> modes(is_offline * (size - i));
      if constexpr (is_offline) {
        for (size_t j{i}; j < size; ++j) {
          queries[j - i] = {i, j + 1, j - i};
        }
      }
      random_shuffle(queries.begin(), queries.end());

      if constexpr (is_offline) {
        rmq.query(move(queries), modes);
      }

      for (size_t j{i}; j < size; ++j) {
        ui const val{vals[j]};
        if (size_t const freq{++count[val]}; freq > mode.freq) {
          mode = {freq, val};
        }

        FreqValue rmode;
        if constexpr (is_offline) {
          rmode = modes[j - i];
        } else {
          rmode = rmq.query(i, j + 1);
        }
        bool ok{true};
        if (rmode.freq != mode.freq) {
          cout << "Reportd Frequency is not of a mode" << endl;
          ok = false;
        }

        if (rmode.freq > 0 && count[rmode.value] != rmode.freq) {
          cout << "Value does not match with reported frequency" << endl;
          ok = false;
        }

        if (!ok) {
          cout << "Class   : " << RMQ_BENCH_FIRST_TMPL_NAME << endl;
          cout << "Query   : " << i << " " << j + 1 << endl;
          cout << "Reported: " << rmode.freq << " " << rmode.value << endl;
          cout << "Ex Mode : " << mode.freq << " " << mode.value << endl;
          cout << "Vals: " << nuniq << " {";
          for (ui const val : vals) {
            cout << val << ", ";
          }
          cout << "}" << endl;
          return;
        }
      }
    }
  }

  auto end{clock::now()};
  auto ms{chrono::duration_cast<chrono::milliseconds>(end - start).count()};

  cout << "Test " << left << setw(35) << setfill('.')
       << RMQ_BENCH_FIRST_TMPL_NAME << " ok " << ms << "ms" << endl;
  return;
}

void test_correctness_all();

} // namespace rmq::bench

#endif
