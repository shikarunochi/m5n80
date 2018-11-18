/**==========================================================================

	PC-8001 Device Emulator for Win32 Ver 1.10
	Copyright (C)1993-2010 H.Kanemoto

	This is a free software. You may copy or distribute
	it without any permissions. But there are NO WARRANTY
	of use. Entire risk of use is with you.

	External assembler functions for IBM-PC with VC++ 5.0
	last modified  Apr,4,1998.

;==========================================================================
**------------------------------------------------------------------------
**  For  M5Stack 2018/11/18 modified by @shikarunochi 
**------------------------------------------------------------------------*/

#include "pc-8001.h"
#ifdef _MSC_VER
#include <windows.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
static __inline void set_pcg_freq()
{
	USHORT	timer;
	DWORD	freq, prev;

	timer = (((USHORT) n80device.hal._timmr_hi) << 8) + n80device.hal._timmr_lo;
	// freq = 4MHz(CPU clock) / i8253 Counter
	if (timer == 0)
		timer = 1;	// 0除算防止
	freq = (DWORD) ((double) n80device.param.hz / (double) timer + 0.5);
	if (freq < 24)
		freq = 24;
	//else if (freq > 4800)
	//	freq = 0;
	prev = n80device.hal._pcg_freq;
	if (prev != freq) {
		n80device.hal._pcg_freq = (USHORT) freq;
//		if (n80device.hal._sound_is_on)
//			n80device.sound_on(freq);
	}
}

export void _n80_pcg_mode(BYTE port, BYTE dat)
{
	USHORT	offset;

	switch (port) {
	case 0x00:	// save PCG char data
				n80device.hal._pcg_dat = dat; break;
	case 0x01:	// PCG RAM low address
				n80device.hal._pcg_lo = dat; break;
	case 0x02:	// PCG sound on/off
//				if (dat & 0x08) {
//					if (!n80device.hal._sound_is_on) {
//						n80device.hal._sound_is_on = 1;
//						n80device.sound_on(n80device.hal._pcg_freq);
//					}
//				} else {
//					n80device.hal._sound_is_on = 0;
//					n80device.sound_off();
//				}
				// PCG RAM high address
				if (dat & 0x10) {
					n80device.hal._pcg_hi = dat & 0x03;	// address A8, A9 bits
					offset = (((USHORT) n80device.hal._pcg_hi) << 8) + n80device.hal._pcg_lo;
					if (dat & 0x20)	// copy data from font rom
						n80device.pcg[1024 + offset] = n80device.font[1024 + offset];
					else {
					#ifdef _ZAURUS_
						BYTE	bit = 0x01 << (offset % 8);
						BYTE 	ubit = ~bit, dat = n80device.hal._pcg_dat;
						USHORT	i, offtop = offset & 0xfff8;
						for (i = 0; i < 8; ++i) {
							if (dat & 0x80)
								n80device.pcg[1024 + offtop + i] |= bit;
							else
								n80device.pcg[1024 + offtop + i] &= ubit;
							dat <<= 1;
						}
					#else
						n80device.pcg[1024 + offset] = n80device.hal._pcg_dat;
					#endif
					}
				}
				break;
	case 0x03:	// PCG ON/OFF
				n80device.pcg_mode = (dat & 0x03) ? 1 : 0;	break;
	case 0x0c:	// i8253 timmer #0 counter
				switch (n80device.hal._timmr_mode) {
				case 3:	// load/store L/H order
						if (--n80device.hal._timmr_cont == 1) {
							n80device.hal._timmr_lo = dat;
						} else {
							n80device.hal._timmr_hi = dat;
							n80device.hal._timmr_cont = 2;
							set_pcg_freq();
						}
						break;

				case 2:	// load/store H
						n80device.hal._timmr_hi = dat;
						set_pcg_freq();
						break;

				case 1:	// load/store L
						n80device.hal._timmr_lo = dat;
						break;
				}
				break;

	case 0x0f:	// i8253 mode register
				if (dat & 0xc0)
					;	// ignore if timer #1,#2 is selected
				else {
					switch ((dat & 0x30) >> 4) {
					case 3:	n80device.hal._timmr_cont = 2; n80device.hal._timmr_mode = 3; break;
					case 2: n80device.hal._timmr_cont = 1; n80device.hal._timmr_mode = 2; n80device.hal._timmr_lo = 0; break;
					case 1: n80device.hal._timmr_cont = 1; n80device.hal._timmr_mode = 1; n80device.hal._timmr_lo = 0; break;
					default:
							n80device.hal._timmr_cont = 0; n80device.hal._timmr_mode = 0; break;
					}
				}
				break;
	}
}

#ifdef __cplusplus
}	// extern "C"
#endif
