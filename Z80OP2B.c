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
 **		ASM Modules
 **------------------------------------------------------------------------*/
extern void _z80_decoder_xd_cb(Z80CPU *cpu, int IXY);
/**--------------------------------------------------------------------------
 **		Decoder
 **------------------------------------------------------------------------*/
export void _z80_decoder_xd(Z80CPU *cpu, int IXY, int IXYH, int IXYL) {
	LPZ80REGS r = &cpu->r;
	switch (__z80_fetchb(cpu)) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:

	case 0x08:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x09: /* ADD IX,BC	 */	__z80_microcode_add16(&r->br[IXYH], &r->br[IXYL], r->h.B, r->h.C, &r->h.F); r->x.T = 15; break;
	case 0x0A:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0E:
	case 0x0F:

	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:					__z80_illegal_instruction(cpu); r->x.T = 5; break;

	case 0x18:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x19:	/* ADD IX,DE */	__z80_microcode_add16(&r->br[IXYH], &r->br[IXYL], r->h.D, r->h.E, &r->h.F); r->x.T = 15; break;
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x1F:					__z80_illegal_instruction(cpu); r->x.T = 5; break;

	case 0x20:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x21: /* LD,IX,nn	 */	r->xr[IXY] = __z80_fetchw(cpu); r->x.T = 14; break;
	case 0x22: /* LD (nn).IX */	__z80_mwwrite(cpu, __z80_fetchw(cpu), r->xr[IXY]); r->x.T = 20; break;
	case 0x23: /* INC IX	 */	++r->xr[IXY]; r->x.T = 10; break;
	case 0x24: /* INC IXH	 */	__z80_microcode_inc8(&r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0x25: /* DEC IXH	 */	__z80_microcode_dec8(&r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0x26: /* LD IXH,n	 */	r->br[IXYH] = __z80_fetchb(cpu); r->x.T = 11; break;
	case 0x27:					__z80_illegal_instruction(cpu); r->x.T = 5; break;

	case 0x28:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x29: /* ADD IX,HL	 */	__z80_microcode_add16(&r->br[IXYH], &r->br[IXYL], r->h.H, r->h.L, &r->h.F); r->x.T = 15; break;
	case 0x2A: /* LD IX,(nn) */	r->xr[IXY] = __z80_mwread(cpu, __z80_fetchw(cpu)); r->x.T = 20; break;
	case 0x2B: /* DEC IX	 */	--r->xr[IXY]; r->x.T = 10; break;
	case 0x2C: /* INC IXL	 */	__z80_microcode_inc8(&r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0x2D: /* DEC IXL	 */	__z80_microcode_dec8(&r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0x2E: /* LD IXL,n	 */	r->br[IXYL] = __z80_fetchb(cpu); r->x.T = 11; break;
	case 0x2F:					__z80_illegal_instruction(cpu); r->x.T = 5; break;

	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x34: /* INC (IX+d) */	{USHORT idx = __z80_index(r->xr[IXY], __z80_fetchb(cpu)); BYTE b = __z80_mbread(cpu, idx); __z80_microcode_inc8(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 26;} break;
	case 0x35: /* DEC (IX+d) */	{USHORT idx = __z80_index(r->xr[IXY], __z80_fetchb(cpu)); BYTE b = __z80_mbread(cpu, idx); __z80_microcode_dec8(&b, &r->h.F); __z80_mbwrite(cpu, idx, b); r->x.T = 26;} break;
	case 0x36: /* LD (IX+d),n*/	{USHORT idx = __z80_index(r->xr[IXY], __z80_fetchb(cpu)); __z80_mbwrite(cpu, idx, __z80_fetchb(cpu));} r->x.T = 19; break;
	case 0x37:

	case 0x38:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x39: /* ADD IX,SP	 */	__z80_microcode_add16(&r->br[IXYH], &r->br[IXYL], r->h.SPH, r->h.SPL, &r->h.F); r->x.T = 15; break;
	case 0x3A:
	case 0x3B:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	case 0x3F:					__z80_illegal_instruction(cpu); r->x.T = 5; break;

	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x44: /* LD B,IXH	 */	r->h.B = r->br[IXYH]; r->x.T = 8; break;
	case 0x45: /* LD B,IXL	 */	r->h.B = r->br[IXYL]; r->x.T = 8; break;
	case 0x46: /* LD B,(IX+d)*/ r->h.B = __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))); r->x.T = 19; break;
	case 0x47:

	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x4B:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x4C: /* LD C,IXH	 */	r->h.C = r->br[IXYH]; r->x.T = 8; break;
	case 0x4D: /* LD C,IXL	 */	r->h.C = r->br[IXYL]; r->x.T = 8; break;
	case 0x4E: /* LD C,(IX+d)*/ r->h.C = __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))); r->x.T = 19; break;
	case 0x4F:

	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x54: /* LD D,IXH	 */	r->h.D = r->br[IXYH]; r->x.T = 8; break;
	case 0x55: /* LD D,IXL	 */	r->h.D = r->br[IXYL]; r->x.T = 8; break;
	case 0x56: /* LD D,(IX+d)*/ r->h.D = __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))); r->x.T = 19; break;

	case 0x57:
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x5C: /* LD E,IXH	 */	r->h.E = r->br[IXYH]; r->x.T = 8; break;
	case 0x5D: /* LD E,IXL	 */	r->h.E = r->br[IXYL]; r->x.T = 8; break;
	case 0x5E: /* LD E,(IX+d)*/ r->h.E = __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))); r->x.T = 19; break;
	case 0x5F:					__z80_illegal_instruction(cpu); r->x.T = 5; break;

	case 0x60: /* LD IXH,B	 */	r->br[IXYH] = r->h.B; r->x.T = 8; break;
	case 0x61: /* LD IXH,C	 */	r->br[IXYH] = r->h.C; r->x.T = 8; break;
	case 0x62: /* LD IXH,D	 */	r->br[IXYH] = r->h.D; r->x.T = 8; break;
	case 0x63: /* LD IXH,E	 */	r->br[IXYH] = r->h.E; r->x.T = 8; break;
	case 0x64: /* LD IXH,H	 */	r->br[IXYH] = r->h.H; r->x.T = 8; break;
	case 0x65: /* LD IXH,L	 */	r->br[IXYH] = r->h.L; r->x.T = 8; break;
	case 0x66: /* LD H,(IX+d)*/ r->h.H = __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))); r->x.T = 19; break;
	case 0x67: /* LD IXH,A	 */	r->br[IXYH] = r->h.A; r->x.T = 8; break;

	case 0x68: /* LD IXL,B	 */	r->br[IXYL] = r->h.B; r->x.T = 8; break;
	case 0x69: /* LD IXL,C	 */	r->br[IXYL] = r->h.C; r->x.T = 8; break;
	case 0x6A: /* LD IXL,D	 */	r->br[IXYL] = r->h.D; r->x.T = 8; break;
	case 0x6B: /* LD IXL,E	 */	r->br[IXYL] = r->h.E; r->x.T = 8; break;
	case 0x6C: /* LD IXL,H	 */	r->br[IXYL] = r->h.H; r->x.T = 8; break;
	case 0x6D: /* LD IXL,L	 */	r->br[IXYL] = r->h.L; r->x.T = 8; break;
	case 0x6E: /* LD L,(IX+d)*/ r->h.L = __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))); r->x.T = 19; break;
	case 0x6F: /* LD IXL,A	 */	r->br[IXYL] = r->h.A; r->x.T = 8; break;

	case 0x70: /* LD (IX+d),B*/ __z80_mbwrite(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu)), r->h.B); r->x.T = 19; break;
	case 0x71: /* LD (IX+d),C*/ __z80_mbwrite(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu)), r->h.C); r->x.T = 19; break;
	case 0x72: /* LD (IX+d),D*/ __z80_mbwrite(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu)), r->h.D); r->x.T = 19; break;
	case 0x73: /* LD (IX+d),E*/ __z80_mbwrite(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu)), r->h.E); r->x.T = 19; break;
	case 0x74: /* LD (IX+d),H*/ __z80_mbwrite(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu)), r->h.H); r->x.T = 19; break;
	case 0x75: /* LD (IX+d),L*/ __z80_mbwrite(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu)), r->h.L); r->x.T = 19; break;
	case 0x76:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x77: /* LD (IX+d),A*/ __z80_mbwrite(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu)), r->h.A); r->x.T = 19; break;

	case 0x78:
	case 0x79:
	case 0x7A:
	case 0x7B:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x7C: /* LD A,IXH	 */	r->h.A = r->br[IXYH]; r->x.T = 8; break;
	case 0x7D: /* LD A,IXL	 */	r->h.A = r->br[IXYL]; r->x.T = 8; break;
	case 0x7E: /* LD A,(IX+d)*/ r->h.A = __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))); r->x.T = 19; break;
	case 0x7F:

	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x84: /* ADD A,IXH	 */	__z80_microcode_add8(&r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0x85: /* ADD A,IXL	 */	__z80_microcode_add8(&r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0x86: /* ADD A,(IX+d)*/__z80_microcode_add8(&r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0x87:

	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x8C: /* ADC A,IXH	 */	__z80_microcode_adc8(&r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0x8D: /* ADC A,IXL	 */	__z80_microcode_adc8(&r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0x8E: /* ADC A,(IX+d)*/__z80_microcode_adc8(&r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0x8F:

	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x94: /* SUB A,IXH	 */	__z80_microcode_sub8(&r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0x95: /* SUB A,IXL	 */	__z80_microcode_sub8(&r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0x96: /* SUB A,(IX+d)*/__z80_microcode_sub8(&r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0x97:

	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0x9C: /* SBC A,IXH	 */	__z80_microcode_sbc8(&r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0x9D: /* SBC A,IXL	 */	__z80_microcode_sbc8(&r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0x9E: /* SBC A,(IX+d)*/__z80_microcode_sbc8(&r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0x9F:

	case 0xA0:
	case 0xA1:
	case 0xA2:
	case 0xA3:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xA4: /* AND A,IXH	 */	__z80_microcode_and8(&r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0xA5: /* AND A,IXL	 */	__z80_microcode_and8(&r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0xA6: /* AND A,(IX+d)*/__z80_microcode_and8(&r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0xA7:

	case 0xA8:
	case 0xA9:
	case 0xAA:
	case 0xAB:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xAC: /* XOR A,IXH	 */	__z80_microcode_xor8(&r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0xAD: /* XOR A,IXL	 */	__z80_microcode_xor8(&r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0xAE: /* XOR A,(IX+d)*/__z80_microcode_xor8(&r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0xAF:

	case 0xB0:
	case 0xB1:
	case 0xB2:
	case 0xB3:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xB4: /* OR A,IXH	 */	__z80_microcode_or8(&r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0xB5: /* OR A,IXL	 */	__z80_microcode_or8(&r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0xB6: /* OR A,(IX+d)*/	__z80_microcode_or8(&r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0xB7:

	case 0xB8:
	case 0xB9:
	case 0xBA:
	case 0xBB:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xBC: /* CP IXH	 */	__z80_microcode_cp8(r->h.A, r->br[IXYH], &r->h.F); r->x.T = 8; break;
	case 0xBD: /* CP IXL	 */	__z80_microcode_cp8(r->h.A, r->br[IXYL], &r->h.F); r->x.T = 8; break;
	case 0xBE: /* CP (IX+d)	 */	__z80_microcode_cp8(r->h.A, __z80_mbread(cpu, __z80_index(r->xr[IXY], __z80_fetchb(cpu))), &r->h.F); r->x.T = 19; break;
	case 0xBF:

	case 0xC0:
	case 0xC1:
	case 0xC2:
	case 0xC3:
	case 0xC4:
	case 0xC5:
	case 0xC6:
	case 0xC7:

	case 0xC8:
	case 0xC9:
	case 0xCA:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xCB:					_z80_decoder_xd_cb(cpu, IXY); break;
	case 0xCC:
	case 0xCD:
	case 0xCE:
	case 0xCF:

	case 0xD0:
	case 0xD1:
	case 0xD2:
	case 0xD3:
	case 0xD4:
	case 0xD5:
	case 0xD6:
	case 0xD7:

	case 0xD8:
	case 0xD9:
	case 0xDA:
	case 0xDB:
	case 0xDC:
	case 0xDD:
	case 0xDE:
	case 0xDF:

	case 0xE0:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xE1: /* POP IX	 */	r->xr[IXY] = __z80_pop(cpu); r->x.T = 14; break;
	case 0xE2:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xE3: /* EX (SP),IX */	{USHORT x = __z80_mwread(cpu, r->x.SP); SWAP16(&x, &r->xr[IXY]); __z80_mwwrite(cpu, r->x.SP, x); }
	case 0xE4:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xE5: /* PUSH IX	 */	__z80_pushw(cpu, r->xr[IXY]); r->x.T = 15; break;
	case 0xE6:
	case 0xE7:

	case 0xE8:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xE9: /* JP (IX)	 */	r->x.PC = r->xr[IXY]; r->x.T = 8; break;
	case 0xEA:
	case 0xEB:
	case 0xEC:
	case 0xED:
	case 0xEE:
	case 0xEF:

	case 0xF0:
	case 0xF1:
	case 0xF2:
	case 0xF3:
	case 0xF4:
	case 0xF5:
	case 0xF6:
	case 0xF7:

	case 0xF8:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	case 0xF9: /* LD SP,IX	 */	r->x.SP = r->xr[IXY]; r->x.T = 10; break;
	case 0xFA:
	case 0xFB:
	case 0xFC:
	case 0xFD:
	case 0xFE:
	case 0xFF:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	}
}

#ifdef __cplusplus
}	// extern "C"
#endif
