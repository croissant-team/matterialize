#include "benchmark.hpp"
#include "../matting/background_cut/background_cut_matter.hpp"
#include "../matting/background_negation_matter.hpp"
#include "../matting/opencv_matter.hpp"
#include "scorer.hpp"
#include <chrono>

using namespace chrono;

vector<vector<BenchmarkResult>> Benchmark::run() {
  vector<vector<BenchmarkResult>> result(NUM_MATTERS + 1);
  string names[] = {
      "OpenCVMatter", "BackgroundNegationMatter", "BackgroundCutMatter"};
  for (int i = 0; i < backgrounds.size(); i++) {
    OpenCVMatter opencv_matter = OpenCVMatter();

    // TODO Ask Matteo for cleanup help here
    const auto background_negation_mode = BackgroundNegationMode();
    auto background_negation_config =
        MatterConfig::default_for(&background_negation_mode);
    auto background_negation_matter =
        BackgroundNegationMatter(backgrounds[i], background_negation_config);
    //-------------------------------------------------------------------------

    BackgroundCutMatter background_cut_matter =
        BackgroundCutMatter(backgrounds[i]);
    IMatter *matters[] = {
        &opencv_matter, &background_negation_matter, &background_cut_matter};
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
