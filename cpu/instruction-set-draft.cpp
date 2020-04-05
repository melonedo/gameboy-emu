// See gen-exec-instr.py

#include <cstdint>
#include "cpu.h"

namespace gameboy
{
  uint8_t instruction_length[256] =
  {
    /*--- The data will go here ---*/
  };

  const char *disas_table[512] =
  {
    /*--- The disas will go here ---*/
  };

  int exec_instruction(byte_t opcode, byte_t opr8, dbyte_t opr16)
  {
    int clocks, opcode_extended;
    if (opcode == 0xcb)
    {
      opcode_extended = 0x100 + opr8;
    }
    else
    {
      opcode_extended = opcode;
    }

    using namespace instruction;
    switch (opcode_extended)
    {
      // case 0xcb: // See above

      case 0x08: // LD (a16), SP
      write_dbyte(opr16, reg.sp());
      clocks = 20;
      break;

      case 0xd9: // RETI
      RET();
      EI();
      clocks = 16;
      break;

      case 0xe8: // ADD SP,r8
      reg.sp() = ADDSP(reg.sp(), opr8);
      break;

      case 0xf8: // LD HL,SP+r8
      reg.hl() = ADDSP(reg.sp(), opr8);
      break;

      // case 0x10: // STOP
      // STOP is coded as 10 00, here I simply ignore the second byte

      /*--- More cases will go here ---*/

      default:
      return -1;
    }

    return clocks;
  }

};
