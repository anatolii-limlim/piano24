#include <numeric>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "../../threads.h"

void pose_detection_thread(
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

    double start_time = clock();
    
    if (relative_search) {
      for (int i = 0; i < markerIds.size(); i++) {
        cv::Rect bbox = cv::boundingRect(markerCorners[i]);
        bbox.x -= ARUCO_RELATIVE_DW;
        bbox.y -= ARUCO_RELATIVE_DW;
        bbox.width += 2 * ARUCO_RELATIVE_DW;
        bbox.height += 2 * ARUCO_RELATIVE_DW;

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

      relative_search = true;
    }

    std::cout << "ARUCO DETECTED #" << event.frame_index << " FPS: " << CLOCKS_PER_SEC / (clock() - start_time) << "\n";
  }  
}
