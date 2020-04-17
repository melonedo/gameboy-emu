#include <functional>
#include "thread-util.h"
namespace gameboy
{
  Thread::Thread(Thread::func_t func_)
  {
    thrd = nullptr;
    func = func_;
  }

  Thread::~Thread()
  {
    join();
  }

  int Thread::start(void *param)
  {
    if (thrd != nullptr)
      return 0;
    thrd = new pthread_t;
    // Explicitly declare joinable
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int res = pthread_create(thrd, &attr, func, param);
    pthread_attr_destroy(&attr);
    return res;
  }

  int Thread::join(void **retval)
  {
    if (thrd == nullptr)
      return 0;
    int res = pthread_join(*thrd, retval);
    delete thrd;
    thrd = nullptr;
    return res;
  }

  Mutex::Mutex()
  {
    mtx = new pthread_mutex_t;
    pthread_mutex_init(mtx, NULL);
  }

  Mutex::~Mutex()
  {
    if (mtx == nullptr)
      return;
    lock();
    unlock();
    pthread_mutex_destroy(mtx);
    delete mtx;
  }

  int Mutex::lock()
  {
    return pthread_mutex_lock(mtx);
  }

  int Mutex::unlock()
  {
    return pthread_mutex_unlock(mtx);
  }

  int Mutex::trylock()
  {
    return pthread_mutex_trylock(mtx);
  }

  Condition::Condition()
  {
    cnd = new pthread_cond_t;
    pthread_cond_init(cnd, NULL);
  }

  Condition::~Condition()
  {
    if (cnd == nullptr)
      return;
    pthread_cond_destroy(cnd);
    delete cnd;
  }

  int Condition::wait()
  {
    return pthread_cond_wait(cnd, mutex.mtx);
  }

  void Condition::wait_for(std::function<bool()> pred)
  {
    Lock l(mutex);
    while (!pred())
    {
      wait();
    }
  }

  int Condition::signal()
  {
    return pthread_cond_signal(cnd);
  }

  Lock::Lock(Mutex &mtx_) : mtx(mtx_)
  {
    mtx.lock();
  }

  Lock::~Lock()
  {
    mtx.unlock();
  }

  Promise::Promise()
  {
    is_ready = false;
  }

  void Promise::set_value(int value_)
  {
    Lock l(cnd.mutex);
    is_ready = true;
    value = value_;
    cnd.signal();
  }

  int Promise::get_value()
  {
    cnd.wait_for([&]() { return is_ready; });
    is_ready = false;
    return value;
  }

  void Promise::reset()
  {
    is_ready = false;
  }

};
