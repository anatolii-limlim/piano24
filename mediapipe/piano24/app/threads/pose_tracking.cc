#include <numeric>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "../threads.h"

// Utility: get dict of markers to detect
cv::Ptr<cv::aruco::Dictionary> get_aruco_dict(const std::array<int, 3>& desiredIds ) {
    cv::Ptr<cv::aruco::Dictionary> baseDict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::Ptr<cv::aruco::Dictionary> customDict = cv::makePtr<cv::aruco::Dictionary>();
    customDict->markerSize = baseDict->markerSize;
    customDict->maxCorrectionBits = baseDict->maxCorrectionBits;

    for (int id : desiredIds) {
        customDict->bytesList.push_back(baseDict->bytesList.row(id));
    }

    return customDict;
}

void pose_detection_thread(
  Settings& settings,
  FramesData& frames_data,
  SafeQueue<PoseDetectQueueElem>& q_pose,
  SafeQueue<CvFusionQueueElem>& q_cv_fusion
) {
  bool relative_search = false;

  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

  cv::Ptr<cv::aruco::Dictionary> dict_all = get_aruco_dict(ARUCO_MARKERS);
  std::vector<cv::Ptr<cv::aruco::Dictionary>> dicts_1;

  for (int i = 0; i < ARUCO_MARKERS.size(); i++) {
    dicts_1.push_back(get_aruco_dict({ARUCO_MARKERS[i]}));
  }

  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();

  while (true) {
    PoseDetectQueueElem event = q_pose.dequeue_all();

    FPS fps;
    
    Frame* frame = frames_data.get_frame(event.frame_index);
    if (frame == NULL) {
      continue;
    }
    cv::Mat camera_frame;    

    cv::cvtColor(*frame->mat, camera_frame, cv::COLOR_BGR2RGB);

    if (relative_search) {
      for (int i = 0; i < markerIds.size(); i++) {
        cv::Rect bbox = cv::boundingRect(markerCorners[i]);
        bbox.x = std::max(bbox.x - settings.aruco_relative_max_d, 0);
        bbox.y = std::max(bbox.y - settings.aruco_relative_max_d, 0);
        bbox.width = std::min(bbox.width + 2 * settings.aruco_relative_max_d, camera_frame.cols - bbox.x);
        bbox.height = std::min(bbox.height + 2 * settings.aruco_relative_max_d, camera_frame.rows - bbox.y);

        std::vector<int> rMarkerIds;
        std::vector<std::vector<cv::Point2f>> rMarkerCorners, rRejectedCandidates;
        
        cv::aruco::detectMarkers(camera_frame(bbox), dicts_1[i], rMarkerCorners, rMarkerIds, parameters, rRejectedCandidates);

        if (rMarkerCorners.size() != 1 || rMarkerIds[0] != 0) {
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

      std::vector<int> rMarkerIds;
      std::vector<std::vector<cv::Point2f>> rMarkerCorners, rRejectedCandidates;

      cv::aruco::detectMarkers(camera_frame, dict_all, rMarkerCorners, rMarkerIds, parameters, rRejectedCandidates);

      for (size_t i = 0; i < ARUCO_MARKERS.size(); ++i) {
        for (size_t j = 0; j < rMarkerIds.size(); ++j) {
            if (rMarkerIds[j] == i) {
                markerIds.push_back(ARUCO_MARKERS[i]);
                markerCorners.push_back(rMarkerCorners[j]);
                break;
            }
        }
      }
    }

    bool is_pose_detected = markerIds.size() == ARUCO_MARKERS.size();

    if (is_pose_detected) {
      relative_search = true;

      std::array<cv::Point2f, 3> markerCenters;
      for (size_t i = 0; i < markerCorners.size(); ++i) {
          const auto& corner = markerCorners[i];
          cv::Point2f center(0, 0);
          for (const auto& pt : corner) {
            center += pt;
          }
          center *= (1.0f / corner.size());
          markerCenters[i] = center;
      }

      settings.piano_coord.updateFrame(markerCenters);
    }

    frames_data.update_frame_pose(
      event.frame_index, true, is_pose_detected,
      markerIds, markerCorners, PianoCoordinateTransformer(settings.piano_coord),
      fps.get_fps()
    );
  }  
}
