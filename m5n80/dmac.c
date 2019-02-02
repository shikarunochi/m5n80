/**==========================================================================
 
	PC-8001 Device Emulator for Win32 Ver 1.10
	Copyright (C)1993-2010 H.Kanemoto
 
	This is a free software. You may copy or distribute
	it without any permissions. But there are NO WARRANTY
	of use. Entire risk of use is with you.
 
	External assembler functions for IBM-PC with VC++ 5.0
	last modified  Apr,4,1998.
 
 ;==========================================================================*/
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
static void _n80_reset_dmac() {
	n80device.dmac._begin = 0xf3c8;
	n80device.dmac._end = 0xff80;
	n80device.dmac._penarty = 0;
	n80device.dmac._dmaact = 0;
	n80device.dmac._ch2_wait_hl = 0;
	n80device.dmac._ch2_low = 0;
	n80device.dmac._ch2_hi = 0;
}

static void _n80_dmac_cmd() {
	BYTE en = n80device.io._o68 & 0x04;
	//BYTE chg = n80device.dmac._dmaact ^ en;
	if (en)
		n80device.crtc._n80_text_vis &= 0xfb;
	else
		n80device.crtc._n80_text_vis |= 4;
	n80device.dmac._dmaact = en;
}

static void _n80_dmac_ch2() {
	if (n80device.dmac._ch2_wait_hl == 0) {
		n80device.dmac._ch2_low = n80device.io._o64;
		n80device.dmac._ch2_wait_hl = 1;
	} else {
		USHORT addr;
		n80device.dmac._ch2_hi = n80device.io._o64;
		n80device.dmac._ch2_wait_hl = 0;
		addr = ((USHORT)n80device.dmac._ch2_hi << 8) + ((USHORT)n80device.dmac._ch2_low);
		n80device.dmac._begin = addr;
		n80device.dmac._end = addr + 3000;
	}
}

export void _n80_init_dmac() {
	n80device.dmac.reset = _n80_reset_dmac;
	n80device.dmac.cmd = _n80_dmac_cmd;
	n80device.dmac.ch2adr = _n80_dmac_ch2;
	_n80_reset_dmac();
}

#ifdef __cplusplus
}	// extern "C"
#endif
