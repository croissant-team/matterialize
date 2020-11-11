#ifndef MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_RESULT_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_RESULT_HPP

#include "confusion_matrix.hpp"

class BenchmarkResult {
public:
  string owner;
  ConfusionMatrix stats;
  long run_time;
  BenchmarkResult(string owner, ConfusionMatrix stats, long run_time)
      : owner{owner}, stats{stats}, run_time{run_time} {}
};

#endif
