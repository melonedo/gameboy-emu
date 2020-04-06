// Synchronize events, and render video

#include <cstdio>
#include <fstream>
#include <limits>
#include <set>
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

std::set<dbyte_t> breakpoints;

// Console interaction
void repl();

int main(int argc, char *argv[])
{
  pthread_mutex_init(&oscillator_mutex, NULL);
  pthread_cond_init(&oscillator_cond, NULL);
  pthread_mutex_init(&cpu_mutex, NULL);

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
  pthread_mutex_lock(&cpu_mutex);
  pthread_mutex_unlock(&cpu_mutex);
  pthread_mutex_destroy(&cpu_mutex);
  pthread_cond_destroy(&oscillator_cond);
  return 0;
}

void show_boot_rom();
void repl()
{
  printf("Entering step mode. Enter any number to increase the clock by that "
    "number of steps, or press enter to reuse last input. Enter 'q' to quit. "
    "Enter 'd' to toggle debug information. Enter 's' to show status once. "
    "Enter 'r' to run until next breakpoint. Enter 'b' to set new breakpoint."
    "Enter 'n' to delete all breakpoints. Enter 'm' to dump memory. "
    "Enter 'v' to view video buffer. \n");
  int step_len = 4;
  char c;

  debugger_on = false;
  while (!program_ended)
  {
    c = getchar();
    if (c == '\n')
    {
      ;
    }
    else if (c >= 'a' && c <= 'z')
    {
      switch (c)
      {
        case 'q':
        program_ended = true;
        break;

        case 'd':
        debugger_on = !debugger_on;
        printf("Debugger turned %s\n", debugger_on ? "on" : "off");
        if (debugger_on)
        {
          show_status();
          puts(get_disas().c_str());
        }
        break;

        case 's':
        show_status();
        puts(get_disas().c_str());
        break;

        case 'b':
        {
          dbyte_t addr;
          if (scanf("%hx", &addr) != 1)
          {
            printf("Invalid input!\n");
            continue;
          }
          breakpoints.insert(addr);
          printf("Breakpoints: ");
          for (dbyte_t i : breakpoints)
            printf("%.4hx ", i);
          putchar('\n');
        }
        break;

        case 'n':
        breakpoints.clear();
        printf("All breakpoints cleared.\n");
        break;

        case 'r':
        printf("Run until breakpoint.\n");
        while (true)
        {
          pthread_mutex_lock(&cpu_mutex);
          if (breakpoints.count(reg.pc()) != 0 || program_ended)
          {
            pthread_mutex_unlock(&cpu_mutex);
            break;
          }
          pthread_mutex_unlock(&cpu_mutex);
          if (cpu_clock >= oscillator)
          {
            pthread_mutex_lock(&oscillator_mutex);
            oscillator = cpu_clock + 1;
            pthread_cond_signal(&oscillator_cond);
            pthread_mutex_unlock(&oscillator_mutex);
          }
          else
            pthread_cond_signal(&oscillator_cond);
        }
        pthread_cond_signal(&oscillator_cond);
        show_status();
        puts(get_disas().c_str());
        // Go to next instruction
        oscillator = cpu_clock + 1;
        break;

        case 'm':
        {
          dbyte_t begin, end;
          if (scanf("%hx %hx", &begin, &end) != 2)
          {
            printf("Invalid input!\n");
            continue;
          }
          for (int i = begin; i < end; i++)
          {
            printf("%.2hhx ", memory.at(i));
            if ((i - begin) % 16 == 15)
            {
              printf("\n");
            }
          }
          if ((end - begin) % 16 != 0)
            printf("\n");
          break;
        }

        case 'v':
        for (int row = 0; row < screen_row_num; row++)
        {
          for (int col = 0; col < screen_column_num; col++)
          {
            printf("%d", screen_buf[row][col - 8]);
          }
          printf("\n");
        }
        break;

        default:
        printf("Unkown character: \'%c\'!\n", c);
      }
      while (c != '\n')
        c = getchar();
      continue;
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
    printf("Osc: %lld\n", oscillator);
  }
  // Emulator is probably still locked now
  pthread_mutex_lock(&oscillator_mutex);
  oscillator = std::numeric_limits<long long>::max();
  pthread_cond_signal(&oscillator_cond);
  pthread_mutex_unlock(&oscillator_mutex);
}

void show_boot_rom()
{
  for (int i = 0; i < 0x60; i++)
  {
    printf("%.2hhx ", memory.at(i));
  }
  putchar('\n');
}
