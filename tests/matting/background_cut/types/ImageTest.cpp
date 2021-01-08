#include "gtest/gtest.h"
#include <files/matting/background_cut/types/image.hpp>
#include <opencv2/core.hpp>

TEST(ImageTest, PixelVarianceTest) {
  cv::Mat example{cv::Size(3, 3), CV_8UC3};

  example.ptr<uchar>(0, 0)[0] = 0;
  example.ptr<uchar>(0, 0)[1] = 0;
  example.ptr<uchar>(0, 0)[2] = 0;

  example.ptr<uchar>(1, 0)[0] = 255;
  example.ptr<uchar>(1, 0)[1] = 0;
  example.ptr<uchar>(1, 0)[2] = 0;

  example.ptr<uchar>(2, 0)[0] = 0;
  example.ptr<uchar>(2, 0)[1] = 255;
  example.ptr<uchar>(2, 0)[2] = 0;

  example.ptr<uchar>(0, 1)[0] = 255;
  example.ptr<uchar>(0, 1)[1] = 255;
  example.ptr<uchar>(0, 1)[2] = 0;

  example.ptr<uchar>(1, 1)[0] = 0;
  example.ptr<uchar>(1, 1)[1] = 0;
  example.ptr<uchar>(1, 1)[2] = 255;

  example.ptr<uchar>(2, 1)[0] = 255;
  example.ptr<uchar>(2, 1)[1] = 0;
  example.ptr<uchar>(2, 1)[2] = 255;

  example.ptr<uchar>(0, 2)[0] = 0;
  example.ptr<uchar>(0, 2)[1] = 255;
  example.ptr<uchar>(0, 2)[2] = 255;

  example.ptr<uchar>(1, 2)[0] = 255;
  example.ptr<uchar>(1, 2)[1] = 255;
  example.ptr<uchar>(1, 2)[2] = 255;

  example.ptr<uchar>(2, 2)[0] = 127;
  example.ptr<uchar>(2, 2)[1] = 127;
  example.ptr<uchar>(2, 2)[2] = 127;

  Image img{static_cast<const cv::Mat &&>(example)};

  const auto var{img.get_pixel_variances()};

  const double varianceB{var.at(4, 0)};
  const double varianceG{var.at(4, 1)};
  const double varianceR{var.at(4, 2)};

  EXPECT_NEAR(varianceB, 14450.0, 0.1);
  EXPECT_NEAR(varianceG, 14450.0, 0.1);
  EXPECT_NEAR(varianceR, 14450.0, 0.1);
}