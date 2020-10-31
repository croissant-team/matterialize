#include "face_detection_matter.hpp"

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat FaceDetectionMatter::background_mask(const cv::Mat &video_frame) {
  cv::Mat mask(video_frame.size(), CV_8U, cv::Scalar(0.0));
  cv::Mat gray{};

  cv::cvtColor(video_frame, gray, cv::COLOR_BGR2GRAY);

  std::vector<cv::Rect> faces{};

  classifier.detectMultiScale(
      gray, faces, 1.05, 8, 0, cv::Size(50.0, 50.0), cv::Size(150.0, 150.0));

  for (cv::Rect &rect : faces) {
    for (int i{rect.x - rect.width / 2}; i < rect.x + (3 * rect.width) / 2;
         ++i) {
      for (int j{rect.y}; j <= mask.size().height; ++j) {
        mask.at<cv::Vec2d>(j, i) = 255.0;
      }
    }
  }

  return mask;
}
