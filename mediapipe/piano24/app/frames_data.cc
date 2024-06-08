#include "threads.h"

int FramesData::add_frame(cv::Mat* frame) {
  std::lock_guard<std::mutex> lock(this->m);

  int index = this->next_frame_index;
  this->frames[index] = Frame { mat: frame };
  this->next_frame_index++;

  return index;
}

void FramesData::erase() {
  std::lock_guard<std::mutex> lock(this->m);

  int index = this->next_frame_index;

  if (this->frames.size() == MAX_FRAMES) {
    Frame elem = this->frames[index - MAX_FRAMES];
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

Frame* FramesData::get_last_frame() {
  std::lock_guard<std::mutex> lock(this->m);

  std::vector<int> keys;
  for (const auto &pair: this->frames) {
    keys.push_back(pair.first);
  }

  sort(keys.begin(), keys.end());

  if (keys.size()) {
    return &this->frames[keys.back()];
  }

  return NULL;  
}    
