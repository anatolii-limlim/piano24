#include "../threads.h"

void admin_app_thread(
  Settings& settings,
  FramesData& frames_data
) {
  int step_i = 0;

  while (true) {
    double start_time = clock();

    Frame *frame = frames_data.get_last_detected_frame();

    if (frame != NULL) {
      // // ARUCO MARKERS
      // for (auto markerCorners: frame->markerCorners) {
      //   std::vector<cv::Point> points;
      //   for (auto p: markerCorners) {
      //     points.push_back(cv::Point(int(p.x), int(p.y)));
      //   }
      //   cv::polylines(*(frame->mat), points, true, cv::Scalar(255, 0, 0), 2);
      // }

      // // HANDS
      // auto draw_hand = [](cv::Mat* frame, cv::Point2f* hand) {
      //   auto draw_line = [](cv::Mat* frame, cv::Point2f* hand, int i1, int i2) {
      //     cv::line(
      //       *frame,
      //       cv::Point(hand[i1].x * frame->cols, hand[i1].y * frame->rows),
      //       cv::Point(hand[i2].x * frame->cols, hand[i2].y * frame->rows),
      //       cv::Scalar(0, 255, 0),
      //       2,
      //       cv::LINE_8
      //     );
      //   };
      //   draw_line(frame, hand, HT_THUMB_1, HT_THUMB_2);
      //   draw_line(frame, hand, HT_THUMB_2, HT_THUMB_3);
      //   draw_line(frame, hand, HT_THUMB_3, HT_THUMB_4);
      //   draw_line(frame, hand, HT_INDEX_1, HT_INDEX_2);
      //   draw_line(frame, hand, HT_INDEX_2, HT_INDEX_3);
      //   draw_line(frame, hand, HT_INDEX_3, HT_INDEX_4);
      //   draw_line(frame, hand, HT_MIDDLE_1, HT_MIDDLE_2);
      //   draw_line(frame, hand, HT_MIDDLE_2, HT_MIDDLE_3);
      //   draw_line(frame, hand, HT_MIDDLE_3, HT_MIDDLE_4);
      //   draw_line(frame, hand, HT_RING_1, HT_RING_2);
      //   draw_line(frame, hand, HT_RING_2, HT_RING_3);
      //   draw_line(frame, hand, HT_RING_3, HT_RING_4);
      //   draw_line(frame, hand, HT_PINKY_1, HT_PINKY_2);
      //   draw_line(frame, hand, HT_PINKY_2, HT_PINKY_3);
      //   draw_line(frame, hand, HT_PINKY_3, HT_PINKY_4);
      // };

      // if (frame->is_left_hand_found) {
      //   draw_hand(frame->mat, frame->left_hand);
      // }
      // if (frame->is_right_hand_found) {
      //   draw_hand(frame->mat, frame->right_hand);
      // }

      // // KEYBOARD
      // if (frame->is_pose_detected) {
      //   cv::Scalar kbd_color = cv::Scalar(255, 0, 0);
      //   cv::Point2f p1, p2;

      //   settings.piano_coord.kbdBasisToPixel(settings.ethalon_kbd_left_top, p1);
      //   settings.piano_coord.kbdBasisToPixel(settings.ethalon_kbd_right_top, p2);
      //   cv::line(*(frame->mat), p1, p2, kbd_color, 2, cv::LINE_8);

      //   settings.piano_coord.kbdBasisToPixel(settings.ethalon_kbd_right_top, p1);
      //   settings.piano_coord.kbdBasisToPixel(settings.ethalon_kbd_right_bottom, p2);
      //   cv::line(*(frame->mat), p1, p2, kbd_color, 2, cv::LINE_8);
      // }

      // INFO 
      std::string camera_str = "CAMERA: " + std::to_string(((int)frame->camera_fps)) + "fps";
      std::string pose_str = "POSE: " + std::to_string(((int)frame->pose_fps)) + "fps";
      if (!frame->is_pose_detected) {
        pose_str += " NOT FOUND!";
      }
      std::string hands_str = "HANDS: " + std::to_string(((int)frame->hand_tracking_fps)) + "fps";
      if (frame->is_left_hand_found) {
        hands_str += " LEFT";
      }
      if (frame->is_right_hand_found) {
        hands_str += " RIGHT";
      }

      cv::putText(*(frame->mat), camera_str, cv::Point(10, 35), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(0, 0, 255), 2);
      cv::putText(*(frame->mat), pose_str, cv::Point(10, 70), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(0, 0, 255), 2);
      cv::putText(*(frame->mat), hands_str, cv::Point(10, 105), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(0, 0, 255), 2);

      cv::imshow("Piano24", *(frame->mat));
      int key = cv::waitKey(1);
      step_i++;

      if (key == 27 || key == 'q' || key == 'Q') {
        std::cout << "'q' pressed. Quitting..." << std::endl;
        break;
      }
    }

    double sleep_t = (1.0 / settings.admin_app_fps - (clock() - start_time) / CLOCKS_PER_SEC) * CLOCKS_PER_SEC;
    // if (step_i == 1) {
    //   sleep_t = 1000;
    // }
    if (sleep_t > 0) {
      usleep(sleep_t);
    }
  }
}
