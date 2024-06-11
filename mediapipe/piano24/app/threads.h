#include <cstddef>
#include <map>
#include <mutex>
#include <time.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/absl_log.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/classification.pb.h"
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

#define MAX_FRAMES 10

struct Frame {
  std::mutex m;
  int index;
  cv::Mat *mat;
  bool is_pose_detection_finished = false;
  bool is_pose_detected;
  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners;
  bool is_hand_tracking_finished = false;
  bool is_left_hand_found; 
  bool is_right_hand_found; 
  cv::Point2f *left_hand, *right_hand;  // 21 elements arrays
  double camera_fps;  
  double hand_tracking_fps;  
  double pose_fps;
};

class FramesData {
  std::mutex m;
  std::map<int, Frame> frames;
  int next_frame_index = 0;

  void get_available_keys(std::vector<int>& keys);

  public:
    int add_frame(cv::Mat* frame);
    void erase(void);
    Frame* get_frame(int index);    
    Frame* get_last_frame();
    void update_camera_fps(
      int frame_index,
      double camera_fps
    );
    void update_frame_pose(
      int frame_index,
      bool is_pose_detection_finished,
      bool is_pose_detected,
      std::vector<int> markerIds,
      std::vector<std::vector<cv::Point2f>> markerCorners,
      double pose_fps
    );
    void update_hands(
      int frame_index,
      bool is_hand_tracking_finished,
      bool is_left_hand_found,
      bool is_right_hand_found,
      cv::Point2f *left_hand,
      cv::Point2f *right_hand,
      double hand_tracking_fps
    );
    Frame* get_last_detected_frame();    
};

struct HandTrackingQueueElem {
  int frame_index;
};

struct PoseDetectQueueElem {
  int frame_index;
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
  FramesData& frames_data,
  SafeQueue<HandTrackingQueueElem>& q_hand_tracking,
  SafeQueue<PoseDetectQueueElem>& q_pose
);
absl::Status hand_tracking_thread(
  Settings& settings,
  FramesData& frames_data,
  SafeQueue<HandTrackingQueueElem>& q_hand_tracking
);
void pose_detection_thread(
  Settings& settings,
  FramesData& frames_data,
  SafeQueue<PoseDetectQueueElem>& q_pose
);

#define ARUCO_START 8
#define ARUCO_END 47

class FPS {
  timespec start;

  public:
    FPS();
    double get_time_spent();
    double get_fps();
    std::string get_fps_str();
};

#define HAND_LEFT    0
#define HAND_RIGHT   1

#define HT_WRIST     0
#define HT_THUMB_1   1
#define HT_THUMB_2   2
#define HT_THUMB_3   3
#define HT_THUMB_4   4
#define HT_INDEX_1   5
#define HT_INDEX_2   6
#define HT_INDEX_3   7
#define HT_INDEX_4   8
#define HT_MIDDLE_1  9
#define HT_MIDDLE_2  10
#define HT_MIDDLE_3  11
#define HT_MIDDLE_4  12
#define HT_RING_1    13
#define HT_RING_2    14
#define HT_RING_3    15
#define HT_RING_4    16
#define HT_PINKY_1   17
#define HT_PINKY_2   18
#define HT_PINKY_3   19
#define HT_PINKY_4   20

