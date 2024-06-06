#include <time.h>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "app/threads.h"

int main(int argc, char** argv) {
  cv::Mat image = cv::imread("mediapipe/piano24/docs/aruco0.png", 0);

  if (image.empty()) {
    std::cout << "Image not found";
    return 0;
  }
  std::cout << "Image opened\n";

  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();

  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

  cv::Rect roi;

  roi.x = 202;
  roi.y = 169;
  roi.width = 97;
  roi.height = 97;  
  cv::Mat croppedImage = image(roi);

  for (int i = 0; i < 100; i++) {
    double start_time = clock();
    cv::aruco::detectMarkers(croppedImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
    std::cout << "FPS " << CLOCKS_PER_SEC / (clock() - start_time) * 1 << "\n";
  }  

  std::cout << markerIds << "\n";
  for (auto& markerPoint: markerCorners) {
    for (auto& point: markerPoint) {
      std::cout << point << " ";
    }
    std::cout << "\n";
  }

  // GENERATE

  // cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

  // for (int i = 0; i < 50; i++) {
  //   std::cout << i << '\n';
  //   cv::Mat markerImage;
  //   cv::aruco::drawMarker(dictionary, i, 200, markerImage, 1);
  //   cv::imwrite("mediapipe/piano24/aruco/marker" + std::to_string(i) + ".png", markerImage);
  // }
}
