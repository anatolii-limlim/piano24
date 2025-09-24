#include "../threads.h"

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
      // capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
      // capture.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
      // capture.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
      // capture.set(cv::CAP_PROP_FPS, 120);
      // capture.open("v4l2src device=/dev/video2 ! "
      //                "image/jpeg, width=640, height=480, framerate=120/1 ! "
      //                "jpegdec ! videoconvert ! appsink",
      //                cv::CAP_GSTREAMER);
      capture.open(2);
    }
    std::cout << "############################## " << capture.isOpened() << std::endl;
    RET_CHECK(capture.isOpened());
  }

  cv::Mat aruco_set_img = cv::imread("mediapipe/piano24/docs/aruco_set.png");

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
    cv::flip(*camera_frame, *camera_frame, /*flipcode=HORIZONTAL*/ 1);
    
    cv::Rect roi(
      camera_frame->cols - aruco_set_img.cols - 20,
      20,
      aruco_set_img.cols,
      aruco_set_img.rows
    );
    aruco_set_img.copyTo((*camera_frame)(roi));

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
