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
/**--------------------------------------------------------------------------]]
 **		Decoder
 **------------------------------------------------------------------------*/
export void _z80_decoder_ed(Z80CPU *cpu) {
	LPZ80REGS r = &cpu->r;
	switch (__z80_fetchb(cpu)) {
/*	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:

	case 0x08:					
	case 0x09:
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
	case 0x17:

	case 0x18:
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x1F:

	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:
	case 0x25:
	case 0x26:
	case 0x27:

	case 0x28:
	case 0x29:
	case 0x2A:
	case 0x2B:
	case 0x2C:
	case 0x2D:
	case 0x2E:
	case 0x2F:

	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37:

	case 0x38:
	case 0x39:
	case 0x3A:
	case 0x3B:
	case 0x3C:
	case 0x3D:
	case 0x3E:
	case 0x3F:	*/

	case 0x40: /* IN B,(C)	 */	__z80_microcode_in(cpu, &r->h.B, r->h.B, r->h.C); r->x.T = 12; break;
	case 0x41: /* OUT (C),B	 */ cpu->iowrite(MAKER16(r->h.B, r->h.C), r->h.B); r->x.T = 12; break;
	case 0x42: /* SBC HL,BC	 */	__z80_microcode_sbc16(&r->h.H, &r->h.L, r->h.B, r->h.C, &r->h.F); r->x.T = 15; break;
	case 0x43: /* LD (nn),BC */	__z80_mwwrite(cpu, __z80_fetchw(cpu), r->x.BC); r->x.T = 20; break;
	case 0x44: /* NEG		 */ {BYTE v = 0; __z80_microcode_sub8(&v, r->h.A, &r->h.F); r->h.A = v;} r->x.T = 8; break;
	case 0x45: /* RETN		 */ r->x.PC = __z80_pop(cpu); r->x.IFF1 = r->x.IFF2; r->x.T = 14; break;
	case 0x46: /* IM 0		 */ r->x.IM = 0; r->x.T = 8; break;
	case 0x47: /* LD I,A	 */ r->x.I = r->h.A; r->x.T = 9; break;

	case 0x48: /* IN C,(C)	 */	__z80_microcode_in(cpu, &r->h.C, r->h.B, r->h.C); r->x.T = 12; break;
	case 0x49: /* OUT (C),C	 */ cpu->iowrite(MAKER16(r->h.B, r->h.C), r->h.C); r->x.T = 12; break;
	case 0x4A: /* ADC HL,BC	 */	__z80_microcode_adc16(&r->h.H, &r->h.L, r->h.B, r->h.C, &r->h.F); r->x.T = 15; break;
	case 0x4B: /* LD BC,(nn) */	r->x.BC = __z80_mwread(cpu, __z80_fetchw(cpu));r->x.T = 20; break;
/*	case 0x4C:	*/
	case 0x4D: /* RETI		 */ r->x.PC = __z80_pop(cpu); r->x.T = 14; break;
/*	case 0x4E:	*/
	case 0x4F: /* LD R,A	 */ r->x.R = r->h.A; r->x.T = 9; break;

	case 0x50: /* IN D,(C)	 */	__z80_microcode_in(cpu, &r->h.D, r->h.B, r->h.C); r->x.T = 12; break;
	case 0x51: /* OUT (C),D	 */ cpu->iowrite(MAKER16(r->h.B, r->h.C), r->h.D); r->x.T = 12; break;
	case 0x52: /* SBC HL,DE	 */	__z80_microcode_sbc16(&r->h.H, &r->h.L, r->h.D, r->h.E, &r->h.F); r->x.T = 15; break;
	case 0x53: /* LD (nn),DE */	__z80_mwwrite(cpu, __z80_fetchw(cpu), r->x.DE); r->x.T = 20; break;
/*	case 0x54:
	case 0x55:	*/
	case 0x56: /* IM 1		 */ r->x.IM = 1; r->x.T = 8; break;
	case 0x57: /* LD A,I	 */ __z80_microcode_ldair(cpu, r->x.I); r->x.T = 9; break;

	case 0x58: /* IN E,(C)	 */	__z80_microcode_in(cpu, &r->h.E, r->h.B, r->h.C); r->x.T = 12; break;
	case 0x59: /* OUT (C),B	 */ cpu->iowrite(MAKER16(r->h.B, r->h.C), r->h.E); r->x.T = 12; break;
	case 0x5A: /* ADC HL,DE	 */	__z80_microcode_adc16(&r->h.H, &r->h.L, r->h.D, r->h.E, &r->h.F); r->x.T = 15; break;
	case 0x5B: /* LD DE,(nn) */	r->x.DE = __z80_mwread(cpu, __z80_fetchw(cpu)); r->x.T = 20; break;
/*	case 0x5C:
	case 0x5D:	*/
	case 0x5E: /* IM 2		 */ r->x.IM = 2; r->x.T = 8; break;
	case 0x5F: /* LD A,R	 */ __z80_microcode_ldair(cpu, cpu->rreg[cpu->clk]); r->x.T = 9; break;

	case 0x60: /* IN H,(C)	 */	__z80_microcode_in(cpu, &r->h.H, r->h.B, r->h.C); r->x.T = 12; break;
	case 0x61: /* OUT (C),B	 */ cpu->iowrite(MAKER16(r->h.B, r->h.C), r->h.H); r->x.T = 12; break;
	case 0x62: /* SBC HL,HL	 */	__z80_microcode_sbc16(&r->h.H, &r->h.L, r->h.H, r->h.L, &r->h.F); r->x.T = 15; break;
	case 0x63: /* LD (nn),HL */	__z80_mwwrite(cpu, __z80_fetchw(cpu), r->x.HL); r->x.T = 20; break;
/*	case 0x64:
	case 0x65:
	case 0x66:	*/
	case 0x67: /* RRD		 */	__z80_microcode_rrd(cpu); r->x.T = 18; break;

	case 0x68: /* IN L,(C)	 */	__z80_microcode_in(cpu, &r->h.L, r->h.B, r->h.C); r->x.T = 12; break;
	case 0x69: /* OUT (C),L	 */ cpu->iowrite(MAKER16(r->h.B, r->h.C), r->h.L); r->x.T = 12; break;
	case 0x6A: /* ADC HL,HL	 */	__z80_microcode_adc16(&r->h.H, &r->h.L, r->h.H, r->h.L, &r->h.F); r->x.T = 15; break;
	case 0x6B: /* LD HL,(nn) */	r->x.HL = __z80_mwread(cpu, __z80_fetchw(cpu)); r->x.T = 20; break;
/*	case 0x6C:
	case 0x6D:
	case 0x6E:	*/
	case 0x6F: /* RLD		 */	__z80_microcode_rld(cpu); r->x.T = 18; break;

/*	case 0x70:	  IN F,(C)	 */
/*	case 0x71:	  OUT (C),0	 */
	case 0x72: /* SBC HL,SP	 */	__z80_microcode_sbc16(&r->h.H, &r->h.L, r->h.SPH, r->h.SPL, &r->h.F); r->x.T = 15; break;
	case 0x73: /* LD (nn),SP */	__z80_mwwrite(cpu, __z80_fetchw(cpu), r->x.SP); r->x.T = 20; break;
/*	case 0x74:
	case 0x75:
	case 0x76:
	case 0x77:	*/

	case 0x78: /* IN A,(C)	 */	__z80_microcode_in(cpu, &r->h.A, r->h.B, r->h.C); r->x.T = 12; break;
	case 0x79: /* OUT (C),A	 */ cpu->iowrite(MAKER16(r->h.B, r->h.C), r->h.A); r->x.T = 12; break;
	case 0x7A: /* ADC HL,SP	 */	__z80_microcode_adc16(&r->h.H, &r->h.L, r->h.SPH, r->h.SPL, &r->h.F); r->x.T = 15; break;
	case 0x7B: /* LD SP,(nn) */	r->x.SP = __z80_mwread(cpu, __z80_fetchw(cpu)); r->x.T = 20; break;
/*	case 0x7C:
	case 0x7D:
	case 0x7E:
	case 0x7F:

	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:

	case 0x88:
	case 0x89:
	case 0x8A:
	case 0x8B:
	case 0x8C:
	case 0x8D:
	case 0x8E:
	case 0x8F:

	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:

	case 0x98:
	case 0x99:
	case 0x9A:
	case 0x9B:
	case 0x9C:
	case 0x9D:
	case 0x9E:
	case 0x9F:	*/

	case 0xA0: /* LDI		 */	__z80_microcode_ldi(cpu); r->x.T = 16; break;
	case 0xA1: /* CPI		 */	__z80_microcode_cpi(cpu); r->x.T = 16; break;
	case 0xA2: /* INI		 */	__z80_unimplemented_instruction(cpu); break;
	case 0xA3: /* OUTI		 */	__z80_unimplemented_instruction(cpu); break;
/*	case 0xA4:
	case 0xA5:
	case 0xA6:
	case 0xA7:	*/

	case 0xA8: /* LDD		 */	__z80_microcode_ldd(cpu); r->x.T = 16; break;
	case 0xA9: /* CPD		 */	__z80_microcode_cpd(cpu); r->x.T = 16; break;
	case 0xAA: /* IND		 */	__z80_unimplemented_instruction(cpu); break;
	case 0xAB: /* OUTD		 */	__z80_unimplemented_instruction(cpu); break;
/*	case 0xAC:
	case 0xAD:
	case 0xAE:
	case 0xAF:	*/

	case 0xB0: /* LDIR		 */	__z80_microcode_ldi(cpu); if (r->x.BC == 0) r->x.T = 16; else { r->x.T = 21; r->x.PC -= 2; } break;
	case 0xB1: /* CPIR		 */	__z80_microcode_cpi(cpu); if (r->x.BC == 0 || (r->h.F & _ZF)) r->x.T = 18; else { r->x.T = 21; r->x.PC -= 2; } break;
	case 0xB2: /* INIR		 */	__z80_unimplemented_instruction(cpu); break;
	case 0xB3: /* OUTIR		 */	__z80_unimplemented_instruction(cpu); break;
/*	case 0xB4:
	case 0xB5:
	case 0xB6:
	case 0xB7:	*/

	case 0xB8: /* LDDR		 */	__z80_microcode_ldd(cpu); if (r->x.BC == 0) r->x.T = 16; else { r->x.T = 21; r->x.PC -= 2; } break;
	case 0xB9: /* CPDR		 */	__z80_microcode_cpd(cpu); if (r->x.BC == 0 || (r->h.F & _ZF)) r->x.T = 18; else { r->x.T = 21; r->x.PC -= 2; } break;
	case 0xBA: /* INDR		 */	__z80_unimplemented_instruction(cpu); break;
	case 0xBB: /* OUTDR		 */	__z80_unimplemented_instruction(cpu); break;
/*	case 0xBC:
	case 0xBD:
	case 0xBE:
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
	case 0xCA:
	case 0xCB:					
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

	case 0xE0:
	case 0xE1:
	case 0xE2:
	case 0xE3:
	case 0xE4:
	case 0xE5:
	case 0xE6:
	case 0xE7:

	case 0xE8:
	case 0xE9:
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

	case 0xF8:
	case 0xF9:
	case 0xFA:
	case 0xFB:
	case 0xFC:
	case 0xFD:
	case 0xFE:
	case 0xFF:	*/
	default:					__z80_illegal_instruction(cpu); r->x.T = 5; break;
	}
}

#ifdef __cplusplus
}	// extern "C"
#endif
