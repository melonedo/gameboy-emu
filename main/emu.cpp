
#include <cstdio>
#include <vector>
#include <fstream>
#include <cstring>
#include <algorithm>
#include "threads.h"
#include "../util/byte-type.h"
#include "../memory/memory.h"

namespace gameboy
{
  pthread_t emulator_thread;

  std::vector<byte_t> rom_buf;

  bool init_emulator(const char *rom_dir);

  void *emulator_main(void *dir)
  {
    begin_init();
    bool success = init_emulator(static_cast<const char *>(dir));
    end_init(success);
    if (!success)
      return NULL;

    while (!program_ended)
    {
      //
    }
    return NULL;
  }

  bool init_emulator(const char *rom_dir)
  {
    // First load the ROM
    try
    {
      std::ifstream file(rom_dir);
      if (!file.good())
      {
        printf("Error occurred when reading file %s. Wrong directory?\n",
          rom_dir);
        return false;
      }

      std::vector<byte_t> buf((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
      rom_buf = std::move(buf);
    }
    catch (const std::exception &e)
    {
      printf("Error occurred when reading rom: %s", e.what());
      return false;
    }

    if (rom_buf.size() > 0x8000)
    {
      printf("Memory bank controler is not supported!\n");
      return false;
    }

    copy_n(rom_buf.cbegin(), 0x4000, memory.begin());


    return true;
  }
};
