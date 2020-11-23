#include "benchmark.hpp"
#include "../matting/background_cut/background_cut_matter.hpp"
#include "../matting/background_negation_matter.hpp"
#include "../matting/opencv_matter.hpp"
#include "../matting/modes.hpp"
#include "scorer.hpp"
#include <chrono>

using namespace chrono;

#define NUM_MATTERS MatterModes::num_modes

vector<vector<BenchmarkResult>> Benchmark::run() {
  vector<vector<BenchmarkResult>> result(NUM_MATTERS + 1);
  string names[NUM_MATTERS];
  for (int i = 0; i < NUM_MATTERS; i++) {
    auto mode = MatterModes::modes[i];
    names[i] = mode->name();
  }

  for (int i = 0; i < backgrounds.size(); i++) {
    MatterInitData init_data{backgrounds[i]};

    IMatter *matters[NUM_MATTERS];
    for (int j = 0; j < NUM_MATTERS; j++) {
      auto mode = MatterModes::modes[j];
      auto config = MatterConfig::default_for(mode);
      matters[j] = mode->init_matter(init_data, config);
    }
    //-------------------------------------------------------------------------

    for (int j = 0; j < NUM_MATTERS; j++) {
      auto start{system_clock::now()};
      Mat mask = matters[j]->background_mask(compositions[i]);
      long time =
          duration_cast<milliseconds>(system_clock::now() - start).count();
      imwrite(
          "../images/output/" + to_string(i) + "_" + names[j] +
              "_generated_mask.png",
          mask);
      ConfusionMatrix stats = Scorer::difference(mask, masks[i]);
      result[j].emplace_back(names[j], stats, time);
    }
  }
  for (int i = 0; i < result.size() - 1; i++) {
    vector<ConfusionMatrix> confusion_matrices;
    for (auto &entry : result[i]) {
      confusion_matrices.push_back(entry.stats);
    }
    result[NUM_MATTERS].emplace_back(
        names[i], ConfusionMatrix::average(confusion_matrices), -1);
  }
  return result;
}

void Benchmark::export_images() {
  for (int i = 0; i < compositions.size(); i++) {
    imwrite(
        "../images/output/" + to_string(i) + "_composed.png", compositions[i]);
    imwrite("../images/output/" + to_string(i) + "_mask.png", masks[i]);
  }
}
