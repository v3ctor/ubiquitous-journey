#ifndef NAIVE_HPP_INCLUDED
#define NAIVE_HPP_INCLUDED

#include "BlockMode.hpp"
#include "FreqValue.hpp"
#include "FrequencyTable.hpp"
#include "MultiMode.hpp"

#include <type_traits>
#include <vector>

namespace rmq {
using namespace std;

struct OfflineAlgorithm {};

template <typename T>
constexpr bool is_online_algorithm_v{
    !is_base_of_v<OfflineAlgorithm, T>};

// for benchmarking purposses
struct EmptyRmq {
  EmptyRmq(vector<ui> const &vals, size_t nuniq);
  FreqValue query(size_t first, size_t last) const noexcept;

private:
  // vector<ui> const &vals_;
  // size_t const nuniq_;
};

struct NaiveRmq {
  NaiveRmq(vector<ui> const &vals, size_t nuniq);
  FreqValue query(size_t first, size_t last) const noexcept;

private:
  vector<ui> const &vals_;
  // size_t const nuniq_;
};

struct NaiveFreqTblRmq {
  NaiveFreqTblRmq(vector<ui> const &vals, size_t nuniq);
  FreqValue query(size_t first, size_t last) const noexcept;

private:
  vector<ui> const &vals_;
  // size_t const nuniq_;
  vector<size_t> mutable freqTbl_;
};

struct MoQuery {
  size_t first, last, idx;
};

template <typename MultiMode>
struct MoRmq : OfflineAlgorithm {
  MoRmq(vector<ui> const &vals, size_t nuniq);
  void query(vector<MoQuery> &&queries, vector<FreqValue> &results);

private:
  vector<ui> const &vals_;
  size_t const nuniq_;
};

struct KMSRmq {
  KMSRmq(vector<ui> const &vals, size_t nuniq);
  FreqValue query(size_t first, size_t last) const noexcept;

private:
  vector<ui> const &vals_;
  // size_t const nuniq_;
  BlockModeTable bm_;
  FrequencyTable fr_;
};

using MoRmqBST = MoRmq<MultiModeBST>;
using MoRmqList = MoRmq<MultiModeList>;

template <typename BlockMode>
struct CDLMWRmq {
  CDLMWRmq(vector<ui> const &vals, size_t nuniq);
  FreqValue query(size_t first, size_t last) const noexcept;

private:
  vector<ui> const &vals_;
  size_t const nuniq_;
  FrequencyRankTable fr_;
  BlockMode bm_;
};

using CDLMW1Rmq = CDLMWRmq<BlockModeTable>;
using CDLMW2Rmq = CDLMWRmq<BlockModeUnary>;

struct SFRmq {
  SFRmq(vector<ui> const &vals, size_t nuniq);
  FreqValue query(size_t first, size_t last) const;

private:
  vector<ui> const &vals_;
  size_t const nuniq_;
  vector<size_t> sf_mem_;
  size_t *sf_;
};

// pimpl?
namespace detail {
struct CountSplitData {
  struct {
    vector<ui> vals, rev_map, I, J;
  } part[2];

  auto &operator[](size_t i) noexcept { return part[i]; }
  auto const &operator[](size_t i) const noexcept { return part[i]; }
};
} // namespace detail

struct CDLMW_SFRmq {
  CDLMW_SFRmq(vector<ui> const &vals, size_t nuniq);
  FreqValue query(size_t first, size_t last) const noexcept;

private:
  detail::CountSplitData const data_;
  CDLMW2Rmq const cdlmw_;
  SFRmq const sf_;
};

} // namespace rmq

#endif
