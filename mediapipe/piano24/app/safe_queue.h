#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
  SafeQueue(bool debug=false)
    : q()
    , m()
    , c()
  {
    debug_output = debug;
  }

  ~SafeQueue()
  {}

  // Add an element to the queue.
  void enqueue(T t)
  {
    if (debug_output) {
      std::cout << "== REQUEST ADDING ELEMENT, SIZE: " << q.size() << std::endl;
    }
    std::lock_guard<std::mutex> lock(m);
    q.push(t);
    if (debug_output) {
      std::cout << "== ELEMENT ADDED, SIZE: " << q.size() << std::endl;
    }
    c.notify_one();
  }

  // Get the "front"-element.
  // If the queue is empty, wait till an element is avaiable.
  T dequeue(void)
  {
    if (debug_output) {
      std::cout << "== REQUEST DEQUEUE ELEMENT, SIZE: " << q.size() << std::endl;
    }
    std::unique_lock<std::mutex> lock(m);
    while(q.empty())
    {
      // release lock as long as the wait and require it afterwards.
      c.wait(lock);
    }
    T val = q.front();
    q.pop();
    if (debug_output) {
      std::cout << "== ELEMENT DEQUEUED, SIZE: " << q.size() << std::endl;
    }
    return val;
  }

  T dequeue_all()
  {
    if (debug_output) {
      std::cout << "== REQUEST DEQUEUE ALL, SIZE: " << q.size() << std::endl;
    }
    std::unique_lock<std::mutex> lock(m);
    while(q.empty())
    {
      // release lock as long as the wait and require it afterwards.
      c.wait(lock);
    }
    T val;
    while(!q.empty())
    {
      val = q.front();
      q.pop();
    }
    if (debug_output) {
      std::cout << "== ALL ELEMENT DEQUEUED, SIZE: " << q.size() << std::endl;
    }
    return val;
  }

private:
  std::queue<T> q;
  mutable std::mutex m;
  std::condition_variable c;
  bool debug_output;
};
#endif

;