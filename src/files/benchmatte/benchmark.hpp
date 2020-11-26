#ifndef MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_HPP

#include "../config/config.hpp"
#include "../matting/matter.hpp"
#include "benchmark_result.hpp"
#include "compositor.hpp"
#include "confusion_matrix.hpp"
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <utility>

using namespace cv;
using namespace std;
using namespace filesystem;

class Benchmark {
private:
  vector<Mat> backgrounds;
  vector<Mat> foregrounds;
  vector<Mat> compositions;
  vector<Mat> masks;

public:
  Benchmark(const path benchmark_path = Config::default_benchmark_path()) {
    const path masks_path = benchmark_path / "masks";
    const path backgrounds_path = benchmark_path / "bg";
    const path foregrounds_path = benchmark_path / "fg";

    const directory_iterator end_iterator;
    directory_iterator background_file_iterator =
        directory_iterator(backgrounds_path);
    directory_iterator foreground_file_iterator =
        directory_iterator(foregrounds_path);
    directory_iterator mask_file_iterator = directory_iterator(masks_path);
    Compositor compositor = Compositor();
    while (background_file_iterator != end_iterator &&
           foreground_file_iterator != end_iterator &&
           mask_file_iterator != end_iterator) {
      string background_filename = background_file_iterator->path().filename();
      string foreground_filename = foreground_file_iterator->path().filename();
      string mask_filename = mask_file_iterator->path().filename();
      cout << "Composing (bg) " << background_filename << " with (fg) "
           << foreground_filename << "\n";
      Mat background_mat = imread(background_file_iterator->path());
      Mat foreground_mat = imread(foreground_file_iterator->path());
      Mat mask_mat = imread(mask_file_iterator->path());
      pair<Mat, Mat> pair =
          compositor.compose(background_mat, foreground_mat, mask_mat);
      backgrounds.push_back(background_mat);
      foregrounds.push_back(foreground_mat);
      compositions.push_back(pair.first);
      masks.push_back(pair.second);
      background_file_iterator++;
      foreground_file_iterator++;
      mask_file_iterator++;
    }
  }
  vector<vector<BenchmarkResult>> run();
  void export_images();
};

#endif
