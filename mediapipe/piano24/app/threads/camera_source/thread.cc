#include "../../threads.h"

absl::Status camera_source_thread(
  Settings& settings,
  FramesData& frames_data,
  SafeQueue<HandTrackingQueueElem>& q_hand_tracking,
  SafeQueue<PoseDetectQueueElem>& q_pose
) {
  ABSL_LOG(INFO) << "Initialize the camera.";
  cv::VideoCapture capture;
  const bool is_load_video = !settings.video_file_path.empty();
  if (is_load_video) {
    capture.open(settings.video_file_path);
  } else {
    capture.open(0);
  }
  RET_CHECK(capture.isOpened());

  ABSL_LOG(INFO) << "Start grabbing and processing frames.";
  bool grab_frames = true;
  while (grab_frames) 
  {
    double start_time = clock();

    // Capture opencv camera or video frame.
    cv::Mat camera_frame_raw;
    capture >> camera_frame_raw;
    if (camera_frame_raw.empty()) {
      if (is_load_video) {
        return absl::Status();
      } else {
        ABSL_LOG(INFO) << "Ignore empty frames from camera.";
        continue;
      }
    }    
    cv::Mat* camera_frame = new cv::Mat();
    cv::cvtColor(camera_frame_raw, *camera_frame, cv::COLOR_BGR2RGBA);
    // cv::flip(*camera_frame, *camera_frame, /*flipcode=HORIZONTAL*/ 1);

    if (is_load_video) {
      cv::cvtColor(*camera_frame, *camera_frame, cv::COLOR_RGBA2RGB);
    }

    int index = frames_data.add_frame(camera_frame);

    std::cout << "-- NEW FRAME #" << index << " FPS: " << CLOCKS_PER_SEC / (clock() - start_time) << "\n";;
    q_hand_tracking.enqueue(HandTrackingQueueElem { frame_index: index });
    q_pose.enqueue(PoseDetectQueueElem { frame_index: index });

    if (is_load_video) {
      sleep(1);
    }
  }

  return absl::Status();
}
