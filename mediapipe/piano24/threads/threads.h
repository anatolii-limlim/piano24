#include <cstddef>
#include <map>

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
#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"
#include "mediapipe/util/resource_util.h"

#include "safe_queue.h"

enum MidiOutQueueElemType { MidiIn, Pitch };

struct MidiOutQueueElem {
  MidiOutQueueElemType type; 

  // if type == MidiIn
  unsigned char byte0, byte1, byte2;

  // if type == Pitch
  unsigned char note;
  double pitch;
};

void midiin_thread( SafeQueue<MidiOutQueueElem> &q_in_midiout );
void midiout_thread( SafeQueue<MidiOutQueueElem> &queue_in );

#define MAX_FRAMES 10

class FramesData {
  std::map<int, cv::Mat> frames;
  int next_frame_index = 0;

  public:
    int add_frame(cv::Mat &frame);
    void get_frame(int index);    
};
