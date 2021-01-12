#include "benchmark.hpp"
#include "../config/config.hpp"
#include "../matting/modes.hpp"
#include "scorer.hpp"
#include <chrono>

using namespace chrono;

#define NUM_MATTERS MatterModes::num_modes

vector<vector<BenchmarkResult>> Benchmark::run() {
  const path output_folder = Config::default_benchmark_path() / "output";
  create_directory(output_folder);

  vector<vector<BenchmarkResult>> result(NUM_MATTERS + 1);
  string names[NUM_MATTERS];
  vector<MatterConfig> configs{};

  for (int i = 0; i < NUM_MATTERS; i++) {
    auto mode = MatterModes::modes[i];
    names[i] = mode->name();
    configs.emplace_back(MatterConfig::default_for(mode));
  }

  for (int i = 0; i < backgrounds.size(); i++) {
    MatterInitData init_data{backgrounds[i]};

    IMatter *matters[NUM_MATTERS];
    for (int j = 0; j < NUM_MATTERS; j++) {
      auto mode = MatterModes::modes[j];
      matters[j] = mode->init_matter(init_data, configs[j]);
    }

    for (int j = 0; j < NUM_MATTERS; j++) {
      auto start{system_clock::now()};
      Mat mask = matters[j]->background_mask(compositions[i]);
      long time =
          duration_cast<milliseconds>(system_clock::now() - start).count();
      const path output_file = output_folder / (to_string(i) + "_" + names[j] +
                                                "_generated_mask.png");
      imwrite(output_file, mask);
      ConfusionMatrix stats = Scorer::difference(mask, masks[i]);
      result[j].emplace_back(names[j], stats, time);

      delete matters[j];
      matters[j] = nullptr;
    }
  }

  for (int i = 0; i < result.size() - 1; i++) {
    vector<ConfusionMatrix> confusion_matrices;
    long run_time_sum;
    for (auto &entry : result[i]) {
      confusion_matrices.push_back(entry.stats);
      run_time_sum += entry.run_time;
    }
    long avg_run_time = run_time_sum / (result.size() - 1);

    result[NUM_MATTERS].emplace_back(
        names[i], ConfusionMatrix::average(confusion_matrices), avg_run_time);
  }
  return result;
}

void Benchmark::export_images() {
  const path output_folder = Config::default_benchmark_path() / "output";
  create_directory(output_folder);
  for (int i = 0; i < compositions.size(); i++) {
    string name = to_string(i);
    imwrite(output_folder / (name + "_composed.png"), compositions[i]);
    imwrite(output_folder / (name + "_mask.png"), masks[i]);
  }
}
