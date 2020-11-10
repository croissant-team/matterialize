#ifndef MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_BENCHMARK_HPP

#include "../matting/matter.hpp"
#include "compositor.hpp"
#include "confusion_matrix.hpp"
#include "benchmark_result.hpp"
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
  Benchmark(
      const string &masks_path, const string &backgrounds_path,
      const string &foregrounds_path) {
    const directory_iterator end_iterator;
    directory_iterator background_file_iterator =
        directory_iterator(backgrounds_path);
    directory_iterator foreground_file_iterator =
        directory_iterator(foregrounds_path);
    directory_iterator masks_file_iterator = directory_iterator(masks_path);
    Compositor compositor = Compositor();
    while (background_file_iterator != end_iterator &&
           foreground_file_iterator != end_iterator &&
           masks_file_iterator != end_iterator) {
      string background_filename = background_file_iterator->path().filename();
      string foreground_filename = foreground_file_iterator->path().filename();
      string mask_file_path = masks_file_iterator->path().filename();
      cout << "Composing (bg) " << background_filename << " with (fg) "
           << foreground_filename << "\n";
      Mat background_mat = imread(background_file_iterator->path());
      Mat foreground_mat = imread(foreground_file_iterator->path());
      backgrounds.push_back(background_mat);
      foregrounds.push_back(foreground_mat);
      Mat mask_mat = imread(masks_file_iterator->path());
      pair<Mat, Mat> pair =
          compositor.compose(background_mat, foreground_mat, mask_mat);
      compositions.push_back(pair.first);
      masks.push_back(pair.second);
      background_file_iterator++;
      foreground_file_iterator++;
      masks_file_iterator++;
    }
  }
  vector<BenchmarkResult> run();
  void export_images();
};

#endif
