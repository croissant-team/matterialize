#include "benchmark.hpp"

void to_json(json &j, const BenchmarkResults &benchmark_results) {
  for (auto &[mode, avg_result] : benchmark_results.results) {
    j[mode->name()] = {
        {"accuracy", avg_result.stats.calculate_accuracy()},
        {"run_time", avg_result.run_time},
    };
  }
}

BenchmarkResults::BenchmarkResults(
    vector<vector<BenchmarkResult>> raw_run_results) {
  for (int i = 0; i < MatterModes::num_modes; i++) {
    BenchmarkResult avg_result = raw_run_results[i].back();
    auto mode = MatterModes::modes[i];
    results.emplace(mode, avg_result);
  }
}
