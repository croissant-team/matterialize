#ifndef MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_RESULT_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_RESULT_HPP

#include "../matting/modes.hpp"
#include "confusion_matrix.hpp"
#include <nlohmann/json.hpp>

class BenchmarkResult {
public:
  string owner;
  ConfusionMatrix stats;
  long run_time;
  BenchmarkResult(string owner, ConfusionMatrix stats, long run_time)
      : owner{owner}, stats{stats}, run_time{run_time} {}
};

// TODO Someone refactor the benchmarking system please. This
// wrapper is just a hack around the type returned by run
// to get averages for the matter endpoint.
class BenchmarkResults {
  friend void to_json(json &j, const BenchmarkResults &benchmark_results);
private:
  using AvgResult = BenchmarkResult;
  map<MatterMode, AvgResult> results{};
public:
  explicit BenchmarkResults(vector<vector<BenchmarkResult>> raw_run_results);
};


#endif
