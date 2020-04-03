
#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <array>
#include "../util/byte-type.h"

namespace gameboy
{
  // The whole memory is stored contiguously
  // Other module should directly access this
  extern std::array<byte_t, 0x10000> memory;

  // Memory Reference wrapper, basically for use in CPU
  class MemoryReference
  {
  private:
    // Address
    dbyte_t addr;
  public:
    MemoryReference(dbyte_t addr_)
      : addr(addr_) { }

    // Complicated memory write
    // Might involve write control, signal events, etc.
    void write(byte_t val);

    // Read from memory
    byte_t read() const;

    // Calls read()
    operator byte_t() const;

    // Calls write()
    MemoryReference &operator =(byte_t val);
  };

  // convenience function
  MemoryReference mem_ref(dbyte_t);
};

#endif
