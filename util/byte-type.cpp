
#include "byte-type.h"

namespace gameboy
{
  uint16_t add_signed(uint16_t base, byte_t offset)
  {
    return base + int8_t(offset);
  }

  uint32_t add_signed(uint32_t base, byte_t offset)
  {
    return base + int8_t(offset);
  }

};
