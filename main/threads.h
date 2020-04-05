// Thread related global variables

#include <pthread.h>

namespace gameboy
{
  // True when any of the threads requests termination
  extern bool program_ended;

  // Start a new thread, and wait for its initialization
  int init_and_wait(pthread_t *thrd, void *(*func)(void *), void *param);

  // Surround initialization with these two functions
  void begin_init();
  void end_init(int retval);


  // For the window thread
  const int window_width = 480, window_height = 432; // 1.5x zoom
  extern pthread_t window_thread;
  // Entry point
  void *window_main(void *);


  // For the emulator thread
  extern pthread_t emulator_thread;
  // Entry point
  // dir is the directory of rom.
  void *emulator_main(void *dir);


  // Virtual clock mimicking the gameboy clock
  extern long oscillator;
  extern pthread_mutex_t oscillator_mutex;
  extern pthread_cond_t oscillator_cond;

  // Increases after instructions are executed
  // If cpu_clock >= oscillator, cpu will hang
  extern long cpu_clock;

  extern bool show_disas;
};
