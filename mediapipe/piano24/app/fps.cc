#include "threads.h"

FPS::FPS() {
  clock_gettime(CLOCK_REALTIME, &start);  
}

#define NS_PER_SECOND 1000000000

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

double FPS::get_fps() {
  struct timespec finish, delta;
  clock_gettime(CLOCK_REALTIME, &finish);
  sub_timespec(start, finish, &delta);
  return 1.0 / (delta.tv_sec + (double)delta.tv_nsec / NS_PER_SECOND);
};

std::string FPS::get_fps_str() {
  return "FPS: " + std::to_string(this->get_fps());
};
