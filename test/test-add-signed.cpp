
#include <cstdio>
#include <cassert>
#include <cstdint>
#include "../util/byte-type.h"

int main()
{
  using namespace gameboy;
  for (byte_t i = 0; i < 16; i++)
  {
    for (byte_t j = 0; j < 16; j++)
    {
      byte_t x = i * 16 + j;
      printf("%.3hhx ", add_signed(256u, x));
      if (x & 0x80)
      {
        // Negative
        assert(add_signed(256u, x) == unsigned(256 - (256 - x)));
      }
      else
      {
        // Positive
        assert(add_signed(256u, x) == unsigned(256 + x));
      }
    }
    putchar('\n');
  }
}
