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
  
  admin_app_thread(std::ref(settings), std::ref(frames_data));

  return 0;    
}
