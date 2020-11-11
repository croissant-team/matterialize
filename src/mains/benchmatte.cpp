#include "../files/benchmatte/benchmark.hpp"

int main() {
  Benchmark benchmark =
      Benchmark("../images/masks", "../images/bg", "../images/fg");
  vector<vector<BenchmarkResult>> results = benchmark.run();
  for (int i = 0; i < results.size() - 1; i++) {
    cout << "Results for Image Combination " << i << "\n\n";
    for (auto &result : results[i]) {
      cout << result.owner << ": " << result.run_time << " ms\n"
           << result.stats.to_string() << "\n\n";
    }
  }
  cout << "Average Results\n\n";
  for (auto &result : results[results.size() - 1]) {
    cout << result.owner << "\n" << result.stats.to_string() << "\n\n";
  }
  benchmark.export_images();
}