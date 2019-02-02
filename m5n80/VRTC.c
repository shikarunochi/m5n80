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
static void _n80_reset_vrtc() {

}

static int _n80_blanking_vrtc(DWORD T) {
	int result = 0;
	if (n80device.vrtc.vsync_line < 200) {				// vertical display interval
		n80device.vrtc.hsync_state += T;
		if (n80device.vrtc.hsync_state < 179)			// horizontal display interval
			;
		else if (n80device.vrtc.hsync_state < 250)		// horizontal blanking interval
			;
		else {											// end of 1-line display
			// DMA transfer start (CPU WAIT) if dma ch.2 is active or crts start display
			if ((n80device.crtc._n80_text_vis & 0x06) == 0)
				n80device.dmac._penarty = 67;
			else
				n80device.dmac._penarty = 0;
			if (++n80device.vrtc.vsync_line >= 200) {	// vertical blanking interval start
				n80device.vrtc.vsync_state = 0;
				n80device.vrtc._vrtc_flag  = 0xff;
				n80device.vrtc.edge();
				result = 1;
			} else
				n80device.vrtc.hsync_state -= 250;
		}
	} else {											// vertical blanking interval
		n80device.vrtc.vsync_state += T;
		if (n80device.vrtc.vsync_state > 14016) {		// end of VBI
			n80device.vrtc.vsync_line   = 0;
			n80device.vrtc.hsync_state  = n80device.vrtc.vsync_state - 14016;
			n80device.vrtc._vrtc_flag   = 0;
		}
	}
	return result;
}
	
static void _n80_renew_vrtc()
{
	if (++n80device.vrtc._vrtc_blink >= 30) {
		n80device.vrtc._vrtc_blink = 0;
		n80device.cursor._n80_cur_blink ^= 1;
		if (n80device.cursor._n80_cur_enable)
			n80device.attr.set(n80device.cursor._n80_cur_row, 1);
	}
}

static BYTE _n80_poll_vrtc(BYTE i40)
{
	if (n80device.vrtc._vrtc_flag)
		return i40 |= 0x20;
	else
		return i40 &= 0xdf;
}

export void _n80_init_vrtc() {
	n80device.vrtc.reset = _n80_reset_vrtc;
	n80device.vrtc.blanking = _n80_blanking_vrtc;
	n80device.vrtc.edge = _n80_renew_vrtc;
	n80device.vrtc.poll = _n80_poll_vrtc;
	_n80_reset_vrtc();
}
	
#ifdef __cplusplus
}	// extern "C"
#endif
