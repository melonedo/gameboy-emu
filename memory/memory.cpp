
#include <array>
#include "memory.h"
#include "../util/byte-type.h"

namespace gameboy
{
  std::array<byte_t, 0x10000> memory;

  MemoryReference mem_ref(dbyte_t addr)
  {
    return MemoryReference(addr);
  }

  byte_t MemoryReference::read() const
  {
    return memory.at(addr);
  }
  
  void MemoryReference::write(byte_t val)
  {
    if (addr < 0x8000)
    {
      // ROM
      return; // Some more could happen later on
    }
    else
    {
      memory.at(addr) = val;
    }
  }

  MemoryReference::operator byte_t() const
  {
    return read();
  }

  MemoryReference &MemoryReference::operator =(byte_t val)
  {
    write(val);
    return *this;
  }
};
