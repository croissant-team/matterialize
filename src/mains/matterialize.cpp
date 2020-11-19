#include "../files/benchmatte/benchmark.hpp"
#include "../files/camera/fake_webcam.hpp"
#include "../files/camera/opencv_webcam_controls.hpp"
#include "../files/camera/video_recorder.hpp"
#include "../files/matting/background_negation_matter.hpp"
#include "../files/server/server_endpoint.hpp"
#include "../files/util/cleanup_handler.hpp"
#include "../files/util/converter.hpp"

#include <chrono>
#include <condition_variable>
#include <csignal>
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
CleanupHandler cleanup_handler{};
std::vector<int> signals{SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};

void handle_signal(int signal) {
  cleanup_handler.perform_cleanup_actions(signal);
}

int main() {
  std::atomic_bool running{true};
  cleanup_handler = CleanupHandler();
  for (auto &signal : signals) {
    std::signal(signal, handle_signal);
  }
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(input_device, width, height);
  try {
    webcam.start();
  } catch (const std::exception &e) { std::cout << e.what() << "\n"; }
  cleanup_handler.set_input_webcam(&webcam);
  // The webcam must be started before
  // webcam controls are initialised

  // The webcam must be started before the webcam controls are initialised
  OpenCVWebcamControls opencv_controls(webcam);
  cleanup_handler.set_camera_controls(&opencv_controls);
  // The webcam must be started before the fake webcam is initialised
  FakeWebcam output(output_device, width, height);
  output.start();
  cleanup_handler.set_output_webcam(&output);
  // Create preview webcam explicitly for frontend GUI use.
  FakeWebcam preview(preview_device, width, height);
  preview.start();
  cleanup_handler.set_preview_webcam(&preview);

  // Grab some frames to ensure exposure/white balance are setup correctly
  webcam.roll(num_void_frames);

  const cv::Mat clean_plate{webcam.grab()};

  IMatter *matter = nullptr;
  MatterMode initial_matter_mode = &MatterModes::NONE;
  std::mutex matter_mutex;
  std::unique_lock<std::mutex> matter_lock(matter_mutex, std::defer_lock);
  BackgroundSettings bg_settings{width, height};

  // The automatic controls should be disabled after the fake cam is
  // initialised to give time for the automatic values to settle
  opencv_controls.disable_automatic();

  // Begin REST server to allow for frontend communication
  Pistache::Port port(9000);
  int thr = 2;
  Pistache::Address addr(Pistache::Ipv4::any(), port);

  ServerEndpoint server(
      addr,
      running,
      webcam,
      opencv_controls,
      matter,
      initial_matter_mode,
      matter_mutex,
      clean_plate,
      bg_settings);
  server.init(thr);
  cleanup_handler.set_server_endpoint(&server);

  std::thread server_thread(&ServerEndpoint::start, &server);
  cleanup_handler.set_server_thread(&server_thread);

  std::cout << "Starting...\n";

  queue<cv::Mat> frame_queue;
  cleanup_handler.set_frame_queue(&frame_queue);
  std::mutex frame_queue_mutex;
  cleanup_handler.set_frame_queue_mutex(&frame_queue_mutex);
  std::condition_variable new_entry;
  cleanup_handler.set_condition_variable(&new_entry);
  bool has_new_entry = false;
  cleanup_handler.set_condition_boolean(&has_new_entry);

  int i = 0;
  auto start{std::chrono::system_clock::now()};
  while (i < 30) {
    matter_lock.lock();
    const cv::Mat frame{webcam.grab()};
    if (frame.empty()) {
      break;
    }

    cv::Mat result{matter->change_background(frame, bg_settings.generate_background(frame))};
    matter_lock.unlock();

    output.write(result);
    preview.write(result);
    frame_queue_mutex.lock();
    frame_queue.push(result);
    has_new_entry = true;
    cout << frame_queue.size() << "\n";
    frame_queue_mutex.unlock();
    new_entry.notify_one();
    i++;
  }
  int fps = 30000 / std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now() - start)
                     .count();
  cout << "fps is " << fps << "\n";

  VideoRecorder recorder = VideoRecorder(
      fps,
      output.get_size(),
      frame_queue,
      frame_queue_mutex,
      new_entry,
      has_new_entry);
  std::thread recording_thread(&VideoRecorder::record, recorder);

  while (running) {
    auto start{std::chrono::system_clock::now()};

    matter_lock.lock();
    const cv::Mat frame{webcam.grab()};
    if (frame.empty()) {
      break;
    }

    cv::Mat result{matter->change_background(frame, bg_settings.generate_background(frame))};
    matter_lock.unlock();

    output.write(result);
    preview.write(result);
    frame_queue_mutex.lock();
    frame_queue.push(result);
    has_new_entry = true;
    cout << frame_queue.size() << "\n";
    frame_queue_mutex.unlock();
    new_entry.notify_one();
    int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now() - start)
                         .count();
    cout << elapsed_ms << '\n';
  }
  cleanup_handler.perform_cleanup_actions(0);
}