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

struct HandTrackingQueueElem {
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

void midiin_thread( SafeQueue<MidiEmitterQueueElem>& q_in_midiout );
void midiout_thread( SafeQueue<MidiEmitterQueueElem>& queue_in );
absl::Status cam_input_thread( SafeQueue<HandTrackingQueueElem>& q_in_hand_tracking );
absl::Status hand_tracking_thread( std::string& graph_config_file, SafeQueue<HandTrackingQueueElem>& queue_in );

#define MAX_FRAMES 10

class FramesData {
  std::mutex m;
  std::map<int, cv::Mat*> frames;
  int next_frame_index = 0;

  public:
    int add_frame(cv::Mat* frame);
    void erase(void);
    cv::Mat* get_frame(int index);    
};

extern FramesData frames_data;

