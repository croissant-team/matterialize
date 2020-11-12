#include "../files/benchmatte/benchmark.hpp"
#include "../files/camera/fake_webcam.hpp"
#include "../files/camera/opencv_webcam_controls.hpp"
#include "../files/matting/background_cut/background_cut_matter.hpp"
#include "../files/matting/background_negation_matter.hpp"
#include "../files/matting/none_matter.hpp"
#include "../files/matting/opencv_matter.hpp"
#include "../files/util/converter.hpp"
#include "../files/util/video_devices.hpp"
#include "../files/server/camera_endpoint.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <pistache/net.h>
#include <v4l2cpp/V4l2Capture.h>

constexpr int width{640};
constexpr int height{480};
constexpr int input_device{0};
constexpr int output_device{MATTERIALIZE_CAM};
constexpr int num_void_frames{120};

int main() {
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(input_device, width, height);
  webcam.start();
  // The webcam must be started before request.param(":dev_num").as<std::string>();request.param(":dev_num").as<std::string>();the webcam controls are initialised
  OpenCVWebcamControls opencv_controls(webcam);
  // The webcam must be started before the fake webcam is initialised
  FakeWebcam output(output_device, width, height);
  output.start();
  // Grab some frames to ensure exposure/white balance are setup correctly
  for (int i{0}; i < num_void_frames; ++i) {
    webcam.grab();
  }

  cv::Mat cleanPlate{webcam.grab()};

  // Initialise matters available to user
  auto none_matter{new NoneMatter()};
  auto background_negate_matter{new BackgroundNegationMatter(cleanPlate)};
  auto opencv_matter{new OpenCVMatter()};
  auto background_cut_matter{new BackgroundCutMatter(cleanPlate)};
  std::vector<std::pair<std::string, IMatter *>> matters{
      {"None", none_matter},
      {"Background Negation", background_negate_matter},
      {"OpenCV", opencv_matter},
      {"Background Cut", background_cut_matter}};
  IMatter *matter = none_matter;
  std::mutex matter_mutex;
  std::unique_lock<std::mutex> matter_lock(matter_mutex, std::defer_lock);

  // The automatic controls should be disabled after the fake cam is initialised
  // to give time for the automatic values to settle
  opencv_controls.disable_automatic();

  std::atomic_bool running{true};

  const cv::Scalar scalar(0.0, 255.0, 0.0);
  const cv::Mat green_screen(height, width, CV_8UC3, scalar);
  const cv::Mat *bg_mat;

  // Begin REST server to allow for frontend communication
  Pistache::Port port(9000);
  int thr = 2;
  Pistache::Address addr(Pistache::Ipv4::any(), port);

  CameraEndpoint cam_endpoint(
      addr,
      running,
      webcam,
      matter,
      matter_mutex,
      matters,
      bg_mat,
      green_screen);

  cam_endpoint.init(thr);

  std::thread server_thread(&CameraEndpoint::start, &cam_endpoint);

  while (running) {
    auto start{std::chrono::system_clock::now()};

    matter_lock.lock();
    const cv::Mat frame{webcam.grab()};
    if (frame.empty()) {
      break;
    }

    cv::Mat result{matter->change_background(frame, *bg_mat)};
    matter_lock.unlock();

    output.write(result);
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now() - start)
                     .count()
              << '\n';
  }

  cam_endpoint.shutdown();
  server_thread.join();
  webcam.stop();
  output.stop();
}