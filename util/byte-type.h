// Declare aliases of basic types
// and basic operations related to type changes

#ifndef BYTE_TYPE_H_INCLUDED
#define BYTE_TYPE_H_INCLUDED

#include <cstdint>

namespace gameboy
{
  typedef uint8_t byte_t;
  typedef uint16_t dbyte_t;

  // Treat offset as signed and add to base
  uint16_t add_signed(uint16_t base, byte_t offset);
  uint32_t add_signed(uint32_t base, byte_t offset);
};

#endif
