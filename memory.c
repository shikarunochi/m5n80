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
#ifdef __cplusplus
extern "C"
{
#endif
/**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
export BYTE __cdecl _n80_mbread(USHORT Adr, DWORD *W)
{
	BYTE mb = n80device.mem.arr[Adr];
	++(*W);
	return mb;
}

export void __cdecl _n80_mbwrite(USHORT Adr, BYTE v, DWORD *W)
{
	if (Adr >= 0x8000) {
		++(*W);
		if (Adr >= 0xf300 && Adr < 0xfeb8) {
			// Sync Video RAM Area
			n80device.vram_sync_enter();
			n80device.mem.arr[Adr] = v;
			n80device.attr._attrchg[((Adr - 0xf300) / 120)] = 1;
			n80device.vram_sync_leave();
		} else {
			n80device.mem.arr[Adr] = v;
		}
	}
}

#ifdef __cplusplus
}	// extern "C"
#endif
