// Synchronize events, and render video

#include <cstdio>
#include <fstream>
#include <limits>
#include <set>
#include <SDL.h>
#include "../util/thread-util.h"
#include "../util/byte-type.h"
#include "../memory/memory.h"
#include "../cpu/cpu.h"
#include "../video/video.h"
#include "threads.h"

using namespace gameboy;

// Emulator is run in another thread
const char *rom_dir = "testrom.gb";

std::set<dbyte_t> breakpoints;

namespace gameboy
{
  bool program_ended = false;
  Promise emulator_init_promise;
  Promise window_init_promise;
};

// Console interaction
void repl();

// Increase oscillator until clocks is reached, keep synchronized with real time
void sync_oscillator(long long clocks);

int main(int argc, char *argv[])
{
  Thread emulator_thread(emulator_main);
  Thread window_thread(window_main);

  printf("Starting SDL.\n");
  window_thread.start(NULL);
  bool success = window_init_promise.get_value();
  if (!success)
  {
    printf("Initilization failed!\n");
  }
  else
  {
    printf("Starting emulator. Load rom file \"%s\"\n", rom_dir);
    emulator_thread.start(const_cast<char *>(rom_dir));
    success = emulator_init_promise.get_value();
    if (!success)
    {
      printf("Initilization failed!\n");
    }
    else
    {
      repl();
    }
  }

  program_ended = true;
  window_thread.join();
  emulator_thread.join();
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
    "Enter 'v' to view video buffer. Enter 'j' to simulate joypad. "
    "Enter 'g' to simply go and play!\n");
  long long step_len = 4;
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
          cpu_mutex.lock();
          if (breakpoints.count(reg.pc()) != 0 || program_ended)
          // if ((reg.pc() == 0x3270 && reg.a() == 0x82)|| program_ended)
          {
            cpu_mutex.unlock();
            break;
          }
          cpu_mutex.unlock();
          if (cpu_clock >= oscillator)
          {
            oscillator_cond.mutex.lock();
            oscillator = cpu_clock + 4;
            oscillator_cond.signal();
            oscillator_cond.mutex.unlock();
          }
          else
            oscillator_cond.signal();
        }
        oscillator_cond.signal();
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

        case 'j':
        {
          byte_t j;
          if (scanf("%x", &j) != 1)
          {
            printf("Invalid format!\n");
          }
          else
          {
            joypad = j;
            printf("Set joypad to %x.\n", joypad);
          }
        }
        break;

        case 'g':
        sync_oscillator(std::numeric_limits<long long>::max() - oscillator);
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
    sync_oscillator(oscillator + step_len);
  }
  // Emulator is probably still locked now
  oscillator_cond.mutex.lock();
  oscillator = std::numeric_limits<long long>::max();
  oscillator_cond.signal();
  oscillator_cond.mutex.unlock();
}

void show_boot_rom()
{
  for (int i = 0; i < 0x60; i++)
  {
    printf("%.2hhx ", memory.at(i));
  }
  putchar('\n');
}

const int frequency = 4000000;
const int clock_step = frequency / 1000;
void sync_oscillator(long long clocks)
{
  uint32_t ticks_start = SDL_GetTicks();
  long long osc_start = oscillator;
  while (oscillator < clocks && !program_ended)
  {
    while (!program_ended)
    {
      uint32_t ticks_past = SDL_GetTicks() - ticks_start;
      long long clocks_should_past = frequency / 1000 * ticks_past;
      if (clocks_should_past > oscillator - osc_start)
        break;
      SDL_Delay(1);
    }
    Lock l(oscillator_cond.mutex);
    oscillator += clock_step;
    if (oscillator > clocks)
    {
      oscillator = clocks;
    }
    oscillator_cond.signal();
  }
}
