#include "threads.h"

FramesData frames_data;

int FramesData::add_frame(cv::Mat* frame) {
  m.lock();

  int index = this->next_frame_index;
  this->frames[index] = frame;
  this->next_frame_index++;

  m.unlock();

  return index;
}

void FramesData::erase() {
  int index = this->next_frame_index;

  m.lock();

  if (this->frames.size() == MAX_FRAMES) {
    cv::Mat* elem = this->frames[index - MAX_FRAMES];
    this->frames.erase(index - MAX_FRAMES);
    delete elem;    
  }

  m.unlock();
}

cv::Mat* FramesData::get_frame(int index) {
  m.lock();

  cv::Mat* frame = NULL;
  if (this->frames.count(index)) {
    frame = this->frames[index];
  }

  m.unlock();

  return frame;
}
