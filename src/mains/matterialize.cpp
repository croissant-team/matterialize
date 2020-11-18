#include "../files/benchmatte/benchmark.hpp"
#include "../files/camera/fake_webcam.hpp"
#include "../files/camera/opencv_webcam_controls.hpp"
#include "../files/matting/background_cut/background_cut_matter.hpp"
#include "../files/matting/background_negation_matter.hpp"
#include "../files/matting/none_matter.hpp"
#include "../files/matting/opencv_matter.hpp"
#include "../files/server/server_endpoint.hpp"
#include "../files/util/converter.hpp"
#include "../files/util/video_devices.hpp"

#include <chrono>
#include <iostream>
#include <pistache/net.h>
#include <thread>
#include <v4l2cpp/V4l2Capture.h>

constexpr int width{640};
constexpr int height{480};
constexpr int input_device{0};
constexpr int preview_device{MATTERIALIZE_PREVIEW};
constexpr int output_device{MATTERIALIZE_CAM};
constexpr int num_void_frames{120};

int main() {
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(input_device, width, height);

  try {
    webcam.start();
  } catch (const std::exception &e) { std::cout << e.what() << "\n"; }
  // The webcam must be started before the webcam controls are initialised
  OpenCVWebcamControls opencv_controls(webcam);
  // The webcam must be started before the fake webcam is initialised
  FakeWebcam output(output_device, width, height);
  output.start();
  // Create preview webcam explicitly for frontend GUI use.
  FakeWebcam preview(preview_device, width, height);
  preview.start();

  // Grab some frames to ensure exposure/white balance are setup correctly
  for (int i{0}; i < num_void_frames; ++i) {
    webcam.grab();
  }

  const cv::Mat clean_plate{webcam.grab()};

  IMatter *matter = nullptr;
  std::string initial_matter{"None"};
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

  ServerEndpoint server(
      addr,
      running,
      webcam,
      matter,
      initial_matter,
      matter_mutex,
      clean_plate,
      bg_mat,
      green_screen);

  server.init(thr);

  std::thread server_thread(&ServerEndpoint::start, &server);

  matter = new BackgroundCutMatter(clean_plate);

  std::cout << "Starting...\n";

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
    preview.write(result);
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now() - start)
                     .count()
              << '\n';
  }

  server.shutdown();
  server_thread.join();
  webcam.stop();
  output.stop();
}