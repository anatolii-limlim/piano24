#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "../../threads.h"

void aruco_detection_thread(
  SafeQueue<ArucoDetectQueueElem>& q_aruco
) {
  bool relative_search = false;

  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();

  cv::Mat image = cv::imread("mediapipe/piano24/docs/aruco0.png", 0);

  while (true) {
    ArucoDetectQueueElem event = q_aruco.dequeue_all();
    cv::Mat* camera_frame = frames_data.get_frame(event.frame_index);
    if (camera_frame == NULL) {
      continue;
    }

    double start_time = clock();
    
    if (!relative_search) {
      markerIds.clear();
      markerCorners.clear();
      rejectedCandidates.clear();

      cv::aruco::detectMarkers(*camera_frame, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
    } else {

    }

    std::cout << "ARUCO DETECTED #" << event.frame_index << " FPS: " << CLOCKS_PER_SEC / (clock() - start_time) << "\n";
  }  
}
