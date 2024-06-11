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
  while (true) 
  {
    FPS fps;

    // Capture opencv camera or video frame.
    cv::Mat *camera_frame = new cv::Mat();
    capture >> *camera_frame;
    if (camera_frame->empty()) {
      if (is_load_video) {
        return absl::Status();
      } else {
        ABSL_LOG(INFO) << "Ignore empty frames from camera.";
        continue;
      }
    }    
    // cv::Mat* camera_frame = new cv::Mat();
    // cv::cvtColor(*camera_frame, *camera_frame, cv::COLOR_BGR2RGB);
    cv::flip(*camera_frame, *camera_frame, /*flipcode=HORIZONTAL*/ 1);

    // if (is_load_video) {
    //   cv::cvtColor(*camera_frame, *camera_frame, cv::COLOR_RGBA2RGB);
    // }

    int index = frames_data.add_frame(camera_frame);

    q_hand_tracking.enqueue(HandTrackingQueueElem { frame_index: index });
    q_pose.enqueue(PoseDetectQueueElem { frame_index: index });

    std::cout << "NEW FRAME #" << index << " " << fps.get_fps_str() << "\n";
    frames_data.update_camera_fps(index, fps.get_fps());

    if (is_load_video) {
      sleep(1);
    }
  }

  return absl::Status();
}
