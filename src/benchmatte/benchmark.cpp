#include "benchmark.hpp"
#include "../matting/background_cut/background_cut_matter.hpp"
#include "../matting/background_negation_matter.hpp"
#include "../matting/opencv_matter.hpp"
#include "scorer.hpp"
#include <chrono>

using namespace chrono;

vector<BenchmarkResult> Benchmark::run() {
  vector<BenchmarkResult> result;
  int num_matters = 3;
  for (int i = 0; i < backgrounds.size(); i++) {
    OpenCVMatter opencv_matter = OpenCVMatter();
    BackgroundNegationMatter background_negation_matter =
        BackgroundNegationMatter(backgrounds[i]);
    BackgroundCutMatter background_cut_matter =
        BackgroundCutMatter(backgrounds[i]);
    IMatter *matters[] = {
        &opencv_matter, &background_negation_matter, &background_cut_matter};
    string names[] = {
        "OpenCVMatter", "BackgroundNegationMatter", "BackgroundCutMatter"};
    for (int j = 0; j < num_matters; j++) {
      auto start{system_clock::now()};
      Mat mask = matters[j]->background_mask(compositions[i]);
      long time =
          duration_cast<milliseconds>(system_clock::now() - start).count();
      imwrite(
          "../images/output/" + to_string(i) + "_" + names[j] +
              "_generated_mask.png",
          mask);
      ConfusionMatrix stats = Scorer::difference(mask, masks[i]);
      result.emplace_back(names[j], stats, time);
    }
    break;
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
