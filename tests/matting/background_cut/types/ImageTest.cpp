#include "matting/background_cut/types/image.hpp"
#include "gtest/gtest.h"
#include <opencv2/core.hpp>

TEST(ImageTest, PixelVarianceTest) {
  // TODO
  Image img = Image(cv::Mat());
  EXPECT_EQ(1, 1);
}