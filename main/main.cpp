// Synchronize events, and render video

#include <cstdio>
#include <fstream>
#include <SDL.h>
#include <pthread.h>
#include "../util/byte-type.h"
#include "../memory/memory.h"
#include "../cpu/cpu.h"
#include "../video/video.h"
#include "threads.h"

using namespace gameboy;

// Emulator is run in another thread
const char *rom_dir = "testrom.gb";

int main(int argc, char *argv[])
{
  printf("Starting SDL.\n");
  bool success = init_and_wait(&window_thread, window_main, NULL);
  if (!success)
  {
    printf("Initilization failed!\n");
    program_ended = true;
    pthread_join(window_thread, NULL);
    return 0;
  }

  printf("Starting emulator. Load rom file \"%s\"\n", rom_dir);
  success = init_and_wait(&emulator_thread, emulator_main,
    const_cast<char *>(rom_dir));
  if (!success)
  {
    printf("Initilization failed!\n");
    program_ended = true;
    pthread_join(window_thread, NULL);
    pthread_join(emulator_thread, NULL);
    return 0;
  }
  printf("Checking ROM");

  getchar();


  program_ended = true;
  pthread_join(window_thread, NULL);
  pthread_join(emulator_thread, NULL);
  return 0;
}
