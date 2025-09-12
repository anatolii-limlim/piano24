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

  SafeQueue<InputProcessingQueueElem> q_input_processing;
  SafeQueue<HandTrackingQueueElem> q_hand_tracking;
  SafeQueue<PoseDetectQueueElem> q_pose;
  SafeQueue<CvFusionQueueElem> q_cv_fusion;
  SafeQueue<MidiEmitterQueueElem> q_midi_emitter;

  FramesData frames_data;

  std::thread t_midi_source(
    midi_source_thread,
    std::ref(settings),
    std::ref(q_input_processing)
  );
  std::thread t_pedal_source(
    pedal_source_thread,
    std::ref(settings),
    std::ref(q_input_processing)
  );
  std::thread t_camera_source(
    camera_source_thread,
    std::ref(settings),
    std::ref(frames_data),
    std::ref(q_hand_tracking),
    std::ref(q_pose)
  );
  std::thread t_hand_tracking(
    hand_tracking_thread,
    std::ref(settings),
    std::ref(frames_data),
    std::ref(q_hand_tracking),
    std::ref(q_cv_fusion)
  );
  std::thread t_pose_detection(
    pose_detection_thread,
    std::ref(settings),
    std::ref(frames_data),
    std::ref(q_pose),
    std::ref(q_cv_fusion)
  );
  std::thread t_cv_fusion(
    cv_fusion_thread,
    std::ref(settings),
    std::ref(frames_data),
    std::ref(q_cv_fusion),
    std::ref(q_input_processing)
  );
  std::thread t_input_processing(
    input_processing_thread,
    std::ref(settings),
    std::ref(q_input_processing),
    std::ref(q_midi_emitter)
  );
  std::thread t_midi_emitter(
    midi_emitter_thread,
    std::ref(settings),
    std::ref(q_midi_emitter)
  );   
    
  admin_app_thread(std::ref(settings), std::ref(frames_data));

  return 0;    
}
