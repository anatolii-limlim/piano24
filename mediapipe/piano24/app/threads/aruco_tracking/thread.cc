#include "../../threads.h"

void aruco_detect(
  SafeQueue<ArucoDetectQueueElem>& q_aruco
) {
  bool relative_search = false;

  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

  while (true) {
    ArucoDetectQueueElem event = q_aruco.dequeue_all();
    
    if (!relative_search) {
      
    } else {

    }
  }  
}
