
#include <bitset>
#include <string>
#include "bit-register.h"
#include "byte-type.h"

namespace gameboy
{
  byte_t write_controled(byte_t orig, byte_t val, byte_t writable)
  {
    byte_t changed = writable & (orig ^ val);
    return orig ^ changed;
  }

  byte_t BitRegister::read()
  {
    return bits.to_ulong();
  }

  void BitRegister::write(byte_t byte)
  {
    std::bitset<8> changed;
    changed = writable & (std::bitset<8>(byte) ^ bits);
    bits ^= changed;
  }

  void BitRegister::write_privileged(byte_t byte)
  {
    bits = byte;
  }

  std::bitset<8>::reference BitRegister::get_bit(size_t ind)
  {
    return bits[ind];
  }

  std::string BitRegister::to_string()
  {
    return bits.to_string();
  }


};
