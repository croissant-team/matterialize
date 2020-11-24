#include "recording_handler.hpp"

#include <time.h>
#include <wait.h>

void RecordingHandler::setup_routes(Pistache::Rest::Router &router) {
  using namespace Pistache::Rest;

  Routes::Post(
      router,
      "/recording/start",
      Routes::bind(&RecordingHandler::start_recording, this));
  Routes::Post(
      router,
      "/recording/stop",
      Routes::bind(&RecordingHandler::stop_recording, this));
}

void RecordingHandler::cleanup() {
  if (recording) {
    recording = false;
    kill(recording_pid, SIGINT);
    waitpid(recording_pid, nullptr, 0);
  }
}

void RecordingHandler::start_recording(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  pid_t thread_pid = fork();
  if (thread_pid == 0) {
    time_t raw_time;
    time(&raw_time);
    char file_name[256];
    strftime(file_name, 30, "%Y-%m-%d|%H:%M:%S", localtime(&raw_time));
    execlp(
        "ffmpeg",
        "-y",
        "-f",
        "v4l2",
        "-framerate",
        "25",
        "-video_size",
        "640x480",
        "-i",
        "/dev/video100",
        (recordings_path + file_name + ".mkv").c_str(),
        NULL);
  } else {
    recording = true;
    recording_pid = thread_pid;
    response.send(Pistache::Http::Code::Ok);
  }
  response.send(Pistache::Http::Code::Internal_Server_Error);
}

void RecordingHandler::stop_recording(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  if (!recording) {
    response.send(Pistache::Http::Code::Ok);
    return;
  }
  recording = false;

  kill(recording_pid, SIGINT);
  int join_status;
  if (waitpid(recording_pid, &join_status, 0) == -1) {
    response.send(Pistache::Http::Code::Internal_Server_Error);
    return;
  }

  if (!WIFEXITED(join_status)) {
    response.send(Pistache::Http::Code::Internal_Server_Error);
    return;
  }

  if (WEXITSTATUS(join_status) == EXIT_FAILURE) {
    response.send(Pistache::Http::Code::Internal_Server_Error);
    return;
  }

  response.send(Pistache::Http::Code::Ok);
}
