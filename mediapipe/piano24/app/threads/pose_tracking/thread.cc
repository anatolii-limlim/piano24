#include <numeric>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "../../threads.h"

void pose_detection_thread(
  Settings& settings,
  FramesData& frames_data,
  SafeQueue<PoseDetectQueueElem>& q_pose
) {
  bool relative_search = false;

  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();

  cv::Mat image = cv::imread("mediapipe/piano24/docs/aruco0.png", 0);

  while (true) {
    PoseDetectQueueElem event = q_pose.dequeue_all();
    Frame* frame = frames_data.get_frame(event.frame_index);
    if (frame == NULL) {
      continue;
    }
    cv::Mat* camera_frame = frame->mat;

    FPS fps;
    
    if (relative_search) {
      for (int i = 0; i < markerIds.size(); i++) {
        cv::Rect bbox = cv::boundingRect(markerCorners[i]);
        bbox.x = std::max(bbox.x - settings.aruco_relative_max_d, 0);
        bbox.y = std::max(bbox.y - settings.aruco_relative_max_d, 0);
        bbox.width = std::min(bbox.width + 2 * settings.aruco_relative_max_d, camera_frame->cols - bbox.x);
        bbox.height = std::min(bbox.height + 2 * settings.aruco_relative_max_d, camera_frame->rows - bbox.y);

        std::vector<int> rMarkerIds;
        std::vector<std::vector<cv::Point2f>> rMarkerCorners, rRejectedCandidates;
        cv::aruco::detectMarkers((*camera_frame)(bbox), dictionary, rMarkerCorners, rMarkerIds, parameters, rRejectedCandidates);

        if (rMarkerCorners.size() != 1 || rMarkerIds[0] != markerIds[i]) {
          relative_search = false;
          break;
        }

        for (int j = 0; j < 4; j++) {
          markerCorners[i][j] = cv::Point2f(rMarkerCorners[0][j].x + bbox.x, rMarkerCorners[0][j].y + bbox.y);
        }
      }
    }

    if (!relative_search) {
      markerIds.clear();
      markerCorners.clear();
      rejectedCandidates.clear();

      cv::aruco::detectMarkers(*camera_frame, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
    }

    int start_count = std::count_if(
      markerIds.begin(), markerIds.end(),
      [&](int marker_id) { return marker_id == ARUCO_START; }
    );
    int end_count = std::count_if(
      markerIds.begin(), markerIds.end(),
      [&](int marker_id) { return marker_id == ARUCO_END; }
    );
    
    bool is_pose_detected = start_count == 1 && end_count == 2;

    if (is_pose_detected) {
      relative_search = true;
    }

    frames_data.update_frame_pose(
      event.frame_index, true, is_pose_detected,
      markerIds, markerCorners, fps.get_fps()
    );

    std::cout << "ARUCO DETECTED #" << event.frame_index << " " << fps.get_fps_str() << "\n";
  }  
}
