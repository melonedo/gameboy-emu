// See gen-exec-instr.py

#include <cstdint>
#include "cpu.h"

namespace gameboy
{
  uint8_t instruction_length[256] =
  {
    1, 3, 1, 1, 1, 1, 2, 1, 3, 1, 1, 1, 1, 1, 2, 1, 
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 
    2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 1, 3, 3, 2, 1, 
    1, 1, 3, 0, 3, 1, 2, 1, 1, 1, 3, 0, 3, 0, 2, 1, 
    2, 1, 2, 0, 0, 1, 2, 1, 2, 1, 3, 0, 0, 0, 2, 1, 
    2, 1, 2, 1, 0, 1, 2, 1, 2, 1, 3, 1, 0, 0, 2, 1
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
      mem_ref(opr16) = byte_t(reg.sp());
      mem_ref(opr16 + 1) = byte_t(reg.sp() >> 8);
      clocks = 20;
      break;

      case 0xd9: // RETI
      RET();
      EI();
      clocks = 16;
      break;

      case 0xe8: // ADD SP,r8
      reg.sp() = add_signed(reg.sp(), opr8);
      clocks = 16;
      break;

      case 0xf8: // LD HL,SP+r8
      reg.hl() = add_signed(reg.sp(), opr8);
      clocks = 12;
      break;

      case 0x0: // NOP 
      NOP();
      clocks = 4;
      break;

      case 0x1: // LD BC,d16
      reg.bc() = opr16;
      clocks = 12;
      break;

      case 0x2: // LD (BC),A
      mem_ref(reg.bc()) = reg.a();
      clocks = 8;
      break;

      case 0x3: // INC BC
      reg.bc() = INC(reg.bc());
      clocks = 8;
      break;

      case 0x4: // INC B
      reg.b() = INC(reg.b());
      clocks = 4;
      break;

      case 0x5: // DEC B
      reg.b() = DEC(reg.b());
      clocks = 4;
      break;

      case 0x6: // LD B,d8
      reg.b() = opr8;
      clocks = 8;
      break;

      case 0x7: // RLC A
      reg.a() = RLC(reg.a());
      clocks = 4;
      break;

      case 0x9: // ADD HL,BC
      reg.hl() = ADD(reg.hl(), reg.bc());
      clocks = 8;
      break;

      case 0xa: // LD A,(BC)
      reg.a() = mem_ref(reg.bc());
      clocks = 8;
      break;

      case 0xb: // DEC BC
      reg.bc() = DEC(reg.bc());
      clocks = 8;
      break;

      case 0xc: // INC C
      reg.c() = INC(reg.c());
      clocks = 4;
      break;

      case 0xd: // DEC C
      reg.c() = DEC(reg.c());
      clocks = 4;
      break;

      case 0xe: // LD C,d8
      reg.c() = opr8;
      clocks = 8;
      break;

      case 0xf: // RRC A
      reg.a() = RRC(reg.a());
      clocks = 4;
      break;

      case 0x10: // STOP 0
      STOP();
      clocks = 4;
      break;

      case 0x11: // LD DE,d16
      reg.de() = opr16;
      clocks = 12;
      break;

      case 0x12: // LD (DE),A
      mem_ref(reg.de()) = reg.a();
      clocks = 8;
      break;

      case 0x13: // INC DE
      reg.de() = INC(reg.de());
      clocks = 8;
      break;

      case 0x14: // INC D
      reg.d() = INC(reg.d());
      clocks = 4;
      break;

      case 0x15: // DEC D
      reg.d() = DEC(reg.d());
      clocks = 4;
      break;

      case 0x16: // LD D,d8
      reg.d() = opr8;
      clocks = 8;
      break;

      case 0x17: // RL A
      reg.a() = RL(reg.a());
      clocks = 4;
      break;

      case 0x18: // JR r8
      JR(opr8);
      clocks = 12;
      break;

      case 0x19: // ADD HL,DE
      reg.hl() = ADD(reg.hl(), reg.de());
      clocks = 8;
      break;

      case 0x1a: // LD A,(DE)
      reg.a() = mem_ref(reg.de());
      clocks = 8;
      break;

      case 0x1b: // DEC DE
      reg.de() = DEC(reg.de());
      clocks = 8;
      break;

      case 0x1c: // INC E
      reg.e() = INC(reg.e());
      clocks = 4;
      break;

      case 0x1d: // DEC E
      reg.e() = DEC(reg.e());
      clocks = 4;
      break;

      case 0x1e: // LD E,d8
      reg.e() = opr8;
      clocks = 8;
      break;

      case 0x1f: // RR A
      reg.a() = RR(reg.a());
      clocks = 4;
      break;

      case 0x20: // JR NZ,r8
      if (NZ())
      {
        JR(opr8);
        clocks = 12;
      }
      else
        clocks = 8;
      break;

      case 0x21: // LD HL,d16
      reg.hl() = opr16;
      clocks = 12;
      break;

      case 0x22: // LD (HL+),A
      mem_ref(reg.hl()++) = reg.a();
      clocks = 8;
      break;

      case 0x23: // INC HL
      reg.hl() = INC(reg.hl());
      clocks = 8;
      break;

      case 0x24: // INC H
      reg.h() = INC(reg.h());
      clocks = 4;
      break;

      case 0x25: // DEC H
      reg.h() = DEC(reg.h());
      clocks = 4;
      break;

      case 0x26: // LD H,d8
      reg.h() = opr8;
      clocks = 8;
      break;

      case 0x27: // DAA A
      reg.a() = DAA(reg.a());
      clocks = 4;
      break;

      case 0x28: // JR Z,r8
      if (Z())
      {
        JR(opr8);
        clocks = 12;
      }
      else
        clocks = 8;
      break;

      case 0x29: // ADD HL,HL
      reg.hl() = ADD(reg.hl(), reg.hl());
      clocks = 8;
      break;

      case 0x2a: // LD A,(HL+)
      reg.a() = mem_ref(reg.hl()++);
      clocks = 8;
      break;

      case 0x2b: // DEC HL
      reg.hl() = DEC(reg.hl());
      clocks = 8;
      break;

      case 0x2c: // INC L
      reg.l() = INC(reg.l());
      clocks = 4;
      break;

      case 0x2d: // DEC L
      reg.l() = DEC(reg.l());
      clocks = 4;
      break;

      case 0x2e: // LD L,d8
      reg.l() = opr8;
      clocks = 8;
      break;

      case 0x2f: // CPL A
      reg.a() = CPL(reg.a());
      clocks = 4;
      break;

      case 0x30: // JR NC,r8
      if (NC())
      {
        JR(opr8);
        clocks = 12;
      }
      else
        clocks = 8;
      break;

      case 0x31: // LD SP,d16
      reg.sp() = opr16;
      clocks = 12;
      break;

      case 0x32: // LD (HL-),A
      mem_ref(reg.hl()--) = reg.a();
      clocks = 8;
      break;

      case 0x33: // INC SP
      reg.sp() = INC(reg.sp());
      clocks = 8;
      break;

      case 0x34: // INC (HL)
      mem_ref(reg.hl()) = INC(mem_ref(reg.hl()));
      clocks = 12;
      break;

      case 0x35: // DEC (HL)
      mem_ref(reg.hl()) = DEC(mem_ref(reg.hl()));
      clocks = 12;
      break;

      case 0x36: // LD (HL),d8
      mem_ref(reg.hl()) = opr8;
      clocks = 12;
      break;

      case 0x37: // SCF 
      SCF();
      clocks = 4;
      break;

      case 0x38: // JR C,r8
      if (C())
      {
        JR(opr8);
        clocks = 12;
      }
      else
        clocks = 8;
      break;

      case 0x39: // ADD HL,SP
      reg.hl() = ADD(reg.hl(), reg.sp());
      clocks = 8;
      break;

      case 0x3a: // LD A,(HL-)
      reg.a() = mem_ref(reg.hl()--);
      clocks = 8;
      break;

      case 0x3b: // DEC SP
      reg.sp() = DEC(reg.sp());
      clocks = 8;
      break;

      case 0x3c: // INC A
      reg.a() = INC(reg.a());
      clocks = 4;
      break;

      case 0x3d: // DEC A
      reg.a() = DEC(reg.a());
      clocks = 4;
      break;

      case 0x3e: // LD A,d8
      reg.a() = opr8;
      clocks = 8;
      break;

      case 0x3f: // CCF 
      CCF();
      clocks = 4;
      break;

      case 0x40: // LD B,B
      reg.b() = reg.b();
      clocks = 4;
      break;

      case 0x41: // LD B,C
      reg.b() = reg.c();
      clocks = 4;
      break;

      case 0x42: // LD B,D
      reg.b() = reg.d();
      clocks = 4;
      break;

      case 0x43: // LD B,E
      reg.b() = reg.e();
      clocks = 4;
      break;

      case 0x44: // LD B,H
      reg.b() = reg.h();
      clocks = 4;
      break;

      case 0x45: // LD B,L
      reg.b() = reg.l();
      clocks = 4;
      break;

      case 0x46: // LD B,(HL)
      reg.b() = mem_ref(reg.hl());
      clocks = 8;
      break;

      case 0x47: // LD B,A
      reg.b() = reg.a();
      clocks = 4;
      break;

      case 0x48: // LD C,B
      reg.c() = reg.b();
      clocks = 4;
      break;

      case 0x49: // LD C,C
      reg.c() = reg.c();
      clocks = 4;
      break;

      case 0x4a: // LD C,D
      reg.c() = reg.d();
      clocks = 4;
      break;

      case 0x4b: // LD C,E
      reg.c() = reg.e();
      clocks = 4;
      break;

      case 0x4c: // LD C,H
      reg.c() = reg.h();
      clocks = 4;
      break;

      case 0x4d: // LD C,L
      reg.c() = reg.l();
      clocks = 4;
      break;

      case 0x4e: // LD C,(HL)
      reg.c() = mem_ref(reg.hl());
      clocks = 8;
      break;

      case 0x4f: // LD C,A
      reg.c() = reg.a();
      clocks = 4;
      break;

      case 0x50: // LD D,B
      reg.d() = reg.b();
      clocks = 4;
      break;

      case 0x51: // LD D,C
      reg.d() = reg.c();
      clocks = 4;
      break;

      case 0x52: // LD D,D
      reg.d() = reg.d();
      clocks = 4;
      break;

      case 0x53: // LD D,E
      reg.d() = reg.e();
      clocks = 4;
      break;

      case 0x54: // LD D,H
      reg.d() = reg.h();
      clocks = 4;
      break;

      case 0x55: // LD D,L
      reg.d() = reg.l();
      clocks = 4;
      break;

      case 0x56: // LD D,(HL)
      reg.d() = mem_ref(reg.hl());
      clocks = 8;
      break;

      case 0x57: // LD D,A
      reg.d() = reg.a();
      clocks = 4;
      break;

      case 0x58: // LD E,B
      reg.e() = reg.b();
      clocks = 4;
      break;

      case 0x59: // LD E,C
      reg.e() = reg.c();
      clocks = 4;
      break;

      case 0x5a: // LD E,D
      reg.e() = reg.d();
      clocks = 4;
      break;

      case 0x5b: // LD E,E
      reg.e() = reg.e();
      clocks = 4;
      break;

      case 0x5c: // LD E,H
      reg.e() = reg.h();
      clocks = 4;
      break;

      case 0x5d: // LD E,L
      reg.e() = reg.l();
      clocks = 4;
      break;

      case 0x5e: // LD E,(HL)
      reg.e() = mem_ref(reg.hl());
      clocks = 8;
      break;

      case 0x5f: // LD E,A
      reg.e() = reg.a();
      clocks = 4;
      break;

      case 0x60: // LD H,B
      reg.h() = reg.b();
      clocks = 4;
      break;

      case 0x61: // LD H,C
      reg.h() = reg.c();
      clocks = 4;
      break;

      case 0x62: // LD H,D
      reg.h() = reg.d();
      clocks = 4;
      break;

      case 0x63: // LD H,E
      reg.h() = reg.e();
      clocks = 4;
      break;

      case 0x64: // LD H,H
      reg.h() = reg.h();
      clocks = 4;
      break;

      case 0x65: // LD H,L
      reg.h() = reg.l();
      clocks = 4;
      break;

      case 0x66: // LD H,(HL)
      reg.h() = mem_ref(reg.hl());
      clocks = 8;
      break;

      case 0x67: // LD H,A
      reg.h() = reg.a();
      clocks = 4;
      break;

      case 0x68: // LD L,B
      reg.l() = reg.b();
      clocks = 4;
      break;

      case 0x69: // LD L,C
      reg.l() = reg.c();
      clocks = 4;
      break;

      case 0x6a: // LD L,D
      reg.l() = reg.d();
      clocks = 4;
      break;

      case 0x6b: // LD L,E
      reg.l() = reg.e();
      clocks = 4;
      break;

      case 0x6c: // LD L,H
      reg.l() = reg.h();
      clocks = 4;
      break;

      case 0x6d: // LD L,L
      reg.l() = reg.l();
      clocks = 4;
      break;

      case 0x6e: // LD L,(HL)
      reg.l() = mem_ref(reg.hl());
      clocks = 8;
      break;

      case 0x6f: // LD L,A
      reg.l() = reg.a();
      clocks = 4;
      break;

      case 0x70: // LD (HL),B
      mem_ref(reg.hl()) = reg.b();
      clocks = 8;
      break;

      case 0x71: // LD (HL),C
      mem_ref(reg.hl()) = reg.c();
      clocks = 8;
      break;

      case 0x72: // LD (HL),D
      mem_ref(reg.hl()) = reg.d();
      clocks = 8;
      break;

      case 0x73: // LD (HL),E
      mem_ref(reg.hl()) = reg.e();
      clocks = 8;
      break;

      case 0x74: // LD (HL),H
      mem_ref(reg.hl()) = reg.h();
      clocks = 8;
      break;

      case 0x75: // LD (HL),L
      mem_ref(reg.hl()) = reg.l();
      clocks = 8;
      break;

      case 0x76: // HALT 
      HALT();
      clocks = 4;
      break;

      case 0x77: // LD (HL),A
      mem_ref(reg.hl()) = reg.a();
      clocks = 8;
      break;

      case 0x78: // LD A,B
      reg.a() = reg.b();
      clocks = 4;
      break;

      case 0x79: // LD A,C
      reg.a() = reg.c();
      clocks = 4;
      break;

      case 0x7a: // LD A,D
      reg.a() = reg.d();
      clocks = 4;
      break;

      case 0x7b: // LD A,E
      reg.a() = reg.e();
      clocks = 4;
      break;

      case 0x7c: // LD A,H
      reg.a() = reg.h();
      clocks = 4;
      break;

      case 0x7d: // LD A,L
      reg.a() = reg.l();
      clocks = 4;
      break;

      case 0x7e: // LD A,(HL)
      reg.a() = mem_ref(reg.hl());
      clocks = 8;
      break;

      case 0x7f: // LD A,A
      reg.a() = reg.a();
      clocks = 4;
      break;

      case 0x80: // ADD A,B
      reg.a() = ADD(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0x81: // ADD A,C
      reg.a() = ADD(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0x82: // ADD A,D
      reg.a() = ADD(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0x83: // ADD A,E
      reg.a() = ADD(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0x84: // ADD A,H
      reg.a() = ADD(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0x85: // ADD A,L
      reg.a() = ADD(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0x86: // ADD A,(HL)
      reg.a() = ADD(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0x87: // ADD A,A
      reg.a() = ADD(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0x88: // ADC A,B
      reg.a() = ADC(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0x89: // ADC A,C
      reg.a() = ADC(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0x8a: // ADC A,D
      reg.a() = ADC(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0x8b: // ADC A,E
      reg.a() = ADC(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0x8c: // ADC A,H
      reg.a() = ADC(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0x8d: // ADC A,L
      reg.a() = ADC(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0x8e: // ADC A,(HL)
      reg.a() = ADC(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0x8f: // ADC A,A
      reg.a() = ADC(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0x90: // SUB B
      reg.a() = SUB(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0x91: // SUB C
      reg.a() = SUB(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0x92: // SUB D
      reg.a() = SUB(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0x93: // SUB E
      reg.a() = SUB(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0x94: // SUB H
      reg.a() = SUB(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0x95: // SUB L
      reg.a() = SUB(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0x96: // SUB (HL)
      reg.a() = SUB(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0x97: // SUB A
      reg.a() = SUB(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0x98: // SBC A,B
      reg.a() = SBC(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0x99: // SBC A,C
      reg.a() = SBC(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0x9a: // SBC A,D
      reg.a() = SBC(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0x9b: // SBC A,E
      reg.a() = SBC(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0x9c: // SBC A,H
      reg.a() = SBC(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0x9d: // SBC A,L
      reg.a() = SBC(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0x9e: // SBC A,(HL)
      reg.a() = SBC(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0x9f: // SBC A,A
      reg.a() = SBC(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0xa0: // AND B
      reg.a() = AND(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0xa1: // AND C
      reg.a() = AND(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0xa2: // AND D
      reg.a() = AND(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0xa3: // AND E
      reg.a() = AND(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0xa4: // AND H
      reg.a() = AND(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0xa5: // AND L
      reg.a() = AND(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0xa6: // AND (HL)
      reg.a() = AND(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0xa7: // AND A
      reg.a() = AND(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0xa8: // XOR B
      reg.a() = XOR(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0xa9: // XOR C
      reg.a() = XOR(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0xaa: // XOR D
      reg.a() = XOR(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0xab: // XOR E
      reg.a() = XOR(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0xac: // XOR H
      reg.a() = XOR(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0xad: // XOR L
      reg.a() = XOR(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0xae: // XOR (HL)
      reg.a() = XOR(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0xaf: // XOR A
      reg.a() = XOR(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0xb0: // OR B
      reg.a() = OR(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0xb1: // OR C
      reg.a() = OR(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0xb2: // OR D
      reg.a() = OR(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0xb3: // OR E
      reg.a() = OR(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0xb4: // OR H
      reg.a() = OR(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0xb5: // OR L
      reg.a() = OR(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0xb6: // OR (HL)
      reg.a() = OR(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0xb7: // OR A
      reg.a() = OR(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0xb8: // CP B
      CP(reg.a(), reg.b());
      clocks = 4;
      break;

      case 0xb9: // CP C
      CP(reg.a(), reg.c());
      clocks = 4;
      break;

      case 0xba: // CP D
      CP(reg.a(), reg.d());
      clocks = 4;
      break;

      case 0xbb: // CP E
      CP(reg.a(), reg.e());
      clocks = 4;
      break;

      case 0xbc: // CP H
      CP(reg.a(), reg.h());
      clocks = 4;
      break;

      case 0xbd: // CP L
      CP(reg.a(), reg.l());
      clocks = 4;
      break;

      case 0xbe: // CP (HL)
      CP(reg.a(), mem_ref(reg.hl()));
      clocks = 8;
      break;

      case 0xbf: // CP A
      CP(reg.a(), reg.a());
      clocks = 4;
      break;

      case 0xc0: // RET NZ
      if (NZ())
      {
        RET();
        clocks = 20;
      }
      else
        clocks = 8;
      break;

      case 0xc1: // POP BC
      POP(reg.bc());
      clocks = 12;
      break;

      case 0xc2: // JP NZ,a16
      if (NZ())
      {
        JP(opr16);
        clocks = 16;
      }
      else
        clocks = 12;
      break;

      case 0xc3: // JP a16
      JP(opr16);
      clocks = 16;
      break;

      case 0xc4: // CALL NZ,a16
      if (NZ())
      {
        CALL(opr16);
        clocks = 24;
      }
      else
        clocks = 12;
      break;

      case 0xc5: // PUSH BC
      PUSH(reg.bc());
      clocks = 16;
      break;

      case 0xc6: // ADD A,d8
      reg.a() = ADD(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xc7: // RST 00H
      RST(0x00);
      clocks = 16;
      break;

      case 0xc8: // RET Z
      if (Z())
      {
        RET();
        clocks = 20;
      }
      else
        clocks = 8;
      break;

      case 0xc9: // RET 
      RET();
      clocks = 16;
      break;

      case 0xca: // JP Z,a16
      if (Z())
      {
        JP(opr16);
        clocks = 16;
      }
      else
        clocks = 12;
      break;

      case 0xcc: // CALL Z,a16
      if (Z())
      {
        CALL(opr16);
        clocks = 24;
      }
      else
        clocks = 12;
      break;

      case 0xcd: // CALL a16
      CALL(opr16);
      clocks = 24;
      break;

      case 0xce: // ADC A,d8
      reg.a() = ADC(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xcf: // RST 08H
      RST(0x08);
      clocks = 16;
      break;

      case 0xd0: // RET NC
      if (NC())
      {
        RET();
        clocks = 20;
      }
      else
        clocks = 8;
      break;

      case 0xd1: // POP DE
      POP(reg.de());
      clocks = 12;
      break;

      case 0xd2: // JP NC,a16
      if (NC())
      {
        JP(opr16);
        clocks = 16;
      }
      else
        clocks = 12;
      break;

      // case 0xd3: // UNDEF

      case 0xd4: // CALL NC,a16
      if (NC())
      {
        CALL(opr16);
        clocks = 24;
      }
      else
        clocks = 12;
      break;

      case 0xd5: // PUSH DE
      PUSH(reg.de());
      clocks = 16;
      break;

      case 0xd6: // SUB d8
      reg.a() = SUB(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xd7: // RST 10H
      RST(0x10);
      clocks = 16;
      break;

      case 0xd8: // RET C
      if (C())
      {
        RET();
        clocks = 20;
      }
      else
        clocks = 8;
      break;

      case 0xda: // JP C,a16
      if (C())
      {
        JP(opr16);
        clocks = 16;
      }
      else
        clocks = 12;
      break;

      // case 0xdb: // UNDEF

      case 0xdc: // CALL C,a16
      if (C())
      {
        CALL(opr16);
        clocks = 24;
      }
      else
        clocks = 12;
      break;

      // case 0xdd: // UNDEF

      case 0xde: // SBC A,d8
      reg.a() = SBC(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xdf: // RST 18H
      RST(0x18);
      clocks = 16;
      break;

      case 0xe0: // LDH (a8),A
      mem_ref(0xff00 + opr8) = reg.a();
      clocks = 12;
      break;

      case 0xe1: // POP HL
      POP(reg.hl());
      clocks = 12;
      break;

      case 0xe2: // LD (C),A
      mem_ref(0xff00 + reg.c()) = reg.a();
      clocks = 8;
      break;

      // case 0xe3: // UNDEF

      // case 0xe4: // UNDEF

      case 0xe5: // PUSH HL
      PUSH(reg.hl());
      clocks = 16;
      break;

      case 0xe6: // AND d8
      reg.a() = AND(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xe7: // RST 20H
      RST(0x20);
      clocks = 16;
      break;

      case 0xe9: // JP HL
      JP(reg.hl());
      clocks = 4;
      break;

      case 0xea: // LD (a16),A
      mem_ref(opr16) = reg.a();
      clocks = 16;
      break;

      // case 0xeb: // UNDEF

      // case 0xec: // UNDEF

      // case 0xed: // UNDEF

      case 0xee: // XOR d8
      reg.a() = XOR(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xef: // RST 28H
      RST(0x28);
      clocks = 16;
      break;

      case 0xf0: // LDH A,(a8)
      reg.a() = mem_ref(0xff00 + opr8);
      clocks = 12;
      break;

      case 0xf1: // POP AF
      POP(reg.af());
      clocks = 12;
      break;

      case 0xf2: // LD A,(C)
      reg.a() = mem_ref(0xff00 + reg.c());
      clocks = 8;
      break;

      case 0xf3: // DI 
      DI();
      clocks = 4;
      break;

      // case 0xf4: // UNDEF

      case 0xf5: // PUSH AF
      PUSH(reg.af());
      clocks = 16;
      break;

      case 0xf6: // OR d8
      reg.a() = OR(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xf7: // RST 30H
      RST(0x30);
      clocks = 16;
      break;

      case 0xf9: // LD SP,HL
      reg.sp() = reg.hl();
      clocks = 8;
      break;

      case 0xfa: // LD A,(a16)
      reg.a() = mem_ref(opr16);
      clocks = 16;
      break;

      case 0xfb: // EI 
      EI();
      clocks = 4;
      break;

      // case 0xfc: // UNDEF

      // case 0xfd: // UNDEF

      case 0xfe: // CP d8
      CP(reg.a(), opr8);
      clocks = 8;
      break;

      case 0xff: // RST 38H
      RST(0x38);
      clocks = 16;
      break;

      case 0x100: // RLC B
      reg.b() = RLC(reg.b());
      clocks = 8;
      break;

      case 0x101: // RLC C
      reg.c() = RLC(reg.c());
      clocks = 8;
      break;

      case 0x102: // RLC D
      reg.d() = RLC(reg.d());
      clocks = 8;
      break;

      case 0x103: // RLC E
      reg.e() = RLC(reg.e());
      clocks = 8;
      break;

      case 0x104: // RLC H
      reg.h() = RLC(reg.h());
      clocks = 8;
      break;

      case 0x105: // RLC L
      reg.l() = RLC(reg.l());
      clocks = 8;
      break;

      case 0x106: // RLC (HL)
      mem_ref(reg.hl()) = RLC(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x107: // RLC A
      reg.a() = RLC(reg.a());
      clocks = 8;
      break;

      case 0x108: // RRC B
      reg.b() = RRC(reg.b());
      clocks = 8;
      break;

      case 0x109: // RRC C
      reg.c() = RRC(reg.c());
      clocks = 8;
      break;

      case 0x10a: // RRC D
      reg.d() = RRC(reg.d());
      clocks = 8;
      break;

      case 0x10b: // RRC E
      reg.e() = RRC(reg.e());
      clocks = 8;
      break;

      case 0x10c: // RRC H
      reg.h() = RRC(reg.h());
      clocks = 8;
      break;

      case 0x10d: // RRC L
      reg.l() = RRC(reg.l());
      clocks = 8;
      break;

      case 0x10e: // RRC (HL)
      mem_ref(reg.hl()) = RRC(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x10f: // RRC A
      reg.a() = RRC(reg.a());
      clocks = 8;
      break;

      case 0x110: // RL B
      reg.b() = RL(reg.b());
      clocks = 8;
      break;

      case 0x111: // RL C
      reg.c() = RL(reg.c());
      clocks = 8;
      break;

      case 0x112: // RL D
      reg.d() = RL(reg.d());
      clocks = 8;
      break;

      case 0x113: // RL E
      reg.e() = RL(reg.e());
      clocks = 8;
      break;

      case 0x114: // RL H
      reg.h() = RL(reg.h());
      clocks = 8;
      break;

      case 0x115: // RL L
      reg.l() = RL(reg.l());
      clocks = 8;
      break;

      case 0x116: // RL (HL)
      mem_ref(reg.hl()) = RL(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x117: // RL A
      reg.a() = RL(reg.a());
      clocks = 8;
      break;

      case 0x118: // RR B
      reg.b() = RR(reg.b());
      clocks = 8;
      break;

      case 0x119: // RR C
      reg.c() = RR(reg.c());
      clocks = 8;
      break;

      case 0x11a: // RR D
      reg.d() = RR(reg.d());
      clocks = 8;
      break;

      case 0x11b: // RR E
      reg.e() = RR(reg.e());
      clocks = 8;
      break;

      case 0x11c: // RR H
      reg.h() = RR(reg.h());
      clocks = 8;
      break;

      case 0x11d: // RR L
      reg.l() = RR(reg.l());
      clocks = 8;
      break;

      case 0x11e: // RR (HL)
      mem_ref(reg.hl()) = RR(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x11f: // RR A
      reg.a() = RR(reg.a());
      clocks = 8;
      break;

      case 0x120: // SLA B
      reg.b() = SLA(reg.b());
      clocks = 8;
      break;

      case 0x121: // SLA C
      reg.c() = SLA(reg.c());
      clocks = 8;
      break;

      case 0x122: // SLA D
      reg.d() = SLA(reg.d());
      clocks = 8;
      break;

      case 0x123: // SLA E
      reg.e() = SLA(reg.e());
      clocks = 8;
      break;

      case 0x124: // SLA H
      reg.h() = SLA(reg.h());
      clocks = 8;
      break;

      case 0x125: // SLA L
      reg.l() = SLA(reg.l());
      clocks = 8;
      break;

      case 0x126: // SLA (HL)
      mem_ref(reg.hl()) = SLA(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x127: // SLA A
      reg.a() = SLA(reg.a());
      clocks = 8;
      break;

      case 0x128: // SRA B
      reg.b() = SRA(reg.b());
      clocks = 8;
      break;

      case 0x129: // SRA C
      reg.c() = SRA(reg.c());
      clocks = 8;
      break;

      case 0x12a: // SRA D
      reg.d() = SRA(reg.d());
      clocks = 8;
      break;

      case 0x12b: // SRA E
      reg.e() = SRA(reg.e());
      clocks = 8;
      break;

      case 0x12c: // SRA H
      reg.h() = SRA(reg.h());
      clocks = 8;
      break;

      case 0x12d: // SRA L
      reg.l() = SRA(reg.l());
      clocks = 8;
      break;

      case 0x12e: // SRA (HL)
      mem_ref(reg.hl()) = SRA(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x12f: // SRA A
      reg.a() = SRA(reg.a());
      clocks = 8;
      break;

      case 0x130: // SWAP B
      reg.b() = SWAP(reg.b());
      clocks = 8;
      break;

      case 0x131: // SWAP C
      reg.c() = SWAP(reg.c());
      clocks = 8;
      break;

      case 0x132: // SWAP D
      reg.d() = SWAP(reg.d());
      clocks = 8;
      break;

      case 0x133: // SWAP E
      reg.e() = SWAP(reg.e());
      clocks = 8;
      break;

      case 0x134: // SWAP H
      reg.h() = SWAP(reg.h());
      clocks = 8;
      break;

      case 0x135: // SWAP L
      reg.l() = SWAP(reg.l());
      clocks = 8;
      break;

      case 0x136: // SWAP (HL)
      mem_ref(reg.hl()) = SWAP(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x137: // SWAP A
      reg.a() = SWAP(reg.a());
      clocks = 8;
      break;

      case 0x138: // SRL B
      reg.b() = SRL(reg.b());
      clocks = 8;
      break;

      case 0x139: // SRL C
      reg.c() = SRL(reg.c());
      clocks = 8;
      break;

      case 0x13a: // SRL D
      reg.d() = SRL(reg.d());
      clocks = 8;
      break;

      case 0x13b: // SRL E
      reg.e() = SRL(reg.e());
      clocks = 8;
      break;

      case 0x13c: // SRL H
      reg.h() = SRL(reg.h());
      clocks = 8;
      break;

      case 0x13d: // SRL L
      reg.l() = SRL(reg.l());
      clocks = 8;
      break;

      case 0x13e: // SRL (HL)
      mem_ref(reg.hl()) = SRL(mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x13f: // SRL A
      reg.a() = SRL(reg.a());
      clocks = 8;
      break;

      case 0x140: // BIT 0,B
      BIT(0, reg.b());
      clocks = 8;
      break;

      case 0x141: // BIT 0,C
      BIT(0, reg.c());
      clocks = 8;
      break;

      case 0x142: // BIT 0,D
      BIT(0, reg.d());
      clocks = 8;
      break;

      case 0x143: // BIT 0,E
      BIT(0, reg.e());
      clocks = 8;
      break;

      case 0x144: // BIT 0,H
      BIT(0, reg.h());
      clocks = 8;
      break;

      case 0x145: // BIT 0,L
      BIT(0, reg.l());
      clocks = 8;
      break;

      case 0x146: // BIT 0,(HL)
      BIT(0, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x147: // BIT 0,A
      BIT(0, reg.a());
      clocks = 8;
      break;

      case 0x148: // BIT 1,B
      BIT(1, reg.b());
      clocks = 8;
      break;

      case 0x149: // BIT 1,C
      BIT(1, reg.c());
      clocks = 8;
      break;

      case 0x14a: // BIT 1,D
      BIT(1, reg.d());
      clocks = 8;
      break;

      case 0x14b: // BIT 1,E
      BIT(1, reg.e());
      clocks = 8;
      break;

      case 0x14c: // BIT 1,H
      BIT(1, reg.h());
      clocks = 8;
      break;

      case 0x14d: // BIT 1,L
      BIT(1, reg.l());
      clocks = 8;
      break;

      case 0x14e: // BIT 1,(HL)
      BIT(1, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x14f: // BIT 1,A
      BIT(1, reg.a());
      clocks = 8;
      break;

      case 0x150: // BIT 2,B
      BIT(2, reg.b());
      clocks = 8;
      break;

      case 0x151: // BIT 2,C
      BIT(2, reg.c());
      clocks = 8;
      break;

      case 0x152: // BIT 2,D
      BIT(2, reg.d());
      clocks = 8;
      break;

      case 0x153: // BIT 2,E
      BIT(2, reg.e());
      clocks = 8;
      break;

      case 0x154: // BIT 2,H
      BIT(2, reg.h());
      clocks = 8;
      break;

      case 0x155: // BIT 2,L
      BIT(2, reg.l());
      clocks = 8;
      break;

      case 0x156: // BIT 2,(HL)
      BIT(2, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x157: // BIT 2,A
      BIT(2, reg.a());
      clocks = 8;
      break;

      case 0x158: // BIT 3,B
      BIT(3, reg.b());
      clocks = 8;
      break;

      case 0x159: // BIT 3,C
      BIT(3, reg.c());
      clocks = 8;
      break;

      case 0x15a: // BIT 3,D
      BIT(3, reg.d());
      clocks = 8;
      break;

      case 0x15b: // BIT 3,E
      BIT(3, reg.e());
      clocks = 8;
      break;

      case 0x15c: // BIT 3,H
      BIT(3, reg.h());
      clocks = 8;
      break;

      case 0x15d: // BIT 3,L
      BIT(3, reg.l());
      clocks = 8;
      break;

      case 0x15e: // BIT 3,(HL)
      BIT(3, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x15f: // BIT 3,A
      BIT(3, reg.a());
      clocks = 8;
      break;

      case 0x160: // BIT 4,B
      BIT(4, reg.b());
      clocks = 8;
      break;

      case 0x161: // BIT 4,C
      BIT(4, reg.c());
      clocks = 8;
      break;

      case 0x162: // BIT 4,D
      BIT(4, reg.d());
      clocks = 8;
      break;

      case 0x163: // BIT 4,E
      BIT(4, reg.e());
      clocks = 8;
      break;

      case 0x164: // BIT 4,H
      BIT(4, reg.h());
      clocks = 8;
      break;

      case 0x165: // BIT 4,L
      BIT(4, reg.l());
      clocks = 8;
      break;

      case 0x166: // BIT 4,(HL)
      BIT(4, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x167: // BIT 4,A
      BIT(4, reg.a());
      clocks = 8;
      break;

      case 0x168: // BIT 5,B
      BIT(5, reg.b());
      clocks = 8;
      break;

      case 0x169: // BIT 5,C
      BIT(5, reg.c());
      clocks = 8;
      break;

      case 0x16a: // BIT 5,D
      BIT(5, reg.d());
      clocks = 8;
      break;

      case 0x16b: // BIT 5,E
      BIT(5, reg.e());
      clocks = 8;
      break;

      case 0x16c: // BIT 5,H
      BIT(5, reg.h());
      clocks = 8;
      break;

      case 0x16d: // BIT 5,L
      BIT(5, reg.l());
      clocks = 8;
      break;

      case 0x16e: // BIT 5,(HL)
      BIT(5, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x16f: // BIT 5,A
      BIT(5, reg.a());
      clocks = 8;
      break;

      case 0x170: // BIT 6,B
      BIT(6, reg.b());
      clocks = 8;
      break;

      case 0x171: // BIT 6,C
      BIT(6, reg.c());
      clocks = 8;
      break;

      case 0x172: // BIT 6,D
      BIT(6, reg.d());
      clocks = 8;
      break;

      case 0x173: // BIT 6,E
      BIT(6, reg.e());
      clocks = 8;
      break;

      case 0x174: // BIT 6,H
      BIT(6, reg.h());
      clocks = 8;
      break;

      case 0x175: // BIT 6,L
      BIT(6, reg.l());
      clocks = 8;
      break;

      case 0x176: // BIT 6,(HL)
      BIT(6, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x177: // BIT 6,A
      BIT(6, reg.a());
      clocks = 8;
      break;

      case 0x178: // BIT 7,B
      BIT(7, reg.b());
      clocks = 8;
      break;

      case 0x179: // BIT 7,C
      BIT(7, reg.c());
      clocks = 8;
      break;

      case 0x17a: // BIT 7,D
      BIT(7, reg.d());
      clocks = 8;
      break;

      case 0x17b: // BIT 7,E
      BIT(7, reg.e());
      clocks = 8;
      break;

      case 0x17c: // BIT 7,H
      BIT(7, reg.h());
      clocks = 8;
      break;

      case 0x17d: // BIT 7,L
      BIT(7, reg.l());
      clocks = 8;
      break;

      case 0x17e: // BIT 7,(HL)
      BIT(7, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x17f: // BIT 7,A
      BIT(7, reg.a());
      clocks = 8;
      break;

      case 0x180: // RES 0,B
      reg.b() = RES(0, reg.b());
      clocks = 8;
      break;

      case 0x181: // RES 0,C
      reg.c() = RES(0, reg.c());
      clocks = 8;
      break;

      case 0x182: // RES 0,D
      reg.d() = RES(0, reg.d());
      clocks = 8;
      break;

      case 0x183: // RES 0,E
      reg.e() = RES(0, reg.e());
      clocks = 8;
      break;

      case 0x184: // RES 0,H
      reg.h() = RES(0, reg.h());
      clocks = 8;
      break;

      case 0x185: // RES 0,L
      reg.l() = RES(0, reg.l());
      clocks = 8;
      break;

      case 0x186: // RES 0,(HL)
      mem_ref(reg.hl()) = RES(0, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x187: // RES 0,A
      reg.a() = RES(0, reg.a());
      clocks = 8;
      break;

      case 0x188: // RES 1,B
      reg.b() = RES(1, reg.b());
      clocks = 8;
      break;

      case 0x189: // RES 1,C
      reg.c() = RES(1, reg.c());
      clocks = 8;
      break;

      case 0x18a: // RES 1,D
      reg.d() = RES(1, reg.d());
      clocks = 8;
      break;

      case 0x18b: // RES 1,E
      reg.e() = RES(1, reg.e());
      clocks = 8;
      break;

      case 0x18c: // RES 1,H
      reg.h() = RES(1, reg.h());
      clocks = 8;
      break;

      case 0x18d: // RES 1,L
      reg.l() = RES(1, reg.l());
      clocks = 8;
      break;

      case 0x18e: // RES 1,(HL)
      mem_ref(reg.hl()) = RES(1, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x18f: // RES 1,A
      reg.a() = RES(1, reg.a());
      clocks = 8;
      break;

      case 0x190: // RES 2,B
      reg.b() = RES(2, reg.b());
      clocks = 8;
      break;

      case 0x191: // RES 2,C
      reg.c() = RES(2, reg.c());
      clocks = 8;
      break;

      case 0x192: // RES 2,D
      reg.d() = RES(2, reg.d());
      clocks = 8;
      break;

      case 0x193: // RES 2,E
      reg.e() = RES(2, reg.e());
      clocks = 8;
      break;

      case 0x194: // RES 2,H
      reg.h() = RES(2, reg.h());
      clocks = 8;
      break;

      case 0x195: // RES 2,L
      reg.l() = RES(2, reg.l());
      clocks = 8;
      break;

      case 0x196: // RES 2,(HL)
      mem_ref(reg.hl()) = RES(2, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x197: // RES 2,A
      reg.a() = RES(2, reg.a());
      clocks = 8;
      break;

      case 0x198: // RES 3,B
      reg.b() = RES(3, reg.b());
      clocks = 8;
      break;

      case 0x199: // RES 3,C
      reg.c() = RES(3, reg.c());
      clocks = 8;
      break;

      case 0x19a: // RES 3,D
      reg.d() = RES(3, reg.d());
      clocks = 8;
      break;

      case 0x19b: // RES 3,E
      reg.e() = RES(3, reg.e());
      clocks = 8;
      break;

      case 0x19c: // RES 3,H
      reg.h() = RES(3, reg.h());
      clocks = 8;
      break;

      case 0x19d: // RES 3,L
      reg.l() = RES(3, reg.l());
      clocks = 8;
      break;

      case 0x19e: // RES 3,(HL)
      mem_ref(reg.hl()) = RES(3, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x19f: // RES 3,A
      reg.a() = RES(3, reg.a());
      clocks = 8;
      break;

      case 0x1a0: // RES 4,B
      reg.b() = RES(4, reg.b());
      clocks = 8;
      break;

      case 0x1a1: // RES 4,C
      reg.c() = RES(4, reg.c());
      clocks = 8;
      break;

      case 0x1a2: // RES 4,D
      reg.d() = RES(4, reg.d());
      clocks = 8;
      break;

      case 0x1a3: // RES 4,E
      reg.e() = RES(4, reg.e());
      clocks = 8;
      break;

      case 0x1a4: // RES 4,H
      reg.h() = RES(4, reg.h());
      clocks = 8;
      break;

      case 0x1a5: // RES 4,L
      reg.l() = RES(4, reg.l());
      clocks = 8;
      break;

      case 0x1a6: // RES 4,(HL)
      mem_ref(reg.hl()) = RES(4, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1a7: // RES 4,A
      reg.a() = RES(4, reg.a());
      clocks = 8;
      break;

      case 0x1a8: // RES 5,B
      reg.b() = RES(5, reg.b());
      clocks = 8;
      break;

      case 0x1a9: // RES 5,C
      reg.c() = RES(5, reg.c());
      clocks = 8;
      break;

      case 0x1aa: // RES 5,D
      reg.d() = RES(5, reg.d());
      clocks = 8;
      break;

      case 0x1ab: // RES 5,E
      reg.e() = RES(5, reg.e());
      clocks = 8;
      break;

      case 0x1ac: // RES 5,H
      reg.h() = RES(5, reg.h());
      clocks = 8;
      break;

      case 0x1ad: // RES 5,L
      reg.l() = RES(5, reg.l());
      clocks = 8;
      break;

      case 0x1ae: // RES 5,(HL)
      mem_ref(reg.hl()) = RES(5, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1af: // RES 5,A
      reg.a() = RES(5, reg.a());
      clocks = 8;
      break;

      case 0x1b0: // RES 6,B
      reg.b() = RES(6, reg.b());
      clocks = 8;
      break;

      case 0x1b1: // RES 6,C
      reg.c() = RES(6, reg.c());
      clocks = 8;
      break;

      case 0x1b2: // RES 6,D
      reg.d() = RES(6, reg.d());
      clocks = 8;
      break;

      case 0x1b3: // RES 6,E
      reg.e() = RES(6, reg.e());
      clocks = 8;
      break;

      case 0x1b4: // RES 6,H
      reg.h() = RES(6, reg.h());
      clocks = 8;
      break;

      case 0x1b5: // RES 6,L
      reg.l() = RES(6, reg.l());
      clocks = 8;
      break;

      case 0x1b6: // RES 6,(HL)
      mem_ref(reg.hl()) = RES(6, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1b7: // RES 6,A
      reg.a() = RES(6, reg.a());
      clocks = 8;
      break;

      case 0x1b8: // RES 7,B
      reg.b() = RES(7, reg.b());
      clocks = 8;
      break;

      case 0x1b9: // RES 7,C
      reg.c() = RES(7, reg.c());
      clocks = 8;
      break;

      case 0x1ba: // RES 7,D
      reg.d() = RES(7, reg.d());
      clocks = 8;
      break;

      case 0x1bb: // RES 7,E
      reg.e() = RES(7, reg.e());
      clocks = 8;
      break;

      case 0x1bc: // RES 7,H
      reg.h() = RES(7, reg.h());
      clocks = 8;
      break;

      case 0x1bd: // RES 7,L
      reg.l() = RES(7, reg.l());
      clocks = 8;
      break;

      case 0x1be: // RES 7,(HL)
      mem_ref(reg.hl()) = RES(7, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1bf: // RES 7,A
      reg.a() = RES(7, reg.a());
      clocks = 8;
      break;

      case 0x1c0: // SET 0,B
      reg.b() = SET(0, reg.b());
      clocks = 8;
      break;

      case 0x1c1: // SET 0,C
      reg.c() = SET(0, reg.c());
      clocks = 8;
      break;

      case 0x1c2: // SET 0,D
      reg.d() = SET(0, reg.d());
      clocks = 8;
      break;

      case 0x1c3: // SET 0,E
      reg.e() = SET(0, reg.e());
      clocks = 8;
      break;

      case 0x1c4: // SET 0,H
      reg.h() = SET(0, reg.h());
      clocks = 8;
      break;

      case 0x1c5: // SET 0,L
      reg.l() = SET(0, reg.l());
      clocks = 8;
      break;

      case 0x1c6: // SET 0,(HL)
      mem_ref(reg.hl()) = SET(0, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1c7: // SET 0,A
      reg.a() = SET(0, reg.a());
      clocks = 8;
      break;

      case 0x1c8: // SET 1,B
      reg.b() = SET(1, reg.b());
      clocks = 8;
      break;

      case 0x1c9: // SET 1,C
      reg.c() = SET(1, reg.c());
      clocks = 8;
      break;

      case 0x1ca: // SET 1,D
      reg.d() = SET(1, reg.d());
      clocks = 8;
      break;

      case 0x1cb: // SET 1,E
      reg.e() = SET(1, reg.e());
      clocks = 8;
      break;

      case 0x1cc: // SET 1,H
      reg.h() = SET(1, reg.h());
      clocks = 8;
      break;

      case 0x1cd: // SET 1,L
      reg.l() = SET(1, reg.l());
      clocks = 8;
      break;

      case 0x1ce: // SET 1,(HL)
      mem_ref(reg.hl()) = SET(1, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1cf: // SET 1,A
      reg.a() = SET(1, reg.a());
      clocks = 8;
      break;

      case 0x1d0: // SET 2,B
      reg.b() = SET(2, reg.b());
      clocks = 8;
      break;

      case 0x1d1: // SET 2,C
      reg.c() = SET(2, reg.c());
      clocks = 8;
      break;

      case 0x1d2: // SET 2,D
      reg.d() = SET(2, reg.d());
      clocks = 8;
      break;

      case 0x1d3: // SET 2,E
      reg.e() = SET(2, reg.e());
      clocks = 8;
      break;

      case 0x1d4: // SET 2,H
      reg.h() = SET(2, reg.h());
      clocks = 8;
      break;

      case 0x1d5: // SET 2,L
      reg.l() = SET(2, reg.l());
      clocks = 8;
      break;

      case 0x1d6: // SET 2,(HL)
      mem_ref(reg.hl()) = SET(2, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1d7: // SET 2,A
      reg.a() = SET(2, reg.a());
      clocks = 8;
      break;

      case 0x1d8: // SET 3,B
      reg.b() = SET(3, reg.b());
      clocks = 8;
      break;

      case 0x1d9: // SET 3,C
      reg.c() = SET(3, reg.c());
      clocks = 8;
      break;

      case 0x1da: // SET 3,D
      reg.d() = SET(3, reg.d());
      clocks = 8;
      break;

      case 0x1db: // SET 3,E
      reg.e() = SET(3, reg.e());
      clocks = 8;
      break;

      case 0x1dc: // SET 3,H
      reg.h() = SET(3, reg.h());
      clocks = 8;
      break;

      case 0x1dd: // SET 3,L
      reg.l() = SET(3, reg.l());
      clocks = 8;
      break;

      case 0x1de: // SET 3,(HL)
      mem_ref(reg.hl()) = SET(3, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1df: // SET 3,A
      reg.a() = SET(3, reg.a());
      clocks = 8;
      break;

      case 0x1e0: // SET 4,B
      reg.b() = SET(4, reg.b());
      clocks = 8;
      break;

      case 0x1e1: // SET 4,C
      reg.c() = SET(4, reg.c());
      clocks = 8;
      break;

      case 0x1e2: // SET 4,D
      reg.d() = SET(4, reg.d());
      clocks = 8;
      break;

      case 0x1e3: // SET 4,E
      reg.e() = SET(4, reg.e());
      clocks = 8;
      break;

      case 0x1e4: // SET 4,H
      reg.h() = SET(4, reg.h());
      clocks = 8;
      break;

      case 0x1e5: // SET 4,L
      reg.l() = SET(4, reg.l());
      clocks = 8;
      break;

      case 0x1e6: // SET 4,(HL)
      mem_ref(reg.hl()) = SET(4, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1e7: // SET 4,A
      reg.a() = SET(4, reg.a());
      clocks = 8;
      break;

      case 0x1e8: // SET 5,B
      reg.b() = SET(5, reg.b());
      clocks = 8;
      break;

      case 0x1e9: // SET 5,C
      reg.c() = SET(5, reg.c());
      clocks = 8;
      break;

      case 0x1ea: // SET 5,D
      reg.d() = SET(5, reg.d());
      clocks = 8;
      break;

      case 0x1eb: // SET 5,E
      reg.e() = SET(5, reg.e());
      clocks = 8;
      break;

      case 0x1ec: // SET 5,H
      reg.h() = SET(5, reg.h());
      clocks = 8;
      break;

      case 0x1ed: // SET 5,L
      reg.l() = SET(5, reg.l());
      clocks = 8;
      break;

      case 0x1ee: // SET 5,(HL)
      mem_ref(reg.hl()) = SET(5, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1ef: // SET 5,A
      reg.a() = SET(5, reg.a());
      clocks = 8;
      break;

      case 0x1f0: // SET 6,B
      reg.b() = SET(6, reg.b());
      clocks = 8;
      break;

      case 0x1f1: // SET 6,C
      reg.c() = SET(6, reg.c());
      clocks = 8;
      break;

      case 0x1f2: // SET 6,D
      reg.d() = SET(6, reg.d());
      clocks = 8;
      break;

      case 0x1f3: // SET 6,E
      reg.e() = SET(6, reg.e());
      clocks = 8;
      break;

      case 0x1f4: // SET 6,H
      reg.h() = SET(6, reg.h());
      clocks = 8;
      break;

      case 0x1f5: // SET 6,L
      reg.l() = SET(6, reg.l());
      clocks = 8;
      break;

      case 0x1f6: // SET 6,(HL)
      mem_ref(reg.hl()) = SET(6, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1f7: // SET 6,A
      reg.a() = SET(6, reg.a());
      clocks = 8;
      break;

      case 0x1f8: // SET 7,B
      reg.b() = SET(7, reg.b());
      clocks = 8;
      break;

      case 0x1f9: // SET 7,C
      reg.c() = SET(7, reg.c());
      clocks = 8;
      break;

      case 0x1fa: // SET 7,D
      reg.d() = SET(7, reg.d());
      clocks = 8;
      break;

      case 0x1fb: // SET 7,E
      reg.e() = SET(7, reg.e());
      clocks = 8;
      break;

      case 0x1fc: // SET 7,H
      reg.h() = SET(7, reg.h());
      clocks = 8;
      break;

      case 0x1fd: // SET 7,L
      reg.l() = SET(7, reg.l());
      clocks = 8;
      break;

      case 0x1fe: // SET 7,(HL)
      mem_ref(reg.hl()) = SET(7, mem_ref(reg.hl()));
      clocks = 16;
      break;

      case 0x1ff: // SET 7,A
      reg.a() = SET(7, reg.a());
      clocks = 8;
      break;

      default:
      return -1;
    }

    return clocks;
  }

};
