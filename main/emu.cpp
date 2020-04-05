
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

namespace gameboy
{
  pthread_t emulator_thread;
  std::vector<byte_t> rom_buf;
  long oscillator;
  pthread_mutex_t oscillator_mutex;
  pthread_cond_t oscillator_cond;
  long cpu_clock;
  bool show_disas;

  bool init_emulator(const char *rom_dir);

  void emulator_step();

  void *emulator_main(void *dir)
  {
    begin_init();
    bool success = init_emulator(static_cast<const char *>(dir));
    end_init(success);
    if (!success)
      return NULL;

    cpu_clock = oscillator = 0;
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

    memcpy(memory.begin(), rom_buf.data(), 0x4000 * sizeof(byte_t));
    // copy_n(rom_buf.cbegin(), 0x4000, memory.begin());

    return true;
  }

  void emulator_step()
  {
    if (show_disas)
    {
      printf("%s\n", get_disas().c_str());
    }
    byte_t opcode, op8;
    dbyte_t op16;
    fetch_instruction(&opcode, &op8, &op16);
    cpu_clock += exec_instruction(opcode, op8, op16);

    pthread_mutex_lock(&oscillator_mutex);
    while (cpu_clock >= oscillator && !program_ended)
    {
      pthread_cond_wait(&oscillator_cond, &oscillator_mutex);
    }
    pthread_mutex_unlock(&oscillator_mutex);
  }
};
