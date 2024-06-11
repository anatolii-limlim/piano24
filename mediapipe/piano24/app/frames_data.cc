#include "threads.h"

int FramesData::add_frame(cv::Mat* frame) {
  std::lock_guard<std::mutex> lock(this->m);

  int index = this->next_frame_index;
  this->frames[index].mat = frame;
  this->frames[index].index = index;
  this->next_frame_index++;

  return index;
}

void FramesData::erase() {
  std::lock_guard<std::mutex> lock(this->m);

  int index = this->next_frame_index;

  if (this->frames.size() == MAX_FRAMES) {
    Frame& elem = this->frames[index - MAX_FRAMES];
    this->frames.erase(index - MAX_FRAMES);
    delete elem.mat;    
    delete elem.left_hand;   
    delete elem.right_hand;  
  }
}

Frame* FramesData::get_frame(int index) {
  std::lock_guard<std::mutex> lock(this->m);

  if (this->frames.count(index)) {
    return &this->frames[index];
  }

  return NULL;
}

void FramesData::get_available_keys(std::vector<int>& keys) {
  for (const auto &pair: this->frames) {
    keys.push_back(pair.first);
  }
  sort(keys.begin(), keys.end(), std::greater<>());
}

Frame* FramesData::get_last_frame() {
  std::lock_guard<std::mutex> lock(this->m);

  std::vector<int> keys;
  this->get_available_keys(keys);

  if (keys.size()) {
    return &this->frames[keys[0]];
  }

  return NULL;  
}    

Frame* FramesData::get_last_detected_frame() {
  std::lock_guard<std::mutex> lock(this->m);

  std::vector<int> keys;
  this->get_available_keys(keys);

  for (int i = 0; i < keys.size(); i++) {
    if (this->frames[keys[i]].is_pose_detection_finished &&
        this->frames[keys[i]].is_hand_tracking_finished) {
      return &this->frames[keys[i]];
    }
  }

  return NULL;  
}    

void FramesData::update_camera_fps(
  int frame_index,
  double camera_fps
) {
  Frame* frame = this->get_frame(frame_index);

  std::lock_guard<std::mutex> lock(frame->m);

  frame->camera_fps = camera_fps;
}

void FramesData::update_frame_pose(
  int frame_index,
  bool is_pose_detection_finished,
  bool is_pose_detected,
  std::vector<int> markerIds,
  std::vector<std::vector<cv::Point2f>> markerCorners,
  double pose_fps
) {
  Frame* frame = this->get_frame(frame_index);

  std::lock_guard<std::mutex> lock(frame->m);

  frame->is_pose_detection_finished = is_pose_detection_finished;
  frame->is_pose_detected = is_pose_detected;
  frame->markerIds = markerIds;
  frame->markerCorners = markerCorners;
  frame->pose_fps = pose_fps;
}

void FramesData::update_hands(
  int frame_index,
  bool is_hand_tracking_finished,
  bool is_left_hand_found,
  bool is_right_hand_found,
  cv::Point2f *left_hand,
  cv::Point2f *right_hand,
  double hand_tracking_fps
) {
  Frame* frame = this->get_frame(frame_index);

  std::lock_guard<std::mutex> lock(frame->m);

  frame->is_hand_tracking_finished = is_hand_tracking_finished;
  frame->is_left_hand_found = is_left_hand_found;
  frame->is_right_hand_found = is_right_hand_found;
  frame->left_hand = left_hand;
  frame->right_hand = right_hand;
  frame->hand_tracking_fps = hand_tracking_fps;
}
