
#include <cstdio>
#include "../memory/memory.h"
#include "../util/byte-type.h"

using namespace gameboy;

// Register
byte_t a;

void test(dbyte_t addr)
{
  a = mem_ref(addr);
  printf("[%.4x] = %.2hhx\n", addr, a);
  a = 0xff;
  mem_ref(addr) = a;
  a = mem_ref(addr);
  printf("[%.4x] = %.2hhx\n", addr, a);
}

int main()
{
  test(0);
  test(0x8000);
}
