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
    return &this->frames[0];
  }

  return NULL;  
}    

Frame* FramesData::get_last_detected_frame() {
  std::lock_guard<std::mutex> lock(this->m);

  std::vector<int> keys;
  this->get_available_keys(keys);

  for (int i = 0; i < keys.size(); i++) {
    if (this->frames[i].is_pose_detection_finished) {
      return &this->frames[i];
    }
  }

  return NULL;  
}    

void FramesData::update_frame_pose(
  int frame_index,
  bool is_pose_detection_finished,
  bool is_pose_detected,
  std::vector<int> markerIds,
  std::vector<std::vector<cv::Point2f>> markerCorners
) {
  Frame* frame = this->get_frame(frame_index);

  std::lock_guard<std::mutex> lock(frame->m);

  frame->is_pose_detection_finished = is_pose_detection_finished;
  frame->is_pose_detected = is_pose_detected;
  frame->markerIds = markerIds;
  frame->markerCorners = markerCorners;
}