
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "../util/bit-register.h"
#include "../util/byte-type.h"

using namespace gameboy;

byte_t get_byte()
{
  return rand();
}

void show_byte(byte_t byte)
{
  BitRegister b(byte, 0);
  printf("%s", b.to_string().c_str());
}

void test()
{
  byte_t init_val = get_byte(), writable = get_byte(), set_val = get_byte();
  printf("Initial value of register: \n%.2hhx [", init_val);
  show_byte(init_val);
  printf("]\n");
  printf("Only the bits that are 1 are writable: \n%.2hhx [", writable);
  show_byte(writable);
  printf("]\n");
  printf("Attempt to write: \n%.2hhx [", set_val);
  show_byte(set_val);
  printf("]\n");
  BitRegister reg(init_val, writable);
  reg.write(set_val);
  printf("Current value of register: \n%.2hhx [%s]\n\n", reg.read(),
    reg.to_string().c_str());

  byte_t result = reg.read();
  assert((result & writable) == (set_val & writable));
  assert((result & ~writable) == (init_val & ~writable));
}

int main()
{
  srand(time(NULL));
  const int test_times = 3;
  for (int i = 0; i < test_times; i++)
    test();
  printf("Test of bit-register passed");
  return 0;
}
