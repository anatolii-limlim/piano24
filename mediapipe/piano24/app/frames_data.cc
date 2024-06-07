#include "threads.h"

FramesData frames_data;

int FramesData::add_frame(cv::Mat* frame) {
  std::lock_guard<std::mutex> lock(this->m);

  int index = this->next_frame_index;
  this->frames[index] = frame;
  this->next_frame_index++;

  return index;
}

void FramesData::erase() {
  std::lock_guard<std::mutex> lock(this->m);

  int index = this->next_frame_index;

  if (this->frames.size() == MAX_FRAMES) {
    cv::Mat* elem = this->frames[index - MAX_FRAMES];
    this->frames.erase(index - MAX_FRAMES);
    delete elem;    
  }
}

cv::Mat* FramesData::get_frame(int index) {
  std::lock_guard<std::mutex> lock(this->m);

  cv::Mat* frame = NULL;
  if (this->frames.count(index)) {
    frame = this->frames[index];
  }

  return frame;
}

cv::Mat* FramesData::get_last_frame() {
  std::lock_guard<std::mutex> lock(this->m);

  std::vector<int> keys;
  for (const auto &pair: this->frames) {
    keys.push_back(pair.first);
  }

  sort(keys.begin(), keys.end());

  if (keys.size()) {
    return this->frames[keys.back()];
  }

  return NULL;  
}    
