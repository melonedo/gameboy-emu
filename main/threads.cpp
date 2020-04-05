
#include <pthread.h>
#include "threads.h"

namespace gameboy
{
  bool program_ended = false;

  pthread_mutex_t thread_init_mutex;
  pthread_cond_t thread_init_cond;
  bool thread_init_completed;
  int thread_init_retval;

  int init_and_wait(pthread_t *thrd, void *(*func)(void *), void *param)
  {
    // Reset completion indicator
    thread_init_completed = false;

    // Explicitly declare joinable
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(thrd, &attr, func, param);
    pthread_attr_destroy(&attr);

    pthread_mutex_init(&thread_init_mutex, NULL);
    pthread_cond_init(&thread_init_cond, NULL);

    pthread_mutex_lock(&thread_init_mutex);
    while (!thread_init_completed)
    {
      pthread_cond_wait(&thread_init_cond, &thread_init_mutex);
    }
    pthread_mutex_unlock(&thread_init_mutex);

    pthread_cond_destroy(&thread_init_cond);
    pthread_mutex_destroy(&thread_init_mutex);

    return thread_init_retval;
  }

  void begin_init()
  {
    pthread_mutex_lock(&thread_init_mutex);
  }

  void end_init(int retval)
  {
    thread_init_completed = true;
    thread_init_retval = retval;
    pthread_cond_signal(&thread_init_cond);
    pthread_mutex_unlock(&thread_init_mutex);
  }
};
