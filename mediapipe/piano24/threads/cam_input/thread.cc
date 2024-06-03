#include "../threads.h"

absl::Status cam_input_thread() {
  ABSL_LOG(INFO) << "Initialize the camera or load the video.";
  cv::VideoCapture capture;
  capture.open(0);
  RET_CHECK(capture.isOpened());

  ABSL_LOG(INFO) << "Start grabbing and processing frames.";
  bool grab_frames = true;
  while (grab_frames) {
    // Capture opencv camera or video frame.
    cv::Mat camera_frame_raw;
    capture >> camera_frame_raw;
    if (camera_frame_raw.empty()) {
      ABSL_LOG(INFO) << "Ignore empty frames from camera.";
      continue;
    }    
    cv::Mat* camera_frame = new cv::Mat();
    cv::cvtColor(camera_frame_raw, *camera_frame, cv::COLOR_BGR2RGBA);
    cv::flip(*camera_frame, *camera_frame, /*flipcode=HORIZONTAL*/ 1);

    frames_data.add_frame(camera_frame);
  }
}