
#include <cstdint>
#include <string>
#include <cstdio>
#include "cpu.h"
#include "../memory/memory.h"
#include "../main/threads.h"

namespace gameboy
{
  Registers reg;
  bool interrupt_master;

  void fetch_instruction(byte_t *opcode, byte_t *op8, dbyte_t *op16)
  {
    *opcode = memory.at(reg.pc());
    int len = instruction_length[*opcode];
    if (len == 2)
    {
      *op8 = memory.at(reg.pc() + 1);
    }
    else if (len == 3)
    {
      *op16 = memory.at(reg.pc() + 2);
      *op16 <<= 8;
      *op16 |= memory.at(reg.pc() + 1);
    }
    reg.pc() += len;
  }

  std::string get_disas()
  {
    using std::string;
    using std::to_string;
    byte_t opcode = memory.at(reg.pc());
    int len = instruction_length[opcode];
    if (len == 1)
    {
      return string(disas_table[opcode]);
    }
    else if (len == 2)
    {
      byte_t op8 = memory.at(reg.pc() + 1);
      if (opcode == 0xcb)
      {
        return string(disas_table[0x100 + op8]);
      }
      else
      {
        char buf[4];
        sprintf(buf, "%.2hhx", op8);
        return string(disas_table[opcode]) + "; " + buf;
      }
    }
    else
    {
      dbyte_t op16 = read_dbyte(reg.pc() + 1);
      char buf[8];
      sprintf(buf, "%.4hx", op16);
      return string(disas_table[opcode]) + "; " + buf;
    }
  }

  namespace instruction
  {
    const byte_t zero_flag = 1 << 7, sub_flag = 1 << 6;
    const byte_t h_carry_flag = 1 << 5, carry_flag = 1 << 4;

    bool Z() { return reg.f() & zero_flag; }
    bool C() { return reg.f() & carry_flag; }
    bool NZ() { return !Z(); }
    bool NC() { return !C(); }

    void set_flag(bool zero, bool sub, bool h_carry, bool carry)
    {
      reg.f() = zero << 7 | sub << 6 | h_carry << 5 | carry << 4;
    }

    void NOP()
    {
      return;
    }

    // HALT and STOP requires more functionality to instantiate
    void HALT()
    {
      // puts("HALT");
      cpu_mode = cpu_mode_halt;
      return;
    }

    void STOP()
    {
      puts("STOP");
      cpu_mode = cpu_mode_stop;
      return;
    }

    void EI()
    {
      interrupt_master = true;
    }

    void DI()
    {
      interrupt_master = false;
    }

    void RET()
    {
      POP(reg.pc());
    }

    void RST(byte_t addr)
    {
      CALL(dbyte_t(addr));
    }

    void CALL(dbyte_t addr)
    {
      PUSH(reg.pc());
      reg.pc() = addr;
    }

    void JP(dbyte_t addr)
    {
      reg.pc() = addr;
    }

    void JR(byte_t offset)
    {
      // PC already incremented
      reg.pc() = add_signed(reg.pc(), offset);
    }

    void PUSH(dbyte_t val)
    {
      reg.sp() -= 2;
      write_dbyte(reg.sp(), val);
    }

    void POP(dbyte_t &dst)
    {
      dst = read_dbyte(reg.sp());
      reg.sp() += 2;
    }

    byte_t ADD(byte_t v1, byte_t v2)
    {
      int res = v1 + v2;
      set_flag(res == 0, false, (v1 & 0xf) + (v2 & 0xf) > 0x10, res > 0x100);
      return res;
    }

    byte_t ADC(byte_t v1, byte_t v2)
    {
      int res = v1 + v2 + C();
      set_flag(res == 0, false, (v1 & 0xf) + (v2 & 0xf) + C()> 0x10, res > 0x100);
      return res;
    }

    byte_t INC(byte_t val)
    {
      int res = val + 1;
      set_flag(res == 0, false, (val & 0xf) == 0xf, C());
      return res;
    }

    dbyte_t ADD(dbyte_t v1, dbyte_t v2)
    {
      int res = v1 + v2;
      set_flag(Z(), false, (v1 & 0xfff) + (v2 & 0xfff) > 0x1000, res > 0x10000);
      return res;
    }

    dbyte_t ADDSP(dbyte_t v1, byte_t v2)
    {
      set_flag(false, false, (v1 & 0xf) + (v2 & 0xf) > 0x10,
        (v1 & 0xff) + v2 > 0x100);
      return add_signed(v1, v2);
    }

    dbyte_t INC(dbyte_t val)
    {
      return val + 1;
    }

    // In Z80, the carry flag is actually the borrow flag in subtractions
    // But GB CPU man says it is no-borrow flag
    // I chose to use borrow flag because it is specified in GB Prog Man
    byte_t SUB(byte_t v1, byte_t v2)
    {
      int res = v1 - v2;
      set_flag(res == 0, true, (v1 & 0xf) < (v2 & 0xf), res < 0);
      return res;
    }

    byte_t SBC(byte_t v1, byte_t v2)
    {
      int res = v1 - (v2 + C());
      set_flag(res == 0, true, (v1 & 0xf) < (v2 & 0xf) + C(), res < 0);
      return res;
    }

    void CP(byte_t v1, byte_t v2)
    {
      int res = v1 - v2;
      set_flag(res == 0, true, (v1 & 0xf) < (v2 & 0xf), C());
    }

    byte_t DEC(byte_t val)
    {
      int res = val - 1;
      set_flag(res == 0, true, (val & 0xf) == 0, C());
      return res;
    }

    dbyte_t DEC(dbyte_t val)
    {
      return val - 1;
    }

    byte_t OR(byte_t v1, byte_t v2)
    {
      set_flag((v1 | v2) == 0, false, false, false);
      return v1 | v2;
    }

    byte_t AND(byte_t v1, byte_t v2)
    {
      set_flag((v1 & v2) == 0, false, true, false);
      return v1 & v2;
    }

    byte_t XOR(byte_t v1, byte_t v2)
    {
      set_flag((v1 ^ v2) == 0, false, false, false);
      return v1 ^ v2;
    }

    // RRC and RLC rotates val, then sets carry flag.
    // While RR and RL rotate the combination of carry flag and val
    byte_t RLC(byte_t val)
    {
      val = (val << 1) | (val >> 7);
      set_flag(val == 0, false, false, val & 1);
      return val;
    }

    byte_t RRC(byte_t val)
    {
      val = (val >> 1) | (val << 7);
      set_flag(val == 0, false, false, val & 0xf0);
      return val;
    }

    byte_t RL(byte_t val)
    {
      bool new_c = val & 0xf0;
      val = (val << 1) | C();
      set_flag(val == 0, false, false, new_c);
      return val;
    }

    byte_t RR(byte_t val)
    {
      bool new_c = val & 1;
      val = (val >> 1) | (C() << 7);
      set_flag(val == 0, false, false, new_c);
      return val;
    }

    byte_t SLA(byte_t val)
    {
      bool new_c = val & 0xf0;
      val <<= 1;
      set_flag(val == 0, false, false, new_c);
      return val;
    }

    byte_t SRL(byte_t val)
    {
      bool new_c = val & 1;
      val >>= 1;
      set_flag(val == 0, false, false, new_c);
      return val;
    }

    byte_t SRA(byte_t val)
    {
      static_assert((int8_t(-1) >> 1) == int8_t(-1),
        "Arithmetic shift right is not performed");
      bool new_c = val & 1;
      int8_t v = val;
      v >>= 1;
      set_flag(v == 0, false, false, new_c);
      return v;
    }

    byte_t SWAP(byte_t val)
    {
      val = (val << 4) | (val >> 4);
      set_flag(val == 0, false, false, false);
      return val;
    }

    void BIT(int n, byte_t val)
    {
      set_flag((val & (1 << n)) == 0, false, true, C());
    }

    byte_t SET(int n, byte_t val)
    {
      return val | (1 << n);
    }

    byte_t RES(int n, byte_t val)
    {
      return val & ~(1 << n);
    }

    byte_t CPL(byte_t val)
    {
      reg.f() |= h_carry_flag | sub_flag;
      return ~val;
    }

    void CCF()
    {
      reg.f() &= ~(h_carry_flag | sub_flag);
      reg.f() ^= carry_flag;
    }

    void SCF()
    {
      set_flag(Z(), false, false, false);
    }

    byte_t DAA(byte_t val)
    {
      // printf("[%.4hx] DAA %.2hhx with %s %s %s %s: ", reg.pc() - 1, reg.a(),
      //   Z() ? "Z" : "-",
      //   reg.f() & sub_flag ? "Sub" : "Add",
      //   reg.f() & h_carry_flag ? "H" : "-",
      //   C() ? "C" : "-");
      // Game Boy Programming Manual pp.122
      if (reg.f() & sub_flag)
      {
        // After subtraction (carry flag means borrowing)
        if (reg.f() & h_carry_flag)
        {
          val -= 6;
        }
        if (C())
        {
          val -= 0x60;
        }
        set_flag(val == 0, reg.f() & sub_flag, false, C());
      }
      else
      {
        // After addition
        if ((val & 0xf) > 9 || reg.f() & h_carry_flag)
        {
          // If last addition results in a decimal carry,
          // add 6 more to force a correct hexadecimal carry
          val += 6;
        }
        if ((val & 0xf0) > 0x90 || C())
        {
          val += 0x60;
          set_flag(val == 0, reg.f() & sub_flag, false, true);
        }
        else
        {
          set_flag(val == 0, reg.f() & sub_flag, false, false);
        }
      }
      // printf("%.2hhx\n", val);
      return val;
    }

  };

  Registers::Registers()
  {
    af() = 0x01B0;
    bc() = 0x0013;
    de() = 0x00D8;
    hl() = 0x014D;
    sp() = 0xFFFE;
    pc() = 0x0100;
  }

  byte_t &Registers::a()
  {
    return AF.high;
  }
  byte_t &Registers::f()
  {
    return AF.low;
  }
  dbyte_t &Registers::af()
  {
    return AF.dual;
  }

  byte_t &Registers::b()
  {
    return BC.high;
  }
  byte_t &Registers::c()
  {
    return BC.low;
  }
  dbyte_t &Registers::bc()
  {
    return BC.dual;
  }

  byte_t &Registers::d()
  {
    return DE.high;
  }
  byte_t &Registers::e()
  {
    return DE.low;
  }
  dbyte_t &Registers::de()
  {
    return DE.dual;
  }

  byte_t &Registers::h()
  {
    return HL.high;
  }
  byte_t &Registers::l()
  {
    return HL.low;
  }
  dbyte_t &Registers::hl()
  {
    return HL.dual;
  }

  dbyte_t &Registers::pc()
  {
    return PC;
  }

  dbyte_t &Registers::sp()
  {
    return SP;
  }
};
