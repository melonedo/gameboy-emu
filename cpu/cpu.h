
#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <cstdint>
#include "../util/byte-type.h"
#include "../memory/memory.h"

namespace gameboy
{
  // Fetches next instruction.
  // 8-bit operand goes to op8, 16-bit operand goes to op16.
  // Program counter is increased accordingly.
  void fetch_instruction(byte_t *opcode, byte_t *op8, dbyte_t *op16);

  // Execute given instruciton, return number of clocks needed
  int exec_instruction(byte_t opcode, byte_t op8, dbyte_t op16);

  // Length of each instruction, for use in fetch_instruction
  extern uint8_t instruction_length[256];

  // The registers
  class Registers
  {
  private:
    //Little endian
    //F A, C B, E D, L H, SP, PC
    union reg_pair
    {
      struct
      {
        byte_t low;
        byte_t high;
      };
      dbyte_t dual;
    }AF, BC, DE, HL;
    dbyte_t SP, PC;
  public:
    Registers();
    //Single byte_t registers
    byte_t &b(); byte_t &c(); byte_t &d(); byte_t &e();
    byte_t &a(); byte_t &f(); byte_t &h(); byte_t &l();
    //Register pairs
    dbyte_t &af(); dbyte_t &bc(); dbyte_t &de(); dbyte_t &hl();
    //Two byte_t registers
    dbyte_t &sp(); dbyte_t &pc();
  }reg;

  // More CPU state
  // 4 MHz clock which controls execution of commands (in a real gameboy)
  uint64_t cpu_clock;

  // IME flag, controls whether to ahndle an interrupt or not
  bool interrupt_master;

  // All the instructions go here.
  // Format dst = foo(...) where foo has side effect
  namespace instruction
  {
    void set_flag(bool zero, bool sub, bool h_carry, bool carry);

    // Conditions
    bool Z();
    bool C();
    bool NZ();
    bool NC();

    // Instructions
    void NOP();
    void STOP();
    void HALT();
    void EI();
    void DI();
    void CCF();
    void SCF();
    void RET();

    byte_t INC(byte_t);
    byte_t DEC(byte_t);
    byte_t RL(byte_t);
    byte_t RR(byte_t);
    byte_t RRC(byte_t);
    byte_t RLC(byte_t);
    byte_t SWAP(byte_t);
    byte_t SLA(byte_t);
    byte_t SRL(byte_t);
    byte_t SRA(byte_t);
    byte_t CPL(byte_t);
    byte_t DAA(byte_t);

    byte_t ADD(byte_t, byte_t);
    byte_t ADC(byte_t, byte_t);
    byte_t SUB(byte_t, byte_t);
    byte_t SBC(byte_t, byte_t);
    byte_t AND(byte_t, byte_t);
    byte_t OR(byte_t, byte_t);
    byte_t XOR(byte_t, byte_t);
    void CP(byte_t, byte_t);

    byte_t RES(int, byte_t);
    byte_t SET(int, byte_t);

    void BIT(int, byte_t);

    void JP(dbyte_t);
    void CALL(dbyte_t);
    void JR(byte_t);

    void PUSH(dbyte_t);
    void POP(dbyte_t &);

    void RST(byte_t);

    dbyte_t ADD(dbyte_t, dbyte_t);
    dbyte_t ADDSP(dbyte_t, byte_t); // Distinguish by flags
    dbyte_t DEC(dbyte_t);
    dbyte_t INC(dbyte_t);
  };

};

#endif
