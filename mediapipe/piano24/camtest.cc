#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cap("v4l2src device=/dev/video0 ! "
                     "videoconvert ! appsink",
                     cv::CAP_GSTREAMER); // try other indices: 1, 2...
    if (!cap.isOpened()) {
        std::cerr << "❌ Cannot open camera" << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        cv::imshow("Camera", frame);
        if (cv::waitKey(1) == 27) break; // ESC to exit
    }
    return 0;
}
