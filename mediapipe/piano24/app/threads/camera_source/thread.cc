#include "../../threads.h"

absl::Status camera_source_thread(
  Settings& settings,
  FramesData& frames_data,
  SafeQueue<HandTrackingQueueElem>& q_hand_tracking,
  SafeQueue<PoseDetectQueueElem>& q_pose
) {
  ABSL_LOG(INFO) << "Initialize the camera.";
  
  cv::VideoCapture capture;
  cv::Mat static_frame;
  
  const bool is_load_video = !settings.video_file_path.empty();
  const bool is_static_frame = !settings.static_frame_path.empty();
  
  if (is_static_frame) {
    static_frame = cv::imread(settings.static_frame_path);
  } else {
    if (is_load_video) {
        capture.open(settings.video_file_path);
    } else {
        capture.open(0);
    }
    RET_CHECK(capture.isOpened());
  }

  ABSL_LOG(INFO) << "Start grabbing and processing frames.";
  while (true) 
  {
    FPS fps;

    cv::Mat *camera_frame = new cv::Mat();

    if (is_static_frame) {
      *camera_frame = static_frame.clone();
    } else { 
        capture >> *camera_frame;
        if (camera_frame->empty()) {
            if (is_load_video) {
                return absl::Status();
            } else {
                ABSL_LOG(INFO) << "Ignore empty frames from camera.";
                continue;
            }
        }
    }

    // cv::Mat* camera_frame = new cv::Mat();
    // cv::cvtColor(*camera_frame, *camera_frame, cv::COLOR_BGR2RGB);
    // cv::flip(*camera_frame, *camera_frame, /*flipcode=HORIZONTAL*/ 1);

    // Grayscale image
    // cv::cvtColor(*camera_frame, *camera_frame, CV_BGR2GRAY);
    // cv::cvtColor(*camera_frame, *camera_frame, CV_GRAY2BGR);

    // if (is_load_video) {
    //   cv::cvtColor(*camera_frame, *camera_frame, cv::COLOR_RGBA2RGB);
    // }

    int index = frames_data.add_frame(camera_frame);
    frames_data.erase();

    q_hand_tracking.enqueue(HandTrackingQueueElem { frame_index: index });
    q_pose.enqueue(PoseDetectQueueElem { frame_index: index });

    frames_data.update_camera_fps(index, fps.get_fps());

    if (is_static_frame || is_load_video) {
      sleep(1);
    }
  }

  return absl::Status();
}
