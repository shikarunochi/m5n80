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
 **		external functions
 **------------------------------------------------------------------------*/
extern void		_z80_decoder_cb(Z80CPU *cpu);
extern void		_z80_decoder_ed(Z80CPU *cpu);
extern void		_z80_decoder_xd(Z80CPU *cpu, int IXY, int IXYH, int IXYL);
/**--------------------------------------------------------------------------
 **		Decoder
 **------------------------------------------------------------------------*/
export void _z80_decoder(Z80CPU *cpu) {
	LPZ80REGS r = &cpu->r;

	switch (__z80_fetchb(cpu)) {
	case 0x00: /* NOP		 */	r->x.T = 4; break;
	case 0x01: /* LD BC,nn	 */	r->x.BC = __z80_fetchw(cpu); r->x.T = 11; break;
	case 0x02: /* LD (BC),A	 */	__z80_mbwrite(cpu, r->x.BC, r->h.A); r->x.T = 8; break;
	case 0x03: /* INC BC	 */	++r->x.BC; r->x.T = 7; break;
	case 0x04: /* INC B		 */	__z80_microcode_inc8(&r->h.B, &r->h.F);	r->x.T = 5; break;
	case 0x05: /* DEC B		 */	__z80_microcode_dec8(&r->h.B, &r->h.F);	r->x.T = 5; break;
	case 0x06: /* LD B,n	 */	r->h.B = __z80_fetchb(cpu); r->x.T = 8; break;
	case 0x07: /* RLCA		 */	{	int v = ((int) r->h.A) << 1;
									int c = (v >> 8) & 0x01;
									int f = r->h.F & (_SF | _ZF | _PF);
									v |= c;
									f |= c;
									f |= v & (_YF | _XF);
									r->h.F = f;
									r->h.A = v;
									r->x.T = 5;
								};	break;

	case 0x08: /* EX AF,AF'	 */	SWAP16(&r->x.AF, &r->x.altAF); r->x.T = 4; break;
	case 0x09: /* ADD HL,BC	 */	__z80_microcode_add16(&r->h.H, &r->h.L, r->h.B, r->h.C, &r->h.F); r->x.T = 12; break;
	case 0x0A: /* LD A,(BC)	 */	r->h.A = __z80_mbread(cpu, r->x.BC); r->x.T = 8; break;
	case 0x0B: /* DEC BC	 */ --r->x.BC; r->x.T = 7; break;
	case 0x0C: /* INC C		 */	__z80_microcode_inc8(&r->h.C, &r->h.F);	r->x.T = 5; break;
	case 0x0D: /* DEC C		 */	__z80_microcode_dec8(&r->h.C, &r->h.F);	r->x.T = 5; break;
	case 0x0E: /* LD C,n	 */	r->h.C = __z80_fetchb(cpu); r->x.T = 8; break;
	case 0x0F: /* RRCA		 */	{	int c = r->h.A & 0x01;
									int v = ((int) r->h.A) >> 1;
									int f = r->h.F & (_SF | _ZF | _PF);
									v |= (c << 7);
									f |= c;
									f |= v & (_YF | _XF);
									r->h.F = f;
									r->h.A = v;
									r->x.T = 5;
								}; break;

	case 0x10: /* DJNZ e	 */	{	USHORT offset = (USHORT) ((signed char) __z80_fetchb(cpu));
									if (--r->h.B != 0) {
										r->x.PC += offset;
										r->x.T = 13;
									} else
										r->x.T = 10;
								}; break;
	case 0x11: /* LD DE,nn	 */	r->x.DE = __z80_fetchw(cpu); r->x.T = 11; break;
	case 0x12: /* LD (DE),A	 */	__z80_mbwrite(cpu, r->x.DE, r->h.A); r->x.T = 8; break;
	case 0x13: /* INC DE	 */	++r->x.DE; r->x.T = 7; break;
	case 0x14: /* INC D		 */	__z80_microcode_inc8(&r->h.D, &r->h.F);	r->x.T = 5; break;
	case 0x15: /* DEC D		 */	__z80_microcode_dec8(&r->h.D, &r->h.F);	r->x.T = 5; break;
	case 0x16: /* LD D,n	 */	r->h.D = __z80_fetchb(cpu); r->x.T = 8; break;
	case 0x17: /* RLA		 */	{	int v = (((int) r->h.A) << 1) | (r->h.F & _CF);
									int c = (v >> 8) & 0x01;
									int f = r->h.F & (_SF | _ZF | _PF);
									f |= c;
									f |= v & (_YF | _XF);
									r->h.F = f;
									r->h.A = v;
									r->x.T = 5;
								}; break;

	case 0x18: /* JR e		 */	{USHORT offset = (USHORT) ((signed char) __z80_fetchb(cpu)); r->x.PC += offset; r->x.T = 12;}; break;
	case 0x19: /* ADD HL,DE	 */	__z80_microcode_add16(&r->h.H, &r->h.L, r->h.D, r->h.E, &r->h.F); r->x.T = 12; break;
	case 0x1A: /* LD A,(DE)	 */	r->h.A = __z80_mbread(cpu, r->x.DE); r->x.T = 8; break;
	case 0x1B: /* DEC DE	 */ --r->x.DE; r->x.T = 7; break;
	case 0x1C: /* INC E		 */	__z80_microcode_inc8(&r->h.E, &r->h.F);	r->x.T = 5; break;
	case 0x1D: /* DEC E		 */	__z80_microcode_dec8(&r->h.E, &r->h.F);	r->x.T = 5; break;
	case 0x1E: /* LD E,n	 */	r->h.E = __z80_fetchb(cpu); r->x.T = 8; break;
	case 0x1F: /* RRA		 */	{		int c = r->h.A & 0x01;
										int v = (((int) r->h.A) >> 1) | ((r->h.F & _CF) << 7);
										int f = r->h.F & (_SF | _ZF | _PF);
										f |= c;
										f |= v & (_YF | _XF);
										r->h.F = f;
										r->h.A = v;
										r->x.T = 5;
								}; break;

	case 0x20: /* JR NZ,e	 */	{	USHORT offset = (USHORT) ((signed char) __z80_fetchb(cpu));
									if (!(r->h.F & _ZF)) {
										r->x.PC += offset;
										r->x.T = 12;
									} else
										r->x.T = 7;
								}; break;
	case 0x21: /* LD HL,nn	 */	r->x.HL = __z80_fetchw(cpu); r->x.T = 11; break;
	case 0x22: /* LD (nn),HL */	__z80_mwwrite(cpu, __z80_fetchw(cpu), r->x.HL); r->x.T = 17; break;
	case 0x23: /* INC HL	 */	++r->x.HL; r->x.T = 7; break;
	case 0x24: /* INC H		 */	__z80_microcode_inc8(&r->h.H, &r->h.F);	r->x.T = 5; break;
	case 0x25: /* DEC H		 */	__z80_microcode_dec8(&r->h.H, &r->h.F);	r->x.T = 5; break;
	case 0x26: /* LD H,n	 */	r->h.H = __z80_fetchb(cpu); r->x.T = 8; break;
	case 0x27: /* DAA		 */	{	BYTE hn = (r->h.A & 0xf0) >> 4;
									BYTE ln = (r->h.A & 0x0f);
									BYTE f = (r->h.F & (_HF | _NF | _CF));
									if (!(r->h.F & _NF)) {
										if (ln > 9 || (r->h.F & _HF)) {
											ln += 6;
											f |= _HF;
										}
										if (hn > 9 || (r->h.F & _CF)) {
											hn += 6;
											f |= _CF;
										} else {
											f &= ~_CF;
										}
									} else {
										if (ln > 9 || (r->h.F & _HF)) {
											ln -= 6;
											f |= _HF;
										}
										if (hn > 9 || (r->h.F & _CF)) {
											hn -= 6;
											f |= _CF;
										} else {
											f &= ~_CF;
										}
									}
									r->h.A = ((hn << 4) | ln);
									f |= __z80_microcode_sf8(r->h.A);
									f |= __z80_microcode_zf8(r->h.A);
									f |= __z80_microcode_yxf(r->h.A);
									f |= __z80_microcode_pf8(r->h.A);
									r->h.F = f;
									r->x.T = 5;
								}; break;

	case 0x28: /* JR Z,e	 */	{	USHORT offset = (USHORT) ((signed char) __z80_fetchb(cpu));
									if (r->h.F & _ZF) {
										r->x.PC += offset;
										r->x.T = 12;
									} else
										r->x.T = 7;
								}; break;
	case 0x29: /* ADD HL,HL	 */	__z80_microcode_add16(&r->h.H, &r->h.L, r->h.H, r->h.L, &r->h.F); r->x.T = 12; break;
	case 0x2A: /* LD HL,(nn) */	r->x.HL = __z80_mwread(cpu, __z80_fetchw(cpu));	r->x.T = 17; break;
	case 0x2B: /* DEC HL	 */ --r->x.HL; r->x.T = 7; break;
	case 0x2C: /* INC L		 */	__z80_microcode_inc8(&r->h.L, &r->h.F);	r->x.T = 5; break;
	case 0x2D: /* DEC L		 */	__z80_microcode_dec8(&r->h.L, &r->h.F);	r->x.T = 5; break;
	case 0x2E: /* LD L,n	 */	r->h.L = __z80_fetchb(cpu); r->x.T = 8; break;
	case 0x2F: /* CPL		 */	{	BYTE f = (r->h.F & (_SF | _ZF | _PF | _CF)) | _HF | _NF;
									r->h.A = ~r->h.A;
									f |= (r->h.A & (_YF | _XF));
									r->h.F = f;
									r->x.T = 5;
								}; break;

	case 0x30: /* JR NC,e	 */	{	USHORT offset = (USHORT) ((signed char) __z80_fetchb(cpu));
									if (!(r->h.F & _CF)) {
										r->x.PC += offset;
										r->x.T = 12;
									} else
										r->x.T = 7;
								}; break;
	case 0x31: /* LD SP,nn	 */	r->x.SP = __z80_fetchw(cpu); r->x.T = 11; break;
	case 0x32: /* LD (nn),A  */	__z80_mbwrite(cpu, __z80_fetchw(cpu), r->h.A); r->x.T = 14; break;
	case 0x33: /* INC SP	 */	++r->x.SP; r->x.T = 7; break;
	case 0x34: /* INC (HL)	 */	{	BYTE b = __z80_mbread(cpu, r->x.HL);
									__z80_microcode_inc8(&b, &r->h.F);
									__z80_mbwrite(cpu, r->x.HL, b);
									r->x.T = 12;
								}; break;
	case 0x35: /* DEC (HL)	 */	{	BYTE b = __z80_mbread(cpu, r->x.HL);
									__z80_microcode_dec8(&b, &r->h.F);
									__z80_mbwrite(cpu, r->x.HL, b);
									r->x.T = 12;
								}; break;
	case 0x36: /* LD (HL),n	 */	__z80_mbwrite(cpu, r->x.HL, __z80_fetchb(cpu)); r->x.T = 11; break;
	case 0x37: /* SCF		 */	{	BYTE f = (r->h.F & (_SF | _ZF | _PF)) | (r->h.A & (_YF | _XF)) | _CF;
									r->h.F = f;
									r->x.T = 5;
								}; break;

	case 0x38: /* JR C,e	 */	{	USHORT offset = (USHORT) ((signed char) __z80_fetchb(cpu));
									if (r->h.F & _CF) {
										r->x.PC += offset;
										r->x.T = 12;
									} else
										r->x.T = 7;
								}; break;
	case 0x39: /* ADD HL,SP	 */	__z80_microcode_add16(&r->h.H, &r->h.L, r->h.SPH, r->h.SPL, &r->h.F); r->x.T = 12; break;
	case 0x3A: /* LD A,(nn)	 */	r->h.A = __z80_mbread(cpu, __z80_fetchw(cpu)); r->x.T = 14; break;
	case 0x3B: /* DEC SP	 */ --r->x.SP; r->x.T = 7; break;
	case 0x3C: /* INC A		 */	__z80_microcode_inc8(&r->h.A, &r->h.F);	r->x.T = 5; break;
	case 0x3D: /* DEC A		 */	__z80_microcode_dec8(&r->h.A, &r->h.F);	r->x.T = 5; break;
	case 0x3E: /* LD A,n	 */	r->h.A = __z80_fetchb(cpu); r->x.T = 8; break;
	case 0x3F: /* CCF		 */ {	BYTE h = (r->h.F & _CF) << 4;
									BYTE f = (r->h.F & (_SF | _ZF | _PF | _CF)) | (r->h.A & (_YF | _XF)) | h;
									f ^= _CF;
									r->h.F = f;
									r->x.T = 5;
								}; break;

	case 0x40: /* LD B,B	 */	r->h.B = r->h.B; r->x.T = 4; break;
	case 0x41: /* LD B,C	 */	r->h.B = r->h.C; r->x.T = 4; break;
	case 0x42: /* LD B,D	 */	r->h.B = r->h.D; r->x.T = 4; break;
	case 0x43: /* LD B,E	 */	r->h.B = r->h.E; r->x.T = 4; break;
	case 0x44: /* LD B,H	 */	r->h.B = r->h.H; r->x.T = 4; break;
	case 0x45: /* LD B,L	 */	r->h.B = r->h.L; r->x.T = 4; break;
	case 0x46: /* LD B,(HL)	 */	r->h.B = __z80_mbread(cpu, r->x.HL); r->x.T = 7; break;
	case 0x47: /* LD B,A	 */	r->h.B = r->h.A; r->x.T = 4; break;

	case 0x48: /* LD C,B	 */	r->h.C = r->h.B; r->x.T = 4; break;
	case 0x49: /* LD C,C	 */	r->h.C = r->h.C; r->x.T = 4; break;
	case 0x4A: /* LD C,D	 */	r->h.C = r->h.D; r->x.T = 4; break;
	case 0x4B: /* LD C,E	 */	r->h.C = r->h.E; r->x.T = 4; break;
	case 0x4C: /* LD C,H	 */	r->h.C = r->h.H; r->x.T = 4; break;
	case 0x4D: /* LD C,L	 */	r->h.C = r->h.L; r->x.T = 4; break;
	case 0x4E: /* LD C,(HL)	 */	r->h.C = __z80_mbread(cpu, r->x.HL); r->x.T = 7; break;
	case 0x4F: /* LD C,A	 */	r->h.C = r->h.A; r->x.T = 4; break;

	case 0x50: /* LD D,B	 */	r->h.D = r->h.B; r->x.T = 4; break;
	case 0x51: /* LD D,C	 */	r->h.D = r->h.C; r->x.T = 4; break;
	case 0x52: /* LD D,D	 */	r->h.D = r->h.D; r->x.T = 4; break;
	case 0x53: /* LD D,E	 */	r->h.D = r->h.E; r->x.T = 4; break;
	case 0x54: /* LD D,H	 */	r->h.D = r->h.H; r->x.T = 4; break;
	case 0x55: /* LD D,L	 */	r->h.D = r->h.L; r->x.T = 4; break;
	case 0x56: /* LD D,(HL)	 */	r->h.D = __z80_mbread(cpu, r->x.HL); r->x.T = 7; break;
	case 0x57: /* LD D,A	 */	r->h.D = r->h.A; r->x.T = 4; break;

	case 0x58: /* LD E,B	 */	r->h.E = r->h.B; r->x.T = 4; break;
	case 0x59: /* LD E,C	 */	r->h.E = r->h.C; r->x.T = 4; break;
	case 0x5A: /* LD E,D	 */	r->h.E = r->h.D; r->x.T = 4; break;
	case 0x5B: /* LD E,E	 */	r->h.E = r->h.E; r->x.T = 4; break;
	case 0x5C: /* LD E,H	 */	r->h.E = r->h.H; r->x.T = 4; break;
	case 0x5D: /* LD E,L	 */	r->h.E = r->h.L; r->x.T = 4; break;
	case 0x5E: /* LD E,(HL)	 */	r->h.E = __z80_mbread(cpu, r->x.HL); r->x.T = 7; break;
	case 0x5F: /* LD E,A	 */	r->h.E = r->h.A; r->x.T = 4; break;

	case 0x60: /* LD H,B	 */	r->h.H = r->h.B; r->x.T = 4; break;
	case 0x61: /* LD H,C	 */	r->h.H = r->h.C; r->x.T = 4; break;
	case 0x62: /* LD H,D	 */	r->h.H = r->h.D; r->x.T = 4; break;
	case 0x63: /* LD H,E	 */	r->h.H = r->h.E; r->x.T = 4; break;
	case 0x64: /* LD H,H	 */	r->h.H = r->h.H; r->x.T = 4; break;
	case 0x65: /* LD H,L	 */	r->h.H = r->h.L; r->x.T = 4; break;
	case 0x66: /* LD H,(HL)	 */	r->h.H = __z80_mbread(cpu, r->x.HL); r->x.T = 7; break;
	case 0x67: /* LD H,A	 */	r->h.H = r->h.A; r->x.T = 4; break;

	case 0x68: /* LD L,B	 */	r->h.L = r->h.B; r->x.T = 4; break;
	case 0x69: /* LD L,C	 */	r->h.L = r->h.C; r->x.T = 4; break;
	case 0x6A: /* LD L,D	 */	r->h.L = r->h.D; r->x.T = 4; break;
	case 0x6B: /* LD L,E	 */	r->h.L = r->h.E; r->x.T = 4; break;
	case 0x6C: /* LD L,H	 */	r->h.L = r->h.H; r->x.T = 4; break;
	case 0x6D: /* LD L,L	 */	r->h.L = r->h.L; r->x.T = 4; break;
	case 0x6E: /* LD L,(HL)	 */	r->h.L = __z80_mbread(cpu, r->x.HL); r->x.T = 7; break;
	case 0x6F: /* LD L,A	 */	r->h.L = r->h.A; r->x.T = 4; break;

	case 0x70: /* LD (HL),B	 */	__z80_mbwrite(cpu, r->x.HL, r->h.B); r->x.T = 7; break;
	case 0x71: /* LD (HL),C	 */	__z80_mbwrite(cpu, r->x.HL, r->h.C); r->x.T = 7; break;
	case 0x72: /* LD (HL),D	 */	__z80_mbwrite(cpu, r->x.HL, r->h.D); r->x.T = 7; break;
	case 0x73: /* LD (HL),E	 */	__z80_mbwrite(cpu, r->x.HL, r->h.E); r->x.T = 7; break;
	case 0x74: /* LD (HL),H	 */	__z80_mbwrite(cpu, r->x.HL, r->h.H); r->x.T = 7; break;
	case 0x75: /* LD (HL),L	 */	__z80_mbwrite(cpu, r->x.HL, r->h.L); r->x.T = 7; break;
	case 0x76: /* HALT		 */	__z80_halt_instruction(cpu); break;
	case 0x77: /* LD (HL),A	 */	__z80_mbwrite(cpu, r->x.HL, r->h.A); r->x.T = 7; break;

	case 0x78: /* LD A,B	 */	r->h.A = r->h.B; r->x.T = 4; break;
	case 0x79: /* LD A,C	 */	r->h.A = r->h.C; r->x.T = 4; break;
	case 0x7A: /* LD A,D	 */	r->h.A = r->h.D; r->x.T = 4; break;
	case 0x7B: /* LD A,E	 */	r->h.A = r->h.E; r->x.T = 4; break;
	case 0x7C: /* LD A,H	 */	r->h.A = r->h.H; r->x.T = 4; break;
	case 0x7D: /* LD A,L	 */	r->h.A = r->h.L; r->x.T = 4; break;
	case 0x7E: /* LD A,(HL)	 */	r->h.A = __z80_mbread(cpu, r->x.HL); r->x.T = 7; break;
	case 0x7F: /* LD A,A	 */	r->h.A = r->h.A; r->x.T = 4; break;

	case 0x80: /* ADD A,B	 */	__z80_microcode_add8(&r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0x81: /* ADD A,C	 */	__z80_microcode_add8(&r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0x82: /* ADD A,D	 */	__z80_microcode_add8(&r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0x83: /* ADD A,E	 */	__z80_microcode_add8(&r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0x84: /* ADD A,H	 */	__z80_microcode_add8(&r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0x85: /* ADD A,L	 */	__z80_microcode_add8(&r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0x86: /* ADD A,(HL) */	__z80_microcode_add8(&r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0x87: /* ADD A,A	 */	__z80_microcode_add8(&r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0x88: /* ADC A,B	 */	__z80_microcode_adc8(&r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0x89: /* ADC A,C	 */	__z80_microcode_adc8(&r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0x8A: /* ADC A,D	 */	__z80_microcode_adc8(&r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0x8B: /* ADC A,E	 */	__z80_microcode_adc8(&r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0x8C: /* ADC A,H	 */	__z80_microcode_adc8(&r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0x8D: /* ADC A,L	 */	__z80_microcode_adc8(&r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0x8E: /* ADC A,(HL) */	__z80_microcode_adc8(&r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0x8F: /* ADC A,A	 */	__z80_microcode_adc8(&r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0x90: /* SUB A,B	 */	__z80_microcode_sub8(&r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0x91: /* SUB A,C	 */	__z80_microcode_sub8(&r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0x92: /* SUB A,D	 */	__z80_microcode_sub8(&r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0x93: /* SUB A,E	 */	__z80_microcode_sub8(&r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0x94: /* SUB A,H	 */	__z80_microcode_sub8(&r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0x95: /* SUB A,L	 */	__z80_microcode_sub8(&r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0x96: /* SUB A,(HL) */	__z80_microcode_sub8(&r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0x97: /* SUB A,A	 */	__z80_microcode_sub8(&r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0x98: /* SBC A,B	 */	__z80_microcode_sbc8(&r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0x99: /* SBC A,C	 */	__z80_microcode_sbc8(&r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0x9A: /* SBC A,D	 */	__z80_microcode_sbc8(&r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0x9B: /* SBC A,E	 */	__z80_microcode_sbc8(&r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0x9C: /* SBC A,H	 */	__z80_microcode_sbc8(&r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0x9D: /* SBC A,L	 */	__z80_microcode_sbc8(&r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0x9E: /* SBC A,(HL) */	__z80_microcode_sbc8(&r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0x9F: /* SBC A,A	 */	__z80_microcode_sbc8(&r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0xA0: /* AND A,B	 */	__z80_microcode_and8(&r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0xA1: /* AND A,C	 */	__z80_microcode_and8(&r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0xA2: /* AND A,D	 */	__z80_microcode_and8(&r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0xA3: /* AND A,E	 */	__z80_microcode_and8(&r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0xA4: /* AND A,H	 */	__z80_microcode_and8(&r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0xA5: /* AND A,L	 */	__z80_microcode_and8(&r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0xA6: /* AND A,(HL) */	__z80_microcode_and8(&r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0xA7: /* AND A,A	 */	__z80_microcode_and8(&r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0xA8: /* XOR A,B	 */	__z80_microcode_xor8(&r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0xA9: /* XOR A,C	 */	__z80_microcode_xor8(&r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0xAA: /* XOR A,D	 */	__z80_microcode_xor8(&r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0xAB: /* XOR A,E	 */	__z80_microcode_xor8(&r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0xAC: /* XOR A,H	 */	__z80_microcode_xor8(&r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0xAD: /* XOR A,L	 */	__z80_microcode_xor8(&r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0xAE: /* XOR A,(HL) */	__z80_microcode_xor8(&r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0xAF: /* XOR A,A	 */	__z80_microcode_xor8(&r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0xB0: /* OR A,B	 */	__z80_microcode_or8(&r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0xB1: /* OR A,C	 */	__z80_microcode_or8(&r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0xB2: /* OR A,D	 */	__z80_microcode_or8(&r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0xB3: /* OR A,E	 */	__z80_microcode_or8(&r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0xB4: /* OR A,H	 */	__z80_microcode_or8(&r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0xB5: /* OR A,L	 */	__z80_microcode_or8(&r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0xB6: /* OR A,(HL)  */	__z80_microcode_or8(&r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0xB7: /* OR A,A	 */	__z80_microcode_or8(&r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0xB8: /* CP A,B	 */	__z80_microcode_cp8(r->h.A, r->h.B, &r->h.F); r->x.T = 4; break;
	case 0xB9: /* CP A,C	 */	__z80_microcode_cp8(r->h.A, r->h.C, &r->h.F); r->x.T = 4; break;
	case 0xBA: /* CP A,D	 */	__z80_microcode_cp8(r->h.A, r->h.D, &r->h.F); r->x.T = 4; break;
	case 0xBB: /* CP A,E	 */	__z80_microcode_cp8(r->h.A, r->h.E, &r->h.F); r->x.T = 4; break;
	case 0xBC: /* CP A,H	 */	__z80_microcode_cp8(r->h.A, r->h.H, &r->h.F); r->x.T = 4; break;
	case 0xBD: /* CP A,L	 */	__z80_microcode_cp8(r->h.A, r->h.L, &r->h.F); r->x.T = 4; break;
	case 0xBE: /* CP A,(HL)  */	__z80_microcode_cp8(r->h.A, __z80_mbread(cpu, r->x.HL), &r->h.F); r->x.T = 7; break;
	case 0xBF: /* CP A,A	 */	__z80_microcode_cp8(r->h.A, r->h.A, &r->h.F); r->x.T = 4; break;

	case 0xC0: /* RET NZ	 */	if (!(r->h.F & _ZF)) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xC1: /* POP BC	 */	r->x.BC = __z80_pop(cpu); r->x.T = 10; break;
	case 0xC2: /* JP NZ,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (!(r->h.F & _ZF)) r->x.PC = pc; r->x.T = 10;} break;
	case 0xC3: /* JP nn		 */ {USHORT pc = __z80_fetchw(cpu); r->x.PC = pc; r->x.T = 10;} break;
	case 0xC4: /* CALL NZ,nn */	{USHORT nn = __z80_fetchw(cpu);
									if (!(r->h.F & _ZF)) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xC5: /* PUSH BC	 */	__z80_pushw(cpu, r->x.BC); r->x.T = 11; break;
	case 0xC6: /* ADD A,n	 */	__z80_microcode_add8(&r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xC7: /* RST 00H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0000; r->x.T = 11; break;

	case 0xC8: /* RET Z		 */	if (r->h.F & _ZF) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xC9: /* RET		 */	r->x.PC = __z80_pop(cpu); r->x.T = 10; break;
	case 0xCA: /* JP Z,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (r->h.F & _ZF) r->x.PC = pc; r->x.T = 10;} break;
	case 0xCB:					_z80_decoder_cb(cpu); break;
	case 0xCC: /* CALL Z,nn  */	{USHORT nn = __z80_fetchw(cpu);
									if (r->h.F & _ZF) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xCD: /* CALL nn	 */	{USHORT nn = __z80_fetchw(cpu); __z80_pushw(cpu, r->x.PC); r->x.PC = nn; r->x.T = 17;} break;
	case 0xCE: /* ADC A,n	 */	__z80_microcode_adc8(&r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xCF: /* RST 08H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0008; r->x.T = 11; break;

	case 0xD0: /* RET NC	 */	if (!(r->h.F & _CF)) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xD1: /* POP DE	 */	r->x.DE = __z80_pop(cpu); r->x.T = 10; break;
	case 0xD2: /* JP NC,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (!(r->h.F & _CF)) r->x.PC = pc; r->x.T = 10;} break;
	case 0xD3: /* OUT (n),A	 */	cpu->iowrite(MAKER16(r->h.A, __z80_fetchb(cpu)), r->h.A); r->x.T = 11; break;
	case 0xD4: /* CALL NC,nn */	{USHORT nn = __z80_fetchw(cpu);
									if (!(r->h.F & _CF)) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xD5: /* PUSH DE	 */	__z80_pushw(cpu, r->x.DE); r->x.T = 11; break;
	case 0xD6: /* SUB A,n	 */	__z80_microcode_sub8(&r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xD7: /* RST 10H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0010; r->x.T = 11; break;

	case 0xD8: /* RET C		 */	if (r->h.F & _CF) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xD9: /* EXX		 */	SWAP8(&r->h.B, &r->h.B2);
								SWAP8(&r->h.C, &r->h.C2);
								SWAP8(&r->h.D, &r->h.D2);
								SWAP8(&r->h.E, &r->h.E2);
								SWAP8(&r->h.H, &r->h.H2);
								SWAP8(&r->h.L, &r->h.L2);
								r->x.T = 4;
								break;
	case 0xDA: /* JP C,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (r->h.F & _CF) r->x.PC = pc; r->x.T = 10;} break;
	case 0xDB: /* IN A,(n)	 */	r->h.A = cpu->ioread(MAKER16(r->h.A, __z80_fetchb(cpu))); r->x.T = 11; break;
	case 0xDC: /* CALL C,nn  */	{USHORT nn = __z80_fetchw(cpu);
									if (r->h.F & _CF) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xDD:					_z80_decoder_xd(cpu, Z80_IX, Z80_IXH, Z80_IXL); break;
	case 0xDE: /* SBC A,n	 */	__z80_microcode_sbc8(&r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xDF: /* RST 18H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0018; r->x.T = 11; break;

	case 0xE0: /* RET PO	 */	if (!(r->h.F & _PF)) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xE1: /* POP HL	 */	r->x.HL = __z80_pop(cpu); r->x.T = 10; break;
	case 0xE2: /* JP PO,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (!(r->h.F & _PF)) r->x.PC = pc; r->x.T = 10;} break;
	case 0xE3: /* EX (SP),HL */	{USHORT x = __z80_mwread(cpu, r->x.SP); SWAP16(&x, &r->x.HL); __z80_mwwrite(cpu, r->x.SP, x); r->x.T = 19;}; break;
	case 0xE4: /* CALL PO,nn */	{USHORT nn = __z80_fetchw(cpu);
									if (!(r->h.F & _PF)) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xE5: /* PUSH HL	 */	__z80_pushw(cpu, r->x.HL); r->x.T = 11; break;
	case 0xE6: /* AND A,n	 */	__z80_microcode_and8(&r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xE7: /* RST 20H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0020; r->x.T = 11; break;

	case 0xE8: /* RET PE	 */	if (r->h.F & _PF) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xE9: /* JP (HL)	 */	r->x.PC = r->x.HL; r->x.T = 4; break;
	case 0xEA: /* JP PE,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (r->h.F & _PF) r->x.PC = pc; r->x.T = 10;} break;
	case 0xEB: /* EX DE,HL	 */ SWAP16(&r->x.DE, &r->x.HL); r->x.T = 4; break;
	case 0xEC: /* CALL PC,nn */	{USHORT nn = __z80_fetchw(cpu);
									if (r->h.F & _PF) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xED:					_z80_decoder_ed(cpu); break;
	case 0xEE: /* XOR A,n	 */	__z80_microcode_xor8(&r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xEF: /* RST 28H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0028; r->x.T = 11; break;

	case 0xF0: /* RET P		 */	if (!(r->h.F & _SF)) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xF1: /* POP AF	 */	r->x.AF = __z80_pop(cpu); r->x.T = 10; break;
	case 0xF2: /* JP P,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (!(r->h.F & _SF)) r->x.PC = pc; r->x.T = 10;} break;
	case 0xF3: /* DI		 */	r->x.IFF1 = 0; r->x.IFF2 = 0; r->x.T = 4; break;
	case 0xF4: /* CALL P,nn  */	{USHORT nn = __z80_fetchw(cpu);
									if (!(r->h.F & _SF)) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xF5: /* PUSH AF	 */	__z80_pushw(cpu, r->x.AF); r->x.T = 11; break;
	case 0xF6: /* OR A,n	 */	__z80_microcode_or8(&r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xF7: /* RST 30H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0030; r->x.T = 11; break;

	case 0xF8: /* RET M		 */	if (r->h.F & _SF) {r->x.PC = __z80_pop(cpu); r->x.T = 17;} else r->x.T = 5; break;
	case 0xF9: /* LD SP,HL	 */	r->x.SP = r->x.HL; r->x.T = 6; break;
	case 0xFA: /* JP M,nn	 */ {USHORT pc = __z80_fetchw(cpu); if (r->h.F & _SF) r->x.PC = pc; r->x.T = 10;} break;
	case 0xFB: /* EI		 */	r->x.IFF1 = 1; r->x.IFF2 = 1; r->x.T = 4; break;
	case 0xFC: /* CALL M,nn */	{USHORT nn = __z80_fetchw(cpu);
									if (r->h.F & _SF) {
										__z80_pushw(cpu, r->x.PC);
										r->x.PC = nn;
										r->x.T = 17;
									} else
										r->x.T = 10;
								}; break;
	case 0xFD:					_z80_decoder_xd(cpu, Z80_IY, Z80_IYH, Z80_IYL); break;
	case 0xFE: /* CP A,n	 */	__z80_microcode_cp8(r->h.A, __z80_fetchb(cpu), &r->h.F); r->x.T = 7; break;
	case 0xFF: /* RST 38H	 */	__z80_pushw(cpu, r->x.PC); r->x.PC = 0x0038; r->x.T = 11; break;
	}
}

#ifdef __cplusplus
}	// extern "C"
#endif
