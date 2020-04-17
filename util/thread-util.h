// Very sad that gcc does not come with multithreading utilities
// Here I provide basic promise, thread, condition support
#ifndef THREAD_UTIL_H_INCLUDED
#define THREAD_UTIL_H_INCLUDED
#include <pthread.h>
#include <functional>

namespace gameboy
{
  // pthread_t, pthread_mutex_t, pthread_cond_t
  class Thread;
  class Mutex;
  class Condition;

  class Thread
  {
  public:
    typedef void *(*func_t)(void *);
    // Pack the function, does not start thread immediately
    Thread(func_t);
    // Will call join
    ~Thread();
    Thread(const Thread &) = delete;
    // Start the thread
    // No effect if thread already started and simply return 0.
    int start(void *param);
    // Wait for the thread to complete
    int join(void **retval = NULL);
  private:
    pthread_t *thrd;
    func_t func;
  };

  class Mutex
  {
  public:
    Mutex();
    ~Mutex();
    Mutex(const Mutex &) = delete;
    // Basic lock and unlock
    int lock();
    int trylock();
    int unlock();
  private:
    pthread_mutex_t *mtx;
    friend Condition;
  };

  class Condition
  {
  public:
    Condition();
    ~Condition();
    Condition(const Condition &) = delete;
    // pthread_cond_wait
    // Remember to lock this method as critical section.
    int wait();
    // Wait for the the expression to be true
    // Automatically locks and unlocks mutex
    void wait_for(std::function<bool()>);
    // Unblock waiting thread
    // Remember to lock before calling and unlock after.
    int signal();
    // Associated mutex.
    // Directly expose it,  just like it is in pthread
    Mutex mutex;
  private:
    pthread_cond_t *cnd;
  };

  // RAII lock, ctor locks, dtor unlocks
  class Lock
  {
  public:
    Lock(Mutex &);
    ~Lock();
    Lock(Lock &&) = default;
    Lock(const Lock &) = delete;
  private:
    Mutex &mtx;
  };

  // Promise for returning from another thread
  class Promise
  {
  public:
    Promise();
    Promise(const Promise &) = delete;
    // Store value in promise and make it available
    void set_value(int);
    // Get the value, block if not yet available
    int get_value();
    // Reset the state.
    void reset();
  private:
    bool is_ready;
    int value;
    Condition cnd;
  };

};
#endif
