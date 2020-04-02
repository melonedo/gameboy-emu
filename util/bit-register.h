// Registers in gameboy are manipulated in bit granularity,
// but only supports read and writes in bytes.
// Also, some bits are read-only to the CPU, and can only be manipulated
// by specific modules

#ifndef BIT_REGISTER_H_INCLUDED
#define BIT_REGISTER_H_INCLUDED

#include <bitset>
#include <string>
#include "byte-type.h"

namespace gameboy
{
  // Set only writable bits
  byte_t write_controled(byte_t orig, byte_t val, byte_t writable);

  class BitRegister
  {
  private:
    // Content
    std::bitset<8> bits;
    // Bits that are writable in normal writes
    std::bitset<8> writable;
  public:
    BitRegister(std::bitset<8> bits_, std::bitset<8> writable_)
      : bits(bits_), writable(writable_) {}

    // Read the content
    byte_t read();

    // Write, respecting writable
    void write(byte_t);

    // Write, disregard writable
    void write_privileged(byte_t);

    // Manipulate individual bits
    std::bitset<8>::reference get_bit(size_t);

    // String representation
    std::string to_string();
  };
};


#endif
