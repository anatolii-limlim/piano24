#include <iostream>
#include <thread>
#include <unistd.h>

#include "threads.h"

ABSL_FLAG(std::string, settings_path, "", "Path to file with settings");

int app_main(int argc, char** argv) {
  std::cout << "Hello World!\nQuit with Ctrl-C.\n";

  absl::ParseCommandLine(argc, argv);
  std::string settings_path = absl::GetFlag(FLAGS_settings_path);

  Settings settings;
  settings.load_file(settings_path);

  SafeQueue<MidiEmitterQueueElem> q_midi_emitter;
  SafeQueue<HandTrackingQueueElem> q_hand_tracking;
  SafeQueue<PoseDetectQueueElem> q_pose;

  FramesData frames_data;

  std::thread t_midi_source(midi_source_thread, std::ref(q_midi_emitter));
  std::thread t_midi_emitter(midi_emitter_thread, std::ref(q_midi_emitter));
  std::thread t_camera_source(camera_source_thread, std::ref(settings), std::ref(frames_data), std::ref(q_hand_tracking), std::ref(q_pose));
  std::thread t_hand_tracking(hand_tracking_thread, std::ref(settings), std::ref(frames_data), std::ref(q_hand_tracking));
  std::thread t_pose_detection(pose_detection_thread, std::ref(settings), std::ref(frames_data), std::ref(q_pose));
  
  int step_i = 0;

  while (true) {
    double start_time = clock();

    Frame *frame = frames_data.get_last_detected_frame();

    if (frame != NULL) {
      for (auto markerCorners: frame->markerCorners) {
        std::vector<cv::Point> points;
        for (auto p: markerCorners) {
          points.push_back(cv::Point(int(p.x), int(p.y)));
        }
        cv::polylines(*(frame->mat), points, true, cv::Scalar(0, 0, 255), 2);
      }
      cv::imshow("Piano24", *(frame->mat));
      cv::waitKey(1);
      step_i++;
    }

    double sleep_t = (1.0 / settings.admin_app_fps - (clock() - start_time) / CLOCKS_PER_SEC) * 1000000;
    if (step_i == 1) {
      sleep_t = 100;
    }
    usleep(sleep_t);
  }

  return 0;    
}
