#include "app/threads.h"

int main(int argc, char** argv) {
  cv::VideoCapture capture;
  capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
  capture.open(0);

  while (true) 
  {
    FPS fps;

    cv::Mat camera_frame_raw;
    capture >> camera_frame_raw;
    std::cout << fps.get_fps_str() << "\n";
  }
}
