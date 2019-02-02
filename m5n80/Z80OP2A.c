/**--------------------------------------------------------------------------
 **
 **		Z80 Emulator Ver 3.00
 **		Copyright (C)1993-2010 H.Kanemoto
 **
 **		This is a free software. You may copy or distribute
 **		it without any permissions. But there are NO WARRANTY
 **		of use. Entire risk of use is with you.
 **
 **------------------------------------------------------------------------*/
#include "z80.h"
#include "z80mac.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		Decoder
 **------------------------------------------------------------------------*/
export void _z80_decoder_cb(Z80CPU *cpu) {
	LPZ80REGS r = &cpu->r;

	switch (__z80_fetchb(cpu)) {
	case 0x00: /* RLC B		 */	__z80_microcode_rlc(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x01: /* RLC C		 */	__z80_microcode_rlc(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x02: /* RLC D		 */	__z80_microcode_rlc(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x03: /* RLC E		 */	__z80_microcode_rlc(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x04: /* RLC H		 */	__z80_microcode_rlc(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x05: /* RLC L		 */	__z80_microcode_rlc(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x06: /* RLC (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_rlc(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x07: /* RLC A		 */	__z80_microcode_rlc(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x08: /* RRC B		 */	__z80_microcode_rrc(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x09: /* RRC C		 */	__z80_microcode_rrc(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x0A: /* RRC D		 */	__z80_microcode_rrc(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x0B: /* RRC E		 */	__z80_microcode_rrc(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x0C: /* RRC H		 */	__z80_microcode_rrc(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x0D: /* RRC L		 */	__z80_microcode_rrc(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x0E: /* RRC (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_rrc(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x0F: /* RRC A		 */	__z80_microcode_rrc(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x10: /* RL B		 */	__z80_microcode_rl(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x11: /* RL C		 */	__z80_microcode_rl(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x12: /* RL D		 */	__z80_microcode_rl(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x13: /* RL E		 */	__z80_microcode_rl(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x14: /* RL H		 */	__z80_microcode_rl(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x15: /* RL L		 */	__z80_microcode_rl(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x16: /* RL (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_rl(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x17: /* RL A		 */	__z80_microcode_rl(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x18: /* RR B		 */	__z80_microcode_rr(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x19: /* RR C		 */	__z80_microcode_rr(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x1A: /* RR D		 */	__z80_microcode_rr(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x1B: /* RR E		 */	__z80_microcode_rr(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x1C: /* RR H		 */	__z80_microcode_rr(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x1D: /* RR L		 */	__z80_microcode_rr(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x1E: /* RR (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_rr(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x1F: /* RR A		 */	__z80_microcode_rr(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x20: /* SLA B		 */	__z80_microcode_sla(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x21: /* SLA C		 */	__z80_microcode_sla(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x22: /* SLA D		 */	__z80_microcode_sla(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x23: /* SLA E		 */	__z80_microcode_sla(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x24: /* SLA H		 */	__z80_microcode_sla(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x25: /* SLA L		 */	__z80_microcode_sla(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x26: /* SLA (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_sla(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x27: /* SLA A		 */	__z80_microcode_sla(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x28: /* SRA B		 */	__z80_microcode_sra(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x29: /* SRA C		 */	__z80_microcode_sra(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x2A: /* SRA D		 */	__z80_microcode_sra(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x2B: /* SRA E		 */	__z80_microcode_sra(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x2C: /* SRA H		 */	__z80_microcode_sra(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x2D: /* SRA L		 */	__z80_microcode_sra(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x2E: /* SRA (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_sra(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x2F: /* SRA A		 */	__z80_microcode_sra(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x30: /* SLL B		 */	__z80_microcode_sll(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x31: /* SLL C		 */	__z80_microcode_sll(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x32: /* SLL D		 */	__z80_microcode_sll(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x33: /* SLL E		 */	__z80_microcode_sll(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x34: /* SLL H		 */	__z80_microcode_sll(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x35: /* SLL L		 */	__z80_microcode_sll(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x36: /* SLL (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_sll(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x37: /* SLL A		 */	__z80_microcode_sll(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x38: /* SRL B		 */	__z80_microcode_srl(&r->h.B, &r->h.F); r->x.T = 8; break;
	case 0x39: /* SRL C		 */	__z80_microcode_srl(&r->h.C, &r->h.F); r->x.T = 8; break;
	case 0x3A: /* SRL D		 */	__z80_microcode_srl(&r->h.D, &r->h.F); r->x.T = 8; break;
	case 0x3B: /* SRL E		 */	__z80_microcode_srl(&r->h.E, &r->h.F); r->x.T = 8; break;
	case 0x3C: /* SRL H		 */	__z80_microcode_srl(&r->h.H, &r->h.F); r->x.T = 8; break;
	case 0x3D: /* SRL L		 */	__z80_microcode_srl(&r->h.L, &r->h.F); r->x.T = 8; break;
	case 0x3E: /* SRL (HL)	 */	{BYTE d = __z80_mbread(cpu, r->x.HL); __z80_microcode_srl(&d, &r->h.F); __z80_mbwrite(cpu, r->x.HL, d); r->x.T = 15;} break;
	case 0x3F: /* SRL A		 */	__z80_microcode_srl(&r->h.A, &r->h.F); r->x.T = 8; break;

	case 0x40: /* BIT 0,B	 */	__z80_microcode_bit(r->h.B, 0x01, &r->h.F);	r->x.T = 8; break;
	case 0x41: /* BIT 0,C	 */	__z80_microcode_bit(r->h.C, 0x01, &r->h.F);	r->x.T = 8; break;
	case 0x42: /* BIT 0,D	 */	__z80_microcode_bit(r->h.D, 0x01, &r->h.F);	r->x.T = 8; break;
	case 0x43: /* BIT 0,E	 */	__z80_microcode_bit(r->h.E, 0x01, &r->h.F);	r->x.T = 8; break;
	case 0x44: /* BIT 0,H	 */	__z80_microcode_bit(r->h.H, 0x01, &r->h.F);	r->x.T = 8; break;
	case 0x45: /* BIT 0,L	 */	__z80_microcode_bit(r->h.L, 0x01, &r->h.F);	r->x.T = 8; break;
	case 0x46: /* BIT 0,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x01, &r->h.F); r->x.T = 12; break;
	case 0x47: /* BIT 0,A	 */	__z80_microcode_bit(r->h.A, 0x01, &r->h.F);	r->x.T = 8; break;

	case 0x48: /* BIT 1,B	 */	__z80_microcode_bit(r->h.B, 0x02, &r->h.F);	r->x.T = 8; break;
	case 0x49: /* BIT 1,C	 */	__z80_microcode_bit(r->h.C, 0x02, &r->h.F);	r->x.T = 8; break;
	case 0x4A: /* BIT 1,D	 */	__z80_microcode_bit(r->h.D, 0x02, &r->h.F);	r->x.T = 8; break;
	case 0x4B: /* BIT 1,E	 */	__z80_microcode_bit(r->h.E, 0x02, &r->h.F);	r->x.T = 8; break;
	case 0x4C: /* BIT 1,H	 */	__z80_microcode_bit(r->h.H, 0x02, &r->h.F);	r->x.T = 8; break;
	case 0x4D: /* BIT 1,L	 */	__z80_microcode_bit(r->h.L, 0x02, &r->h.F);	r->x.T = 8; break;
	case 0x4E: /* BIT 1,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x02, &r->h.F); r->x.T = 12; break;
	case 0x4F: /* BIT 1,A	 */	__z80_microcode_bit(r->h.A, 0x02, &r->h.F);	r->x.T = 8; break;

	case 0x50: /* BIT 2,B	 */	__z80_microcode_bit(r->h.B, 0x04, &r->h.F);	r->x.T = 8; break;
	case 0x51: /* BIT 2,C	 */	__z80_microcode_bit(r->h.C, 0x04, &r->h.F);	r->x.T = 8; break;
	case 0x52: /* BIT 2,D	 */	__z80_microcode_bit(r->h.D, 0x04, &r->h.F);	r->x.T = 8; break;
	case 0x53: /* BIT 2,E	 */	__z80_microcode_bit(r->h.E, 0x04, &r->h.F);	r->x.T = 8; break;
	case 0x54: /* BIT 2,H	 */	__z80_microcode_bit(r->h.H, 0x04, &r->h.F);	r->x.T = 8; break;
	case 0x55: /* BIT 2,L	 */	__z80_microcode_bit(r->h.L, 0x04, &r->h.F);	r->x.T = 8; break;
	case 0x56: /* BIT 2,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x04, &r->h.F); r->x.T = 12; break;
	case 0x57: /* BIT 2,A	 */	__z80_microcode_bit(r->h.A, 0x04, &r->h.F);	r->x.T = 8; break;

	case 0x58: /* BIT 3,B	 */	__z80_microcode_bit(r->h.B, 0x08, &r->h.F);	r->x.T = 8; break;
	case 0x59: /* BIT 3,C	 */	__z80_microcode_bit(r->h.C, 0x08, &r->h.F);	r->x.T = 8; break;
	case 0x5A: /* BIT 3,D	 */	__z80_microcode_bit(r->h.D, 0x08, &r->h.F);	r->x.T = 8; break;
	case 0x5B: /* BIT 3,E	 */	__z80_microcode_bit(r->h.E, 0x08, &r->h.F);	r->x.T = 8; break;
	case 0x5C: /* BIT 3,H	 */	__z80_microcode_bit(r->h.H, 0x08, &r->h.F);	r->x.T = 8; break;
	case 0x5D: /* BIT 3,L	 */	__z80_microcode_bit(r->h.L, 0x08, &r->h.F);	r->x.T = 8; break;
	case 0x5E: /* BIT 3,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x08, &r->h.F); r->x.T = 12; break;
	case 0x5F: /* BIT 3,A	 */	__z80_microcode_bit(r->h.A, 0x08, &r->h.F);	r->x.T = 8; break;

	case 0x60: /* BIT 4,B	 */	__z80_microcode_bit(r->h.B, 0x10, &r->h.F);	r->x.T = 8; break;
	case 0x61: /* BIT 4,C	 */	__z80_microcode_bit(r->h.C, 0x10, &r->h.F);	r->x.T = 8; break;
	case 0x62: /* BIT 4,D	 */	__z80_microcode_bit(r->h.D, 0x10, &r->h.F);	r->x.T = 8; break;
	case 0x63: /* BIT 4,E	 */	__z80_microcode_bit(r->h.E, 0x10, &r->h.F);	r->x.T = 8; break;
	case 0x64: /* BIT 4,H	 */	__z80_microcode_bit(r->h.H, 0x10, &r->h.F);	r->x.T = 8; break;
	case 0x65: /* BIT 4,L	 */	__z80_microcode_bit(r->h.L, 0x10, &r->h.F);	r->x.T = 8; break;
	case 0x66: /* BIT 4,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x10, &r->h.F); r->x.T = 12; break;
	case 0x67: /* BIT 4,A	 */	__z80_microcode_bit(r->h.A, 0x10, &r->h.F);	r->x.T = 8; break;

	case 0x68: /* BIT 5,B	 */	__z80_microcode_bit(r->h.B, 0x20, &r->h.F);	r->x.T = 8; break;
	case 0x69: /* BIT 5,C	 */	__z80_microcode_bit(r->h.C, 0x20, &r->h.F);	r->x.T = 8; break;
	case 0x6A: /* BIT 5,D	 */	__z80_microcode_bit(r->h.D, 0x20, &r->h.F);	r->x.T = 8; break;
	case 0x6B: /* BIT 5,E	 */	__z80_microcode_bit(r->h.E, 0x20, &r->h.F);	r->x.T = 8; break;
	case 0x6C: /* BIT 5,H	 */	__z80_microcode_bit(r->h.H, 0x20, &r->h.F);	r->x.T = 8; break;
	case 0x6D: /* BIT 5,L	 */	__z80_microcode_bit(r->h.L, 0x20, &r->h.F);	r->x.T = 8; break;
	case 0x6E: /* BIT 5,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x20, &r->h.F); r->x.T = 12; break;
	case 0x6F: /* BIT 5,A	 */	__z80_microcode_bit(r->h.A, 0x20, &r->h.F);	r->x.T = 8; break;

	case 0x70: /* BIT 6,B	 */	__z80_microcode_bit(r->h.B, 0x40, &r->h.F);	r->x.T = 8; break;
	case 0x71: /* BIT 6,C	 */	__z80_microcode_bit(r->h.C, 0x40, &r->h.F);	r->x.T = 8; break;
	case 0x72: /* BIT 6,D	 */	__z80_microcode_bit(r->h.D, 0x40, &r->h.F);	r->x.T = 8; break;
	case 0x73: /* BIT 6,E	 */	__z80_microcode_bit(r->h.E, 0x40, &r->h.F);	r->x.T = 8; break;
	case 0x74: /* BIT 6,H	 */	__z80_microcode_bit(r->h.H, 0x40, &r->h.F);	r->x.T = 8; break;
	case 0x75: /* BIT 6,L	 */	__z80_microcode_bit(r->h.L, 0x40, &r->h.F);	r->x.T = 8; break;
	case 0x76: /* BIT 6,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x40, &r->h.F); r->x.T = 12; break;
	case 0x77: /* BIT 6,A	 */	__z80_microcode_bit(r->h.A, 0x40, &r->h.F);	r->x.T = 8; break;

	case 0x78: /* BIT 7,B	 */	__z80_microcode_bit(r->h.B, 0x80, &r->h.F);	r->x.T = 8; break;
	case 0x79: /* BIT 7,C	 */	__z80_microcode_bit(r->h.C, 0x80, &r->h.F);	r->x.T = 8; break;
	case 0x7A: /* BIT 7,D	 */	__z80_microcode_bit(r->h.D, 0x80, &r->h.F);	r->x.T = 8; break;
	case 0x7B: /* BIT 7,E	 */	__z80_microcode_bit(r->h.E, 0x80, &r->h.F);	r->x.T = 8; break;
	case 0x7C: /* BIT 7,H	 */	__z80_microcode_bit(r->h.H, 0x80, &r->h.F);	r->x.T = 8; break;
	case 0x7D: /* BIT 7,L	 */	__z80_microcode_bit(r->h.L, 0x80, &r->h.F);	r->x.T = 8; break;
	case 0x7E: /* BIT 7,(HL) */	__z80_microcode_bit(__z80_mbread(cpu, r->x.HL), 0x80, &r->h.F); r->x.T = 12; break;
	case 0x7F: /* BIT 7,A	 */	__z80_microcode_bit(r->h.A, 0x80, &r->h.F);	r->x.T = 8; break;

	case 0x80: /* RES 0,B	 */	r->h.B &= 0xfe; r->x.T = 8; break;
	case 0x81: /* RES 0,C	 */	r->h.C &= 0xfe; r->x.T = 8; break;
	case 0x82: /* RES 0,D	 */	r->h.D &= 0xfe; r->x.T = 8; break;
	case 0x83: /* RES 0,E	 */	r->h.E &= 0xfe; r->x.T = 8; break;
	case 0x84: /* RES 0,H	 */	r->h.H &= 0xfe; r->x.T = 8; break;
	case 0x85: /* RES 0,L	 */	r->h.L &= 0xfe; r->x.T = 8; break;
	case 0x86: /* RES 0,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0xfe)); r->x.T = 15; break;
	case 0x87: /* RES 0,A	 */	r->h.A &= 0xfe; r->x.T = 8; break;

	case 0x88: /* RES 1,B	 */	r->h.B &= 0xfd; r->x.T = 8; break;
	case 0x89: /* RES 1,C	 */	r->h.C &= 0xfd; r->x.T = 8; break;
	case 0x8A: /* RES 1,D	 */	r->h.D &= 0xfd; r->x.T = 8; break;
	case 0x8B: /* RES 1,E	 */	r->h.E &= 0xfd; r->x.T = 8; break;
	case 0x8C: /* RES 1,H	 */	r->h.H &= 0xfd; r->x.T = 8; break;
	case 0x8D: /* RES 1,L	 */	r->h.L &= 0xfd; r->x.T = 8; break;
	case 0x8E: /* RES 1,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0xfd)); r->x.T = 15; break;
	case 0x8F: /* RES 1,A	 */	r->h.A &= 0xfd; r->x.T = 8; break;

	case 0x90: /* RES 2,B	 */	r->h.B &= 0xfb; r->x.T = 8; break;
	case 0x91: /* RES 2,C	 */	r->h.C &= 0xfb; r->x.T = 8; break;
	case 0x92: /* RES 2,D	 */	r->h.D &= 0xfb; r->x.T = 8; break;
	case 0x93: /* RES 2,E	 */	r->h.E &= 0xfb; r->x.T = 8; break;
	case 0x94: /* RES 2,H	 */	r->h.H &= 0xfb; r->x.T = 8; break;
	case 0x95: /* RES 2,L	 */	r->h.L &= 0xfb; r->x.T = 8; break;
	case 0x96: /* RES 2,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0xfb)); r->x.T = 15; break;
	case 0x97: /* RES 2,A	 */	r->h.A &= 0xfb; r->x.T = 8; break;

	case 0x98: /* RES 3,B	 */	r->h.B &= 0xf7; r->x.T = 8; break;
	case 0x99: /* RES 3,C	 */	r->h.C &= 0xf7; r->x.T = 8; break;
	case 0x9A: /* RES 3,D	 */	r->h.D &= 0xf7; r->x.T = 8; break;
	case 0x9B: /* RES 3,E	 */	r->h.E &= 0xf7; r->x.T = 8; break;
	case 0x9C: /* RES 3,H	 */	r->h.H &= 0xf7; r->x.T = 8; break;
	case 0x9D: /* RES 3,L	 */	r->h.L &= 0xf7; r->x.T = 8; break;
	case 0x9E: /* RES 3,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0xf7)); r->x.T = 15; break;
	case 0x9F: /* RES 3,A	 */	r->h.A &= 0xf7; r->x.T = 8; break;

	case 0xA0: /* RES 4,B	 */	r->h.B &= 0xef; r->x.T = 8; break;
	case 0xA1: /* RES 4,C	 */	r->h.C &= 0xef; r->x.T = 8; break;
	case 0xA2: /* RES 4,D	 */	r->h.D &= 0xef; r->x.T = 8; break;
	case 0xA3: /* RES 4,E	 */	r->h.E &= 0xef; r->x.T = 8; break;
	case 0xA4: /* RES 4,H	 */	r->h.H &= 0xef; r->x.T = 8; break;
	case 0xA5: /* RES 4,L	 */	r->h.L &= 0xef; r->x.T = 8; break;
	case 0xA6: /* RES 4,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0xef)); r->x.T = 15; break;
	case 0xA7: /* RES 4,A	 */	r->h.A &= 0xef; r->x.T = 8; break;

	case 0xA8: /* RES 5,B	 */	r->h.B &= 0xdf; r->x.T = 8; break;
	case 0xA9: /* RES 5,C	 */	r->h.C &= 0xdf; r->x.T = 8; break;
	case 0xAA: /* RES 5,D	 */	r->h.D &= 0xdf; r->x.T = 8; break;
	case 0xAB: /* RES 5,E	 */	r->h.E &= 0xdf; r->x.T = 8; break;
	case 0xAC: /* RES 5,H	 */	r->h.H &= 0xdf; r->x.T = 8; break;
	case 0xAD: /* RES 5,L	 */	r->h.L &= 0xdf; r->x.T = 8; break;
	case 0xAE: /* RES 5,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0xdf)); r->x.T = 15; break;
	case 0xAF: /* RES 5,A	 */	r->h.A &= 0xdf; r->x.T = 8; break;

	case 0xB0: /* RES 6,B	 */	r->h.B &= 0xbf; r->x.T = 8; break;
	case 0xB1: /* RES 6,C	 */	r->h.C &= 0xbf; r->x.T = 8; break;
	case 0xB2: /* RES 6,D	 */	r->h.D &= 0xbf; r->x.T = 8; break;
	case 0xB3: /* RES 6,E	 */	r->h.E &= 0xbf; r->x.T = 8; break;
	case 0xB4: /* RES 6,H	 */	r->h.H &= 0xbf; r->x.T = 8; break;
	case 0xB5: /* RES 6,L	 */	r->h.L &= 0xbf; r->x.T = 8; break;
	case 0xB6: /* RES 6,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0xbf)); r->x.T = 15; break;
	case 0xB7: /* RES 6,A	 */	r->h.A &= 0xbf; r->x.T = 8; break;

	case 0xB8: /* RES 7,B	 */	r->h.B &= 0x7f; r->x.T = 8; break;
	case 0xB9: /* RES 7,C	 */	r->h.C &= 0x7f; r->x.T = 8; break;
	case 0xBA: /* RES 7,D	 */	r->h.D &= 0x7f; r->x.T = 8; break;
	case 0xBB: /* RES 7,E	 */	r->h.E &= 0x7f; r->x.T = 8; break;
	case 0xBC: /* RES 7,H	 */	r->h.H &= 0x7f; r->x.T = 8; break;
	case 0xBD: /* RES 7,L	 */	r->h.L &= 0x7f; r->x.T = 8; break;
	case 0xBE: /* RES 7,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) & 0x7f)); r->x.T = 15; break;
	case 0xBF: /* RES 7,A	 */	r->h.A &= 0x7f; r->x.T = 8; break;

	case 0xC0: /* SET 0,B	 */	r->h.B |= 0x01; r->x.T = 8; break;
	case 0xC1: /* SET 0,C	 */	r->h.C |= 0x01; r->x.T = 8; break;
	case 0xC2: /* SET 0,D	 */	r->h.D |= 0x01; r->x.T = 8; break;
	case 0xC3: /* SET 0,E	 */	r->h.E |= 0x01; r->x.T = 8; break;
	case 0xC4: /* SET 0,H	 */	r->h.H |= 0x01; r->x.T = 8; break;
	case 0xC5: /* SET 0,L	 */	r->h.L |= 0x01; r->x.T = 8; break;
	case 0xC6: /* SET 0,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x01)); r->x.T = 15; break;
	case 0xC7: /* SET 0,A	 */	r->h.A |= 0x01; r->x.T = 8; break;

	case 0xC8: /* SET 1,B	 */	r->h.B |= 0x02; r->x.T = 8; break;
	case 0xC9: /* SET 1,C	 */	r->h.C |= 0x02; r->x.T = 8; break;
	case 0xCA: /* SET 1,D	 */	r->h.D |= 0x02; r->x.T = 8; break;
	case 0xCB: /* SET 1,E	 */	r->h.E |= 0x02; r->x.T = 8; break;
	case 0xCC: /* SET 1,H	 */	r->h.H |= 0x02; r->x.T = 8; break;
	case 0xCD: /* SET 1,L	 */	r->h.L |= 0x02; r->x.T = 8; break;
	case 0xCE: /* SET 1,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x02)); r->x.T = 15; break;
	case 0xCF: /* SET 1,A	 */	r->h.A |= 0x02; r->x.T = 8; break;

	case 0xD0: /* SET 2,B	 */	r->h.B |= 0x04; r->x.T = 8; break;
	case 0xD1: /* SET 2,C	 */	r->h.C |= 0x04; r->x.T = 8; break;
	case 0xD2: /* SET 2,D	 */	r->h.D |= 0x04; r->x.T = 8; break;
	case 0xD3: /* SET 2,E	 */	r->h.E |= 0x04; r->x.T = 8; break;
	case 0xD4: /* SET 2,H	 */	r->h.H |= 0x04; r->x.T = 8; break;
	case 0xD5: /* SET 2,L	 */	r->h.L |= 0x04; r->x.T = 8; break;
	case 0xD6: /* SET 2,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x04)); r->x.T = 15; break;
	case 0xD7: /* SET 2,A	 */	r->h.A |= 0x04; r->x.T = 8; break;

	case 0xD8: /* SET 3,B	 */	r->h.B |= 0x08; r->x.T = 8; break;
	case 0xD9: /* SET 3,C	 */	r->h.C |= 0x08; r->x.T = 8; break;
	case 0xDA: /* SET 3,D	 */	r->h.D |= 0x08; r->x.T = 8; break;
	case 0xDB: /* SET 3,E	 */	r->h.E |= 0x08; r->x.T = 8; break;
	case 0xDC: /* SET 3,H	 */	r->h.H |= 0x08; r->x.T = 8; break;
	case 0xDD: /* SET 3,L	 */	r->h.L |= 0x08; r->x.T = 8; break;
	case 0xDE: /* SET 3,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x08)); r->x.T = 15; break;
	case 0xDF: /* SET 3,A	 */	r->h.A |= 0x08; r->x.T = 8; break;

	case 0xE0: /* SET 4,B	 */	r->h.B |= 0x10; r->x.T = 8; break;
	case 0xE1: /* SET 4,C	 */	r->h.C |= 0x10; r->x.T = 8; break;
	case 0xE2: /* SET 4,D	 */	r->h.D |= 0x10; r->x.T = 8; break;
	case 0xE3: /* SET 4,E	 */	r->h.E |= 0x10; r->x.T = 8; break;
	case 0xE4: /* SET 4,H	 */	r->h.H |= 0x10; r->x.T = 8; break;
	case 0xE5: /* SET 4,L	 */	r->h.L |= 0x10; r->x.T = 8; break;
	case 0xE6: /* SET 4,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x10)); r->x.T = 15; break;
	case 0xE7: /* SET 4,A	 */	r->h.A |= 0x10; r->x.T = 8; break;

	case 0xE8: /* SET 5,B	 */	r->h.B |= 0x20; r->x.T = 8; break;
	case 0xE9: /* SET 5,C	 */	r->h.C |= 0x20; r->x.T = 8; break;
	case 0xEA: /* SET 5,D	 */	r->h.D |= 0x20; r->x.T = 8; break;
	case 0xEB: /* SET 5,E	 */	r->h.E |= 0x20; r->x.T = 8; break;
	case 0xEC: /* SET 5,H	 */	r->h.H |= 0x20; r->x.T = 8; break;
	case 0xED: /* SET 5,L	 */	r->h.L |= 0x20; r->x.T = 8; break;
	case 0xEE: /* SET 5,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x20)); r->x.T = 15; break;
	case 0xEF: /* SET 5,A	 */	r->h.A |= 0x20; r->x.T = 8; break;

	case 0xF0: /* SET 6,B	 */	r->h.B |= 0x40; r->x.T = 8; break;
	case 0xF1: /* SET 6,C	 */	r->h.C |= 0x40; r->x.T = 8; break;
	case 0xF2: /* SET 6,D	 */	r->h.D |= 0x40; r->x.T = 8; break;
	case 0xF3: /* SET 6,E	 */	r->h.E |= 0x40; r->x.T = 8; break;
	case 0xF4: /* SET 6,H	 */	r->h.H |= 0x40; r->x.T = 8; break;
	case 0xF5: /* SET 6,L	 */	r->h.L |= 0x40; r->x.T = 8; break;
	case 0xF6: /* SET 6,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x40)); r->x.T = 15; break;
	case 0xF7: /* SET 6,A	 */	r->h.A |= 0x40; r->x.T = 8; break;

	case 0xF8: /* SET 7,B	 */	r->h.B |= 0x80; r->x.T = 8; break;
	case 0xF9: /* SET 7,C	 */	r->h.C |= 0x80; r->x.T = 8; break;
	case 0xFA: /* SET 7,D	 */	r->h.D |= 0x80; r->x.T = 8; break;
	case 0xFB: /* SET 7,E	 */	r->h.E |= 0x80; r->x.T = 8; break;
	case 0xFC: /* SET 7,H	 */	r->h.H |= 0x80; r->x.T = 8; break;
	case 0xFD: /* SET 7,L	 */	r->h.L |= 0x80; r->x.T = 8; break;
	case 0xFE: /* SET 7,(HL) */	__z80_mbwrite(cpu, r->x.HL, (__z80_mbread(cpu, r->x.HL) | 0x80)); r->x.T = 15; break;
	case 0xFF: /* SET 7,A	 */	r->h.A |= 0x80; r->x.T = 8; break;
	}
}

#ifdef __cplusplus
}	// extern "C"
#endif
