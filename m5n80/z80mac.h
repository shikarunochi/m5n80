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
#ifndef _Z80MAC_H_
#define _Z80MAC_H_
#ifndef _MSC_VER
#define __cdecl
#define __inline inline
#else
#include <windows.h>
#endif
#include <stdio.h>
#ifdef __cplusplus
extern "C"
{
#endif
/**--------------------------------------------------------------------------
 **		Z80 microcode inline functions
 **------------------------------------------------------------------------*/
extern volatile BYTE z80_signal;		// z80 control signals
/**--------------------------------------------------------------------------
 **		Z80 debug
 **------------------------------------------------------------------------*/
static __inline void __dbpr(LPZ80CPU cpu, const char *fmt) {
	sprintf(cpu->dbg_mnemonic, fmt);
}
static __inline void __dbprb(LPZ80CPU cpu, const char *fmt, BYTE b) {
	sprintf(cpu->dbg_mnemonic, fmt, b);
}
static __inline void __dbprw(LPZ80CPU cpu, const char *fmt, USHORT w) {
	sprintf(cpu->dbg_mnemonic, fmt, w);
}
/**--------------------------------------------------------------------------
 **		Z80 macro
 **------------------------------------------------------------------------*/
static __inline USHORT	MAKER16(BYTE h, BYTE l) { return (((USHORT) h) << 8) | ((USHORT) l); }
static __inline BYTE	R16L(USHORT x) { return (BYTE) (x & 0xff); }
static __inline BYTE	R16H(USHORT x) { return (BYTE) ((x >> 8) & 0xff); }
static __inline void	SWAP8(LPBYTE r1, LPBYTE r2) { BYTE t = *r1; *r1 = *r2; *r2 = t; }
static __inline void	SWAP16(USHORT *r1, USHORT *r2) { USHORT t = *r1; *r1 = *r2; *r2 = t; }
/**--------------------------------------------------------------------------
 **		Z80 microcode inline functions (Flag checker)
 **------------------------------------------------------------------------*/
static __inline BYTE __z80_mbread(LPZ80CPU cpu, USHORT adr) {
	return cpu->mbread(adr, &cpu->r.x.W);
}
static __inline void __z80_mbwrite(LPZ80CPU cpu, USHORT adr, BYTE v) {
	cpu->mbwrite(adr, v, &cpu->r.x.W);
}
static __inline USHORT __z80_mwread(LPZ80CPU cpu, USHORT adr) {
	BYTE dl = cpu->mbread(adr, &cpu->r.x.W);
	BYTE dh = cpu->mbread(++adr, &cpu->r.x.W);
	return MAKER16(dh, dl);
}
static __inline void __z80_mwwrite(LPZ80CPU cpu, USHORT adr, USHORT v) {
	cpu->mbwrite(adr, R16L(v), &cpu->r.x.W);
	cpu->mbwrite(++adr, R16H(v), &cpu->r.x.W);
}
static __inline BYTE __z80_fetchb(LPZ80CPU cpu) {
	if (cpu->req_mode0 == 1) {
		// MODE 0: fetch from data bus which is set by interrupt device
		if (cpu->int_dbus_size > 0) {
			BYTE d = cpu->int_dbus[--cpu->int_dbus_size];
			if (cpu->int_dbus_size == 0)
				cpu->req_mode0 = 0;
			return d;
		} else {
			cpu->signal = _Z80HALT_BUS;	// data bus error
			return 0x00;	//NOP
		}
	} else
		return cpu->mbread(cpu->r.x.PC++, &cpu->r.x.W);
}
static __inline USHORT __z80_fetchw(LPZ80CPU cpu) {
	if (cpu->req_mode0 == 1) {
		// MODE 0: fetch from data bus which is set by interrupt device
		if (cpu->int_dbus_size > 1) {
			BYTE bl = cpu->int_dbus[--cpu->int_dbus_size];
			BYTE bh = cpu->int_dbus[--cpu->int_dbus_size];
			if (cpu->int_dbus_size == 0)
				cpu->req_mode0 = 0;
			return MAKER16(bh, bl);
		} else {
			cpu->signal = _Z80HALT_BUS;	// data bus error
			return 0x0000;
		}
	} else {
		BYTE bl = cpu->mbread(cpu->r.x.PC++, &cpu->r.x.W);
		BYTE bh = cpu->mbread(cpu->r.x.PC++, &cpu->r.x.W);
		return MAKER16(bh, bl);
	}
}
static __inline void __z80_pushw(LPZ80CPU cpu, USHORT v) {
	cpu->mbwrite(--cpu->r.x.SP, R16H(v), &cpu->r.x.W);
	cpu->mbwrite(--cpu->r.x.SP, R16L(v), &cpu->r.x.W);
}
static __inline USHORT __z80_pop(LPZ80CPU cpu) {
	BYTE bl = cpu->mbread(cpu->r.x.SP++, &cpu->r.x.W);
	BYTE bh = cpu->mbread(cpu->r.x.SP++, &cpu->r.x.W);
	return MAKER16(bh, bl);
}
static __inline USHORT __z80_index(USHORT idx, BYTE d) {
	return idx + ((USHORT) ((signed char) d));
}
/**--------------------------------------------------------------------------
 **		Z80 microcode inline functions (illegal instructions)
 **------------------------------------------------------------------------*/
static __inline void __z80_halt_instruction(LPZ80CPU cpu) {
	cpu->signal = 1;
	cpu->r.x.HALT = _Z80HALT_76;
}
static __inline void __z80_illegal_instruction(LPZ80CPU cpu) {
	cpu->signal = 1;
	cpu->r.x.HALT = _Z80HALT_IL;
}
static __inline void __z80_unimplemented_instruction(LPZ80CPU cpu) {
	cpu->signal = 1;
	cpu->r.x.HALT = _Z80HALT_UI;
}
/**--------------------------------------------------------------------------
 **		Z80 microcode inline functions (Flag checker)
 **------------------------------------------------------------------------*/
static __inline BYTE __z80_microcode_sf8(BYTE r)
{
	return (r & _SF);
}

static __inline BYTE __z80_microcode_sf16(int r)
{
	return (r & 0x8000) >> 8;
}

static __inline BYTE __z80_microcode_zf8(BYTE r)
{
	return r == 0 ? _ZF : 0;
}

static __inline BYTE __z80_microcode_zf16(int r)
{
	return (r & 0xffff) == 0 ? _ZF : 0;
}

static __inline BYTE __z80_microcode_yxf(BYTE r)
{
	return (r & (_XF | _YF));
}

static __inline BYTE __z80_microcode_hf8(BYTE r1, BYTE r2, BYTE r3)
{
	return ((r1 ^ r2) ^ r3) & _HF;
}

static __inline BYTE __z80_microcode_pf8(BYTE r)
{
	BYTE pf = 0;
	int i;
	for (i = 0; i < 8; ++i) {
		pf ^= r;
		r >>= 1;
	}
	return ((~pf & 0x01) << 2);
}

static __inline BYTE __z80_microcode_vf8add(BYTE r1, BYTE r2, BYTE r3)
{
	return ((~(r1 ^ r2) & (r1 ^ r3)) >> 5) & _VF;
}

static __inline BYTE __z80_microcode_vf8sub(BYTE r1, BYTE r2, BYTE r3)
{
	return (((r1 ^ r2) & (r1 ^ r3)) >> 5) & _VF;
}

static __inline BYTE __z80_microcode_cf8(int r)
{
	return (r & 0x100) ? _CF : 0;
}

static __inline BYTE __z80_microcode_cf16(int r)
{
	return (r & 0x10000) ? _CF : 0;
}
/**--------------------------------------------------------------------------
 **		Z80 microcode inline functions (ADD/SUB/INC/DEC)
 **------------------------------------------------------------------------*/
static __inline void __z80_microcode_add16(LPBYTE h1, LPBYTE l1, BYTE h2, BYTE l2, LPBYTE flag) {
	BYTE f = *flag & (_SF | _ZF | _PF);
	int lr = (int) *l1 + (int) l2, hr;
	if (__z80_microcode_cf8(lr) == _CF) {
		hr = (int) *h1 + (int) h2 + 1;
	} else {
		hr = (int) *h1 + (int) h2;
	}
	f |= __z80_microcode_yxf(hr);
	f |= __z80_microcode_hf8(*h1, h2, hr);
	f |= __z80_microcode_cf8(hr);
	*flag = f;
	*h1 = (hr & 0xff);
	*l1 = (lr & 0xff);
}

static __inline void __z80_microcode_adc16(LPBYTE h1, LPBYTE l1, BYTE h2, BYTE l2, LPBYTE flag) {
	USHORT r;
	BYTE f = 0;
	int lr = (int) *l1 + (int) l2 + ((*flag & _CF) ? 1 : 0), hr;
	if (__z80_microcode_cf8(lr) == _CF) {
		hr = (int) *h1 + (int) h2 + 1;
	} else {
		hr = (int) *h1 + (int) h2;
	}
	r = MAKER16((hr & 0xff), (lr & 0xff));
	f |= __z80_microcode_sf16(r);
	f |= __z80_microcode_zf16(r);
	f |= __z80_microcode_yxf(hr);
	f |= __z80_microcode_hf8(*h1, h2, hr);
	f |= __z80_microcode_vf8add(*h1, h2, hr);
	f |= __z80_microcode_cf8(hr);
	*h1 = (hr & 0xff);
	*l1 = (lr & 0xff);
	*flag = f;
}

static __inline void __z80_microcode_sbc16(LPBYTE h1, LPBYTE l1, BYTE h2, BYTE l2, LPBYTE flag) {
	USHORT r;
	BYTE f = 0;
	int lr = (int) *l1 - (int) l2 - ((*flag & _CF) ? 1 : 0), hr;
	if (__z80_microcode_cf8(lr) == _CF) {
		hr = (int) *h1 - (int) h2 - 1;
	} else {
		hr = (int) *h1 - (int) h2;
	}
	r = MAKER16((hr & 0xff), (lr & 0xff));
	f |= __z80_microcode_sf16(r);
	f |= __z80_microcode_zf16(r);
	f |= __z80_microcode_yxf(hr);
	f |= __z80_microcode_hf8(*h1, h2, hr);
	f |= __z80_microcode_vf8sub(*h1, h2, hr);
	f |= __z80_microcode_cf8(hr);
	*h1 = (hr & 0xff);
	*l1 = (lr & 0xff);
	*flag = f;
}

static __inline void __z80_microcode_add8(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = 0;
	int ir = (int) *r1 + (int) r2;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_hf8(*r1, r2, ir);
	f |= __z80_microcode_vf8add(*r1, r2, ir);
	f |= __z80_microcode_cf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_adc8(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = 0;
	int ir = (int) *r1 + (int) r2 + ((*flag & _CF) ? 1 : 0);
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_hf8(*r1, r2, ir);
	f |= __z80_microcode_vf8add(*r1, r2, ir);
	f |= __z80_microcode_cf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_sub8(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = _NF;
	int ir = (int) *r1 - (int) r2;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_hf8(*r1, r2, ir);
	f |= __z80_microcode_vf8sub(*r1, r2, ir);
	f |= __z80_microcode_cf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_sbc8(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = _NF;
	int ir = (int) *r1 - (int) r2 - ((*flag & _CF) ? 1 : 0);
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_hf8(*r1, r2, ir);
	f |= __z80_microcode_vf8sub(*r1, r2, ir);
	f |= __z80_microcode_cf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_cp8(BYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = _NF;
	int ir = (int) r1 - (int) r2;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(r2);
	f |= __z80_microcode_hf8(r1, r2, ir);
	f |= __z80_microcode_vf8sub(r1, r2, ir);
	f |= __z80_microcode_cf8(ir);
	*flag = f;
}

static __inline void __z80_microcode_inc8(LPBYTE r, LPBYTE flag) {
	BYTE f = *flag & _CF;
	int ir = (int) *r + 1;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_hf8(*r, 1, ir);
	f |= __z80_microcode_vf8add(*r, 1, ir);
	*flag = f;
	*r = (ir & 0xff);
}

static __inline void __z80_microcode_dec8(LPBYTE r, LPBYTE flag) {
	BYTE f = (*flag & _CF) | _NF;
	int ir = (int) *r - 1;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_hf8(*r, 1, ir);
	f |= __z80_microcode_vf8sub(*r, 1, ir);
	*flag = f;
	*r = (ir & 0xff);
}

static __inline void __z80_microcode_and8(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = _HF;
	int ir = (int) *r1 & (int) r2;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_xor8(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = 0;
	int ir = (int) *r1 ^ (int) r2;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_or8(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = 0;
	int ir = (int) *r1 | (int) r2;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_daa(LPBYTE r1, BYTE r2, LPBYTE flag) {
	BYTE f = *flag & _NF;
	int ir = (int) *r1 + (int) r2;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_hf8(*r1, r2, ir);
	f |= __z80_microcode_pf8(ir);
	f |= __z80_microcode_cf8(ir);
	*flag = f;
	*r1 = (ir & 0xff);
}

static __inline void __z80_microcode_rlc(LPBYTE r, LPBYTE flag) {
	int ir = ((int) *r) << 1;
	int c = (ir >> 8) & 0x01;
	BYTE f = c;
	ir |= c;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = (ir & 0xff);
}

static __inline void __z80_microcode_rrc(LPBYTE r, LPBYTE flag) {
	BYTE c = *r & 0x01;
	BYTE ir = *r >> 1;
	BYTE f = c;
	ir |= (c << 7);
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = (ir & 0xff);
}

static __inline void __z80_microcode_rl(LPBYTE r, LPBYTE flag) {
	int ir = (((int) *r) << 1) | (*flag & _CF);
	int f = (ir >> 8) & 0x01;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = (ir & 0xff);
}

static __inline void __z80_microcode_rr(LPBYTE r, LPBYTE flag) {
	BYTE f = *r & 0x01;
	BYTE ir = (*r >> 1) | ((*flag & _CF) << 7);
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = (ir & 0xff);
}

static __inline void __z80_microcode_sla(LPBYTE r, LPBYTE flag) {
	BYTE f = (*r >> 7) & 0x01;
	BYTE ir = (*r << 1);
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = ir;
}

static __inline void __z80_microcode_sra(LPBYTE r, LPBYTE flag) {
	BYTE m = *r & 0x80;
	BYTE f = *r & 0x01;
	BYTE ir = (*r >> 1) | m;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = ir;
}

static __inline void __z80_microcode_sll(LPBYTE r, LPBYTE flag) {
	BYTE f = (*r >> 7) & 0x01;
	BYTE ir = (*r << 1) | 0x01;
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = ir;
}

static __inline void __z80_microcode_srl(LPBYTE r, LPBYTE flag) {
	BYTE f = *r & 0x01;
	BYTE ir = (*r >> 1);
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	*flag = f;
	*r = ir;
}

static __inline void __z80_microcode_bit(BYTE r, BYTE bit, LPBYTE flag) {
	BYTE f = (*flag & _CF) | _HF;
	BYTE b = r & bit;
	f |= __z80_microcode_sf8(b);
	f |= __z80_microcode_yxf(b);
	if (b == 0) {
		f |= (_ZF | _PF);
	}
	*flag = f;
}

static __inline void __z80_microcode_in(LPZ80CPU cpu, LPBYTE r, BYTE ph, BYTE pl) {
	BYTE f  = cpu->r.h.F & _CF;
	BYTE ir = cpu->ioread(MAKER16(ph, pl));
	f |= __z80_microcode_sf8(ir);
	f |= __z80_microcode_zf8(ir);
	f |= __z80_microcode_yxf(ir);
	f |= __z80_microcode_pf8(ir);
	cpu->r.h.F = f;
	*r = ir;
}

static __inline void __z80_microcode_ldair(LPZ80CPU cpu, BYTE r) {
	BYTE f = cpu->r.h.F & _CF;
	cpu->r.h.A = r;
	f |= __z80_microcode_sf8(r);
	f |= __z80_microcode_zf8(r);
	f |= __z80_microcode_yxf(r);
	if (cpu->r.x.IFF2)
		f |= _PF;
	cpu->r.h.F = f;
}

static __inline void __z80_microcode_rld(LPZ80CPU cpu) {
	BYTE f = cpu->r.h.F & _CF;
	BYTE n = __z80_mbread(cpu, cpu->r.x.HL);
	BYTE a = cpu->r.h.A;
	a &= 0xf0;
	a |= (n >> 4);
	n <<= 4;
	n |= (cpu->r.h.A & 0x0f);
	cpu->r.h.A = a;
	__z80_mbwrite(cpu, cpu->r.x.HL, n);
	f |= __z80_microcode_sf8(a);
	f |= __z80_microcode_zf8(a);
	f |= __z80_microcode_yxf(a);
	f |= __z80_microcode_pf8(a);
}

static __inline void __z80_microcode_rrd(LPZ80CPU cpu) {
	BYTE f = cpu->r.h.F & _CF;
	BYTE n = __z80_mbread(cpu, cpu->r.x.HL);
	BYTE a = cpu->r.h.A;
	a &= 0xf0;
	a |= (n & 0x0f);
	n >>= 4;
	n |= (cpu->r.h.A << 4);
	cpu->r.h.A = a;
	__z80_mbwrite(cpu, cpu->r.x.HL, n);
	f |= __z80_microcode_sf8(a);
	f |= __z80_microcode_zf8(a);
	f |= __z80_microcode_yxf(a);
	f |= __z80_microcode_pf8(a);
}

static __inline void __z80_microcode_ldi(LPZ80CPU cpu) {
	BYTE f = cpu->r.h.F & (_SF | _ZF | _CF);
	BYTE n = __z80_mbread(cpu, cpu->r.x.HL);
	__z80_mbwrite(cpu, cpu->r.x.DE, n);
	++ cpu->r.x.HL;
	++ cpu->r.x.DE;
	-- cpu->r.x.BC;
	if (cpu->r.x.BC != 0)
		f |= _PF;
	n += cpu->r.h.A;
	f |= __z80_microcode_yxf(n);
	cpu->r.h.F = f;
}

static __inline void __z80_microcode_ldd(LPZ80CPU cpu) {
	BYTE f = cpu->r.h.F & (_SF | _ZF | _CF);
	BYTE n = __z80_mbread(cpu, cpu->r.x.HL);
	__z80_mbwrite(cpu, cpu->r.x.DE, n);
	-- cpu->r.x.HL;
	-- cpu->r.x.DE;
	-- cpu->r.x.BC;
	if (cpu->r.x.BC != 0)
		f |= _PF;
	n += cpu->r.h.A;
	f |= __z80_microcode_yxf(n);
	cpu->r.h.F = f;
}

static __inline void __z80_microcode_cpi(LPZ80CPU cpu) {
	BYTE f = (cpu->r.h.F & _CF) | _NF;
	BYTE n = __z80_mbread(cpu, cpu->r.x.HL);
	BYTE a = cpu->r.h.A - n;
	f |= __z80_microcode_sf8(a);
	f |= __z80_microcode_zf8(a);
	f |= __z80_microcode_hf8(cpu->r.h.A, n, a);
	if (f & _HF)
		a -= 1;
	f |= __z80_microcode_yxf(a);
	++ cpu->r.x.HL;
	-- cpu->r.x.BC;
	if (cpu->r.x.BC != 0)
		f |= _PF;
	cpu->r.h.F = f;
}

static __inline void __z80_microcode_cpd(LPZ80CPU cpu) {
	BYTE f = (cpu->r.h.F & _CF) | _NF;
	BYTE n = __z80_mbread(cpu, cpu->r.x.HL);
	BYTE a = cpu->r.h.A - n;
	f |= __z80_microcode_sf8(a);
	f |= __z80_microcode_zf8(a);
	f |= __z80_microcode_hf8(cpu->r.h.A, n, a);
	if (f & _HF)
		a -= 1;
	f |= __z80_microcode_yxf(a);
	-- cpu->r.x.HL;
	-- cpu->r.x.BC;
	if (cpu->r.x.BC != 0)
		f |= _PF;
	cpu->r.h.F = f;
}

#ifdef __cplusplus
}	// extern "C"
#endif
#endif//_Z80MAC_H_
