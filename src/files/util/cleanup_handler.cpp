#include "cleanup_handler.hpp"

void CleanupHandler::reenable_automatic_controls() {
  cout << "Reenabling automatic controls\n";
  if (controls) {
    controls->enable_automatic();
    controls = nullptr;
  }
  cout << "Reenbled automatic controls\n";
}

void CleanupHandler::shutdown_server() {
  cout << "Shutting down server\n";
  if (server) {
    server->shutdown();
    server = nullptr;
  }
  if (server_thread) {
    server_thread->join();
    server_thread = nullptr;
  }
  cout << "Shut down server\n";
}

void CleanupHandler::stop_cameras() {
  cout << "Stopping cameras\n";
  if (input) {
    input->stop();
    input = nullptr;
  }
  if (output) {
    output->stop();
    output = nullptr;
  }
  if (preview) {
    preview->stop();
    preview = nullptr;
  }
  cout << "Stopped cameras";
}

void CleanupHandler::finish_recording() {
  cout << "Wrapping up recording\n";
  if (frame_queue && frame_queue_mutex && new_entry && has_new_entry) {
    std::unique_lock frame_queue_lock{*frame_queue_mutex, std::defer_lock};
    frame_queue_lock.lock();
    while (!frame_queue->empty()) {
      cout << "Remaining frames " << frame_queue->size() << "\n";
      *has_new_entry = true;
      new_entry->notify_one();
      new_entry->wait(frame_queue_lock, [this] { return !(*has_new_entry); });
    }
    cout << "Recording cleanup ended\n";
    *has_new_entry = true;
    frame_queue_lock.unlock();
    new_entry->notify_one();
    frame_queue = nullptr;
    frame_queue_mutex = nullptr;
    new_entry = nullptr;
    has_new_entry = nullptr;
  }
  if (recording_thread) {
    recording_thread->join();
    recording_thread = nullptr;
  }
  cout << "Wrapped up recording\n";
}

void CleanupHandler::perform_cleanup_actions(int signal) {
  cout << "Cleanup started for signal " << signal << "\n";
  reenable_automatic_controls();
  shutdown_server();
  stop_cameras();
  finish_recording();
  cout << "Cleanup finished\n";
  exit(signal);
}

void CleanupHandler::set_camera_controls(OpenCVWebcamControls *new_controls) {
  controls = new_controls;
}

void CleanupHandler::set_server_endpoint(ServerEndpoint *new_server) {
  server = new_server;
}

void CleanupHandler::set_server_thread(std::thread *new_server_thread) {
  server_thread = new_server_thread;
}

void CleanupHandler::set_input_webcam(OpenCVWebcam *new_webcam) {
  input = new_webcam;
}

void CleanupHandler::set_output_webcam(FakeWebcam *new_output) {
  output = new_output;
}

void CleanupHandler::set_preview_webcam(FakeWebcam *new_preview) {
  preview = new_preview;
}

void CleanupHandler::set_frame_queue(queue<cv::Mat> *new_frame_queue) {
  frame_queue = new_frame_queue;
}

void CleanupHandler::set_frame_queue_mutex(std::mutex *new_frame_queue_mutex) {
  frame_queue_mutex = new_frame_queue_mutex;
}

void CleanupHandler::set_condition_variable(
    std::condition_variable *new_condition_variable) {
  new_entry = new_condition_variable;
}

void CleanupHandler::set_condition_boolean(bool *new_condition_boolean) {
  has_new_entry = new_condition_boolean;
}
void CleanupHandler::set_recording_thread(std::thread *new_recording_thread) {
  recording_thread = new_recording_thread;
}
