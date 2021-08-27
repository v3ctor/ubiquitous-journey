#include "Benchmark.hpp"

namespace rmq::bench {
string
format_nano(chrono::nanoseconds ns) {
  string_view sv[]{"ns", "us", "ms", "s"};
  string_view cl[]{"\033[94m", "\033[92m", "\033[93m", "\033[91m"};
  size_t sidx{0};
  auto cnt{ns.count()}; // signed, at least 64bit
  for (; cnt >= 10000 && sidx + 1 < sizeof(sv); ++sidx) {
    cnt /= 1000;
  }
  return to_string(cnt) + string(cl[sidx]) + string(sv[sidx]) +
         "\033[0m";
}

vector<ui>
generate_data(size_t size, size_t nuniq) {
  nuniq = min(size, nuniq);
  vector<ui> vals(size);
  vector<ui> elems(nuniq);
  vector<bool> indices(size);

  random_device rd;
  mt19937_64 gen(rd());
  uniform_int_distribution<> size_dist(0, size - 1);
  uniform_int_distribution<> elems_dist(0, elems.size() - 1);

  for (ui &el : elems) {
    el = ui(gen());
    size_t idx;
    while (indices[(idx = size_dist(gen))])
      ;
    indices[idx] = 1;
    vals[idx] = el;
  }

  for (size_t i{0}; i < vals.size(); ++i) {
    if (!indices[i]) {
      vals[i] = elems[elems_dist(gen)];
    }
  }

  return vals;
}

vector<MoQuery>
generate_queries(size_t nqueries, size_t size) {
  vector<MoQuery> queries(nqueries);

  random_device rd;
  mt19937_64 gen(rd());
  uniform_int_distribution<> size_dist(0, size - 1);
  uniform_int_distribution<> size1_dist(0, size);

  for (size_t i{0}; i < queries.size(); ++i) {
    MoQuery &query{queries[i]};
    query.idx = i;
    query.first = size_dist(gen);
    while ((query.last = size1_dist(gen)) == query.first)
      ;
    if (query.first > query.last) {
      swap(query.first, query.last);
    }
  }

  return queries;
}

void
test_correctness_all() {
  constexpr size_t iters{1024}, size{256}, nuniq{32};
  rmq::bench::test_correctness<NaiveRmq>(iters, size, nuniq);
  rmq::bench::test_correctness<NaiveFreqTblRmq>(iters, size, nuniq);
  rmq::bench::test_correctness<MoRmqBST>(iters, size, nuniq);
  rmq::bench::test_correctness<MoRmqList>(iters, size, nuniq);
  rmq::bench::test_correctness<KMSRmq>(iters, size, nuniq);
  rmq::bench::test_correctness<CDLMW1Rmq>(iters, size, nuniq);
  rmq::bench::test_correctness<CDLMW2Rmq>(iters, size, nuniq);
  rmq::bench::test_correctness<SFRmq>(iters, size, nuniq);
  rmq::bench::test_correctness<CDLMW_SFRmq>(iters, size, nuniq);
  cout << endl;
}

constexpr size_t bench_sizes[]{
    1000,    2000,    4000,    6000,    8000,     // 1e3
    10000,   20000,   40000,   60000,   80000,    // 1e4
    100000,  200000,  400000,  600000,  800000,   // 1e5
    1000000, 2000000, 4000000, 6000000, 8000000}; // 1e6

template <typename RMQ, typename Fn>
void
bench_init_single(size_t size_to, Fn nuniq) {
  cout << left << setw(35) << setfill(' ')
       << RMQ_BENCH_FIRST_TMPL_NAME;
  for (size_t const size : bench_sizes) {
    if (size > size_to) {
      break;
    }

    auto dur{bench_init<RMQ>(size, nuniq(size), 100)};
    cout << setfill(' ') << setw(9 + 9) << right << format_nano(dur)
         << " " << flush;
  }
  cout << endl;
}

void
bench_init_all() {
  size_t constexpr const_nuniq_val{64};
  auto const const_nuniq{[&](...) { return const_nuniq_val; }};
  auto const sqrt_nuniq{[](auto x) { return roundl(sqrtl(x)); }};
  auto const lin_nuniq{[](auto x) { return x / 64; }};
  size_t const size{1000000};

  cout << left << setw(35 + 9) << setfill(' ')
       << "\033[94mBENCH INIT\033[0m";
  for (size_t const size : bench_sizes) {
    cout << setfill(' ') << setw(9) << right << size << " " << flush;
  }
  cout << endl;

  cout << "--- nuniq is " << const_nuniq_val << endl;
  bench_init_single<NaiveRmq>(size, const_nuniq);
  bench_init_single<NaiveFreqTblRmq>(size, const_nuniq);
  bench_init_single<MoRmqBST>(size, const_nuniq);
  bench_init_single<MoRmqList>(size, const_nuniq);
  bench_init_single<KMSRmq>(size, const_nuniq);
  bench_init_single<CDLMW1Rmq>(size, const_nuniq);
  bench_init_single<CDLMW2Rmq>(size, const_nuniq);
  bench_init_single<SFRmq>(size, const_nuniq);
  bench_init_single<CDLMW_SFRmq>(size, const_nuniq);
  cout << endl;

  cout << "--- nuniq is sqrt(size)" << endl;
  bench_init_single<NaiveRmq>(size, sqrt_nuniq);
  bench_init_single<NaiveFreqTblRmq>(size, sqrt_nuniq);
  bench_init_single<MoRmqBST>(size, sqrt_nuniq);
  bench_init_single<MoRmqList>(size, sqrt_nuniq);
  bench_init_single<KMSRmq>(size, const_nuniq);
  bench_init_single<CDLMW1Rmq>(size, sqrt_nuniq);
  bench_init_single<CDLMW2Rmq>(size, sqrt_nuniq);
  bench_init_single<SFRmq>(size, sqrt_nuniq);
  bench_init_single<CDLMW_SFRmq>(size, sqrt_nuniq);
  cout << endl;

  cout << "--- nuniq is linear(size)" << endl;
  bench_init_single<NaiveRmq>(size, lin_nuniq);
  bench_init_single<NaiveFreqTblRmq>(size, lin_nuniq);
  bench_init_single<MoRmqBST>(size, lin_nuniq);
  bench_init_single<MoRmqList>(size, lin_nuniq);
  bench_init_single<KMSRmq>(size, const_nuniq);
  bench_init_single<CDLMW1Rmq>(size, lin_nuniq);
  bench_init_single<CDLMW2Rmq>(size, lin_nuniq);
  bench_init_single<SFRmq>(size, lin_nuniq);
  bench_init_single<CDLMW_SFRmq>(size, lin_nuniq);
  cout << endl;
}

struct BenchQueryOpts {
  size_t qfactor{1};
  bool always_linear{false};
};

template <typename RMQ, typename Fn>
void
bench_query_single(size_t size_to, Fn nuniq,
                   BenchQueryOpts opts = {}) {
  cout << setw(35) << left << setfill(' ')
       << RMQ_BENCH_FIRST_TMPL_NAME;
  for (size_t const size : bench_sizes) {
    if (size > size_to) {
      break;
    }

    auto const [qfactor, is_linear] = opts;
    size_t nqueries;
    if (is_linear) {
      nqueries = size / qfactor;
    } else {
      nqueries = max<size_t>(size, size_t(1e6)) / qfactor;
    }
    auto const dur{bench_query<RMQ>(size, nuniq(size), nqueries)};
    cout << setfill(' ') << setw(9 + 9) << right << format_nano(dur)
         << " " << flush;
  }
  cout << endl;
}

void
bench_query_all() {
  size_t constexpr const_nuniq_val{64};
  auto const const_nuniq{[&](...) { return const_nuniq_val; }};
  auto const sqrt_nuniq{[](auto x) { return roundl(sqrtl(x)); }};
  auto const lin_nuniq{[](auto x) { return x / 64; }};
  size_t const small{100000}, big{4000000}, med{400000};

  cout << left << setw(35 + 9) << setfill(' ')
       << "\033[94mBENCH QUERY\033[0m";
  for (size_t const size : bench_sizes) {
    cout << right << setfill(' ') << setw(9) << size << " " << flush;
  }
  cout << endl;

  cout << "--- nuniq is " << const_nuniq_val << endl;
  bench_query_single<NaiveRmq>(med, const_nuniq, {10});
  bench_query_single<NaiveFreqTblRmq>(med, const_nuniq, {10});
  bench_query_single<MoRmqBST>(med, const_nuniq, {1, true});
  bench_query_single<MoRmqList>(big, const_nuniq, {1, true});
  bench_query_single<KMSRmq>(big, const_nuniq);
  bench_query_single<CDLMW1Rmq>(big, const_nuniq);
  bench_query_single<CDLMW2Rmq>(big, const_nuniq);
  bench_query_single<SFRmq>(big, const_nuniq);
  bench_query_single<CDLMW_SFRmq>(big, const_nuniq);
  cout << endl;

  cout << "--- nuniq is sqrt(size)" << endl;
  bench_query_single<NaiveRmq>(med, sqrt_nuniq, {10});
  bench_query_single<NaiveFreqTblRmq>(med, sqrt_nuniq, {10});
  bench_query_single<MoRmqBST>(med, sqrt_nuniq, {1, true});
  bench_query_single<MoRmqList>(big, sqrt_nuniq, {1, true});
  bench_query_single<KMSRmq>(big, sqrt_nuniq);
  bench_query_single<CDLMW1Rmq>(big, sqrt_nuniq);
  bench_query_single<CDLMW2Rmq>(big, sqrt_nuniq);
  bench_query_single<SFRmq>(big, sqrt_nuniq);
  bench_query_single<CDLMW_SFRmq>(big, sqrt_nuniq);
  cout << endl;

  cout << "--- nuniq is linear(size)" << endl;
  bench_query_single<NaiveRmq>(small, lin_nuniq, {10});
  bench_query_single<NaiveFreqTblRmq>(med, lin_nuniq, {10});
  bench_query_single<MoRmqBST>(med, lin_nuniq, {1, true});
  bench_query_single<MoRmqList>(med, lin_nuniq, {1, true});
  bench_query_single<KMSRmq>(big, lin_nuniq);
  bench_query_single<CDLMW1Rmq>(big, lin_nuniq);
  bench_query_single<CDLMW2Rmq>(big, lin_nuniq);
  bench_query_single<SFRmq>(med, lin_nuniq);
  bench_query_single<CDLMW_SFRmq>(big, lin_nuniq);
  cout << endl;
}

template <typename RMQ, typename Fn>
void
bench_offline_single(size_t size_to, Fn nuniq) {
  cout << setw(35) << left << setfill(' ')
       << RMQ_BENCH_FIRST_TMPL_NAME;
  for (size_t const s : bench_sizes) {
    if (s > size_to) {
      break;
    }

    auto const dur{s * bench_query<RMQ>(size_to, nuniq(s), 2 * s)};
    cout << setfill(' ') << setw(9 + 9) << right << format_nano(dur)
         << " " << flush;
  }
  cout << endl;
}

void
bench_offline_all() {
  cout << left << setw(35 + 9) << setfill(' ')
       << "\033[94mBENCH OFFLINE\033[0m";
  for (size_t const size : bench_sizes) {
    cout << right << setfill(' ') << setw(9) << size << " " << flush;
  }
  cout << endl;

  size_t constexpr const_nuniq_val{64};
  auto const const_nuniq{[&](...) { return const_nuniq_val; }};
  auto const sqrt_nuniq{[](auto x) { return roundl(sqrtl(x)); }};
  auto const lin_nuniq{[](auto x) { return x / 64; }};

  size_t const size{400000};
  cout << "--- nuniq is " << const_nuniq_val << endl;
  bench_offline_single<MoRmqBST>(size, const_nuniq);
  bench_offline_single<MoRmqList>(size, const_nuniq);
  cout << endl;

  cout << "--- nuniq is sqrt(size)" << endl;
  bench_offline_single<MoRmqBST>(size, sqrt_nuniq);
  bench_offline_single<MoRmqList>(size, sqrt_nuniq);
  cout << endl;

  cout << "--- nuniq is linear(size)" << endl;
  bench_offline_single<MoRmqBST>(size, lin_nuniq);
  bench_offline_single<MoRmqList>(size, lin_nuniq);
  cout << endl;
}
} // namespace rmq::bench
