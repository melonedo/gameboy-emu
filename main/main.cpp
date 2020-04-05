// Synchronize events, and render video

#include <cstdio>
#include <fstream>
#include <limits>
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

// Console interaction
void repl();

int main(int argc, char *argv[])
{
  pthread_mutex_init(&oscillator_mutex, NULL);
  pthread_cond_init(&oscillator_cond, NULL);

  printf("Starting SDL.\n");
  bool success = init_and_wait(&window_thread, window_main, NULL);
  if (!success)
  {
    printf("Initilization failed!\n");
    goto free_pthread;
  }

  printf("Starting emulator. Load rom file \"%s\"\n", rom_dir);
  success = init_and_wait(&emulator_thread, emulator_main,
    const_cast<char *>(rom_dir));
  if (!success)
  {
    printf("Initilization failed!\n");
  }
  else
  {
    repl();
  }

  program_ended = true;
  pthread_join(emulator_thread, NULL);
  free_pthread:
  pthread_join(window_thread, NULL);
  pthread_mutex_lock(&oscillator_mutex);
  pthread_mutex_unlock(&oscillator_mutex);
  pthread_mutex_destroy(&oscillator_mutex);
  pthread_cond_destroy(&oscillator_cond);
  return 0;
}

void dump();
void show_boot_rom();
void repl()
{
  printf("Entering step mode. Enter any number to increase the clock by that "
    "number of steps, or press enter to reuse last input. Enter 'q' to quit.\n");
  int step_len = 4;
  char c;

  show_disas = true;
  show_boot_rom();
  while (!program_ended)
  {
    dump();
    c = getchar();
    if (c == '\n')
    {
      ;
    }
    else if (c == 'q')
    {
      break;
    }
    else
    {
      ungetc(c, stdin);
      int tmp;
      if (scanf("%d", &tmp) != 1)
      {
        printf("Invalid input!\n");
        while (c != '\n')
          c = getchar();
        continue;
      }
      else
      {
        // Consume subsequent '\n'
        getchar();
        step_len = tmp;
      }
    }
    pthread_mutex_lock(&oscillator_mutex);
    oscillator += step_len;
    pthread_cond_signal(&oscillator_cond);
    pthread_mutex_unlock(&oscillator_mutex);
  }
  // Emulator is probably still locked now
  pthread_mutex_lock(&oscillator_mutex);
  oscillator = std::numeric_limits<long>::max();
  pthread_cond_signal(&oscillator_cond);
  pthread_mutex_unlock(&oscillator_mutex);
}

void dump()
{
  printf("AF:%.4x BC:%.4x DE:%.4x HL:%.4x PC:%.4x SP:%.4x clock=%ld\n",
    reg.af(), reg.bc(), reg.de(), reg.hl(), reg.pc(), reg.sp(), cpu_clock);
}

void show_boot_rom()
{
  for (int i = 0; i < 0x60; i++)
  {
    printf("%.2hhx ", memory.at(i));
  }
  putchar('\n');
}
