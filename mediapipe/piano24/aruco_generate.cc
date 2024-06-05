#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "app/threads.h"

int main(int argc, char** argv) {
  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

  for (int i = 0; i < 50; i++) {
    std::cout << i << '\n';
    cv::Mat markerImage;
    cv::aruco::drawMarker(dictionary, i, 200, markerImage, 1);
    cv::imwrite("mediapipe/piano24/aruco/marker" + std::to_string(i) + ".png", markerImage);
  }
}
