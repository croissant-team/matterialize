#include "cleanup_handler.hpp"

void CleanupHandler::finish_recording() {
  frame_queue_mutex->lock();
  while (!frame_queue->empty()) {
    cout << frame_queue->size() << "\n";
    *has_new_entry = true;
    frame_queue_mutex->unlock();
    new_entry->notify_one();
    frame_queue_mutex->lock();
  }
  frame_queue_mutex->unlock();
}

void CleanupHandler::perform_cleanup_actions(int signal) {
  cout << "Cleanup started\n";
  if (controls) {
    controls->enable_automatic();
  }
  if (server) {
    server->shutdown();
  }
  if (server_thread) {
    server_thread->join();
  }
  if (input) {
    input->stop();
  }
  if (output) {
    output->stop();
  }
  if (preview) {
    preview->stop();
  }
  if (frame_queue && frame_queue_mutex && new_entry && has_new_entry) {
    finish_recording();
  }
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
