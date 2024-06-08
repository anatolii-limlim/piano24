#include <cstddef>
#include <map>
#include <mutex>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/absl_log.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/util/resource_util.h"

#include "safe_queue.h"

int app_main(int argc, char** argv);

class Settings {
  public:
    std::string graph_config_path;
    std::string video_file_path;
    int aruco_relative_max_d;
    int target_fps;
    int admin_app_fps;

    void load_file( std::string file_name );    
};

struct HandTrackingQueueElem {
  int frame_index;
};

enum ArucoDetectQueueElemType { NewFrame, OneDetected };

struct ArucoDetectQueueElem {
  ArucoDetectQueueElemType type;

  // if type == NewFrame || type == OneDetected
  int frame_index;
  // if type == OneDetected
  int aruco_index;
  int x, y;
};

enum MidiEmitterQueueElemType { MidiIn, Pitch };

struct MidiEmitterQueueElem {
  MidiEmitterQueueElemType type; 

  // if type == MidiIn
  unsigned char byte0, byte1, byte2;
  // if type == Pitch
  unsigned char note;
  double pitch;
};

void midi_source_thread( SafeQueue<MidiEmitterQueueElem>& q_midi_emitter );
void midi_emitter_thread( SafeQueue<MidiEmitterQueueElem>& q_midi_emitter );
absl::Status camera_source_thread(
  Settings& settings,
  SafeQueue<HandTrackingQueueElem>& q_hand_tracking,
  SafeQueue<ArucoDetectQueueElem>& q_aruco
);
absl::Status hand_tracking_thread(
  Settings& settings,
  SafeQueue<HandTrackingQueueElem>& q_hand_tracking
);
void aruco_detection_thread(
  SafeQueue<ArucoDetectQueueElem>& q_aruco
);

#define MAX_FRAMES 10

struct Frame {
  cv::Mat *mat;
  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners;
  double camera_fps;  
  double hand_tracking_fps;  
  double aruco_fps;
};

class FramesData {
  std::mutex m;
  std::map<int, Frame> frames;
  int next_frame_index = 0;

  public:
    int add_frame(cv::Mat* frame);
    void erase(void);
    Frame* get_frame(int index);    
    Frame* get_last_frame();    
};

extern FramesData frames_data;

#define ARUCO_START 8
#define ARUCO_END 47

#define ARUCO_RELATIVE_DW 30