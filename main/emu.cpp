
#include <cstdio>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <pthread.h>
#include "threads.h"
#include "../util/byte-type.h"
#include "../memory/memory.h"
#include "../cpu/cpu.h"
#include "../video/video.h"

namespace gameboy
{
  pthread_t emulator_thread;
  std::vector<byte_t> rom_buf;
  long long oscillator;
  pthread_mutex_t oscillator_mutex;
  pthread_cond_t oscillator_cond;
  long long cpu_clock;
  long long video_next_event;
  bool debugger_on;
  pthread_mutex_t cpu_mutex;

  // 0 if no interrupt
  byte_t interrupt_address;

  enum {h_blank_clocks = 204, v_blank_clocks = 4560,
    sprite_search_clocks = 80, bg_render_clocks = 172};

  // Represent the currnt mode
  enum {h_blank = 0, sprite_search = 2, bg_render = 3} video_mode;

  bool init_emulator(const char *rom_dir);

  void emulator_step();

  void video_timing();

  void stat_interrupt();

  void load_predef_mem();

  byte_t write_interrupt_flag(dbyte_t, byte_t);

  void start_lcd();

  void *emulator_main(void *dir)
  {
    begin_init();
    bool success = init_emulator(static_cast<const char *>(dir));
    end_init(success);
    if (!success)
      return NULL;

    cpu_clock = oscillator = 0;
    load_predef_mem();

    while (!program_ended)
    {
      emulator_step();
    }
    return NULL;
  }

  bool init_emulator(const char *rom_dir)
  {
    // First load the ROM
    try
    {
      std::ifstream file(rom_dir, std::ios::binary);
      // std::streamsize size = file.tellg();
      // printf("Size of ROM is %zd\n", size);
      // if (!file.good() || size == 0)
      // {
      //   printf("Error occurred when reading file \"%s\". Wrong directory?\n",
      //     rom_dir);
      //   return false;
      // }
      // std::vector<char> buf(size);
      // if (file.read(buf.data(), size).bad())
      // {
      //   printf("Error occurred when reading rom.\n");
      //   return false;
      // }
      std::vector<byte_t> buf(std::istreambuf_iterator<char>(file), {});
      rom_buf = buf;
    }
    catch (const std::exception &e)
    {
      printf("Error occurred when reading rom: %s", e.what());
      return false;
    }

    if (rom_buf.size() == 0)
    {
      printf("Error occurred when reading rom. Size of ROM is 0.\n");
      return false;
    }

    if (rom_buf.size() > 0x8000)
    {
      printf("Memory bank controler is not supported!\n");
      return false;
    }

    memcpy(memory.begin(), rom_buf.data(), 0x8000 * sizeof(byte_t));
    // copy_n(rom_buf.cbegin(), 0x4000, memory.begin());

    return true;
  }

  void emulator_step()
  {
    if (debugger_on)
    {
      show_status();

    }

    pthread_mutex_lock(&cpu_mutex);
    if (interrupt_address)
    {
      if (debugger_on)
      {
        printf("Handle interrupt %d\n", (interrupt_address - 0x40) / 8);
        printf("RST %.2hhx\n", interrupt_address);
      }

      instruction::RST(interrupt_address);
      cpu_clock += 16;
      interrupt_master = false;
      interrupt_address = 0;
    }
    else
    {
      if (debugger_on)
        printf("%s\n", get_disas().c_str());
      byte_t opcode, op8;
      dbyte_t op16;
      fetch_instruction(&opcode, &op8, &op16);
      cpu_clock += exec_instruction(opcode, op8, op16);
    }
    pthread_mutex_unlock(&cpu_mutex);

    video_timing();

    // Wait for clock
    if (cpu_clock >= oscillator && !program_ended)
    {
      pthread_mutex_lock(&oscillator_mutex);
      do {
        pthread_cond_wait(&oscillator_cond, &oscillator_mutex);
      } while (cpu_clock >= oscillator && !program_ended);
      pthread_mutex_unlock(&oscillator_mutex);
    }
  }

  void show_status()
  {
    printf("AF:%.4x BC:%.4x DE:%.4x HL:%.4x PC:%.4x SP:%.4x\n",
      reg.af(), reg.bc(), reg.de(), reg.hl(), reg.pc(), reg.sp(), cpu_clock);
    printf("LCDC:%.2hhx STAT:%.2hhx LY:%.2hhx clock:%lld\n",
      memory.at(LCDC), memory.at(STAT), memory.at(LY), cpu_clock);
    printf("[%.4hx] %.2hhx %.2hhx %.2hhx %.2hhx %.2hhx\n", reg.pc(),
      memory.at(reg.pc()), memory.at(reg.pc()+1), memory.at(reg.pc()+2),
      memory.at(reg.pc()+3), memory.at(reg.pc()+4));
  }

  // Vertical blank is just 10 normal lines
  const int v_blank_lines = 10;

  void video_timing()
  {
    if (lcd_on && cpu_clock >= video_next_event)
    {
      byte_t ly = memory.at(LY);
      byte_t stat = memory.at(STAT) & ~0b111;
      if (video_mode == h_blank)
      {
        video_mode = sprite_search;
        video_next_event += sprite_search_clocks;
        ly = (ly + 1) % (screen_row_num + v_blank_lines);
        memory.at(LY) = ly;
        printf("========== ly=%.2x\n", ly);
        if (stat & (1 << 3))
        {
          stat_interrupt();
        }
        byte_t lyc = memory.at(LYC);
        if (lyc == ly)
        {
          stat |= 0b100;
          if (stat & (1 << 6))
          {
            stat_interrupt();
          }
        }
      }
      else if (video_mode == sprite_search)
      {
        video_mode = bg_render;
        video_next_event += bg_render_clocks;
        if (stat & (1 << 5))
        {
          stat_interrupt();
        }
      }
      else // bg_render
      {
        video_mode = h_blank;
        video_next_event += h_blank_clocks;
        if (ly < screen_row_num)
        {
          render_row(ly);
        }
      }

      if (ly < screen_row_num)
      {
        stat |= video_mode;
      }
      else if (ly < screen_row_num + v_blank_lines)
      {
        stat |= 2;
        if (ly == screen_row_num)
        {
          // The first of 10 lines in vertical blank
          // Set vertical blank flag
          mem_ref(IF) = 1 | memory.at(IF);
          if (stat & (1 << 4))
          {
            stat_interrupt();
          }
        }
      }

      // Set bit 7 to 1
      memory.at(STAT) = 0x80 | stat;
    }
  }

  byte_t write_interrupt_flag(dbyte_t addr, byte_t val)
  {
    byte_t old = memory.at(IF);
    byte_t falling_edge = ~val & old;
    byte_t events = falling_edge & memory.at(IE);

    if (events && interrupt_master)
    {
      for (byte_t i = 0; i < 5; i++)
      {
        if (events & (1 << i))
        {
          interrupt_address = 0x40 + 8 * i;
          printf("Interrupt %d\n", i);
        }
      }
    }

    return 0xe0 | val;
  }

  void load_predef_mem()
  {
    mem_ref(0xFF05) = 0x00; // TIMA
    mem_ref(0xFF06) = 0x00; // TMA
    mem_ref(0xFF07) = 0x00; // TAC
    mem_ref(0xFF10) = 0x80; // NR10
    mem_ref(0xFF11) = 0xBF; // NR11
    mem_ref(0xFF12) = 0xF3; // NR12
    mem_ref(0xFF14) = 0xBF; // NR14
    mem_ref(0xFF16) = 0x3F; // NR21
    mem_ref(0xFF17) = 0x00; // NR22
    mem_ref(0xFF19) = 0xBF; // NR24
    mem_ref(0xFF1A) = 0x7F; // NR30
    mem_ref(0xFF1B) = 0xFF; // NR31
    mem_ref(0xFF1C) = 0x9F; // NR32
    mem_ref(0xFF1E) = 0xBF; // NR33
    mem_ref(0xFF20) = 0xFF; // NR41
    mem_ref(0xFF21) = 0x00; // NR42
    mem_ref(0xFF22) = 0x00; // NR43
    mem_ref(0xFF23) = 0xBF; // NR30
    mem_ref(0xFF24) = 0x77; // NR50
    mem_ref(0xFF25) = 0xF3; // NR51
    mem_ref(0xFF26) = 0xF1; // NR52
    mem_ref(0xFF40) = 0x91; // LCDC
    mem_ref(0xFF42) = 0x00; // SCY
    mem_ref(0xFF43) = 0x00; // SCX
    mem_ref(0xFF45) = 0x00; // LYC
    mem_ref(0xFF47) = 0xFC; // BGP
    mem_ref(0xFF48) = 0xFF; // OBP0
    mem_ref(0xFF49) = 0xFF; // OBP1
    mem_ref(0xFF4A) = 0x00; // WY
    mem_ref(0xFF4B) = 0x00; // WX
    mem_ref(0xFFFF) = 0x00; // IE
  }

  void stat_interrupt()
  {
    byte_t old = memory.at(IF);
    mem_ref(IF) = old & (1 << 1);
  }

  void start_lcd()
  {
    video_next_event = sprite_search_clocks + cpu_clock;
    video_mode = sprite_search;
    memory.at(LY) = 0;
  }
};
