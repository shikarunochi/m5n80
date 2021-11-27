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
extern void	chg_bltfunc(BYTE);
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#define export

#define	_NONE	(0)
#define	_OCW1	(1)
#define	_OCW5	(5)
/**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
static void _n80_crtc_cmd(BYTE cmd) {
	switch ((cmd & 0xe0) >> 5) {
	case 0:	// ocw1
			n80device.crtc._prev_cmd = _OCW1;
			n80device.crtc._param_count = 0;
			n80device.crtc._n80_text_vis |= 2;
			break;
	case 1:	// ocw2
			n80device.crtc._attr_reverse = cmd & 0x01 ? 1 : 0;
			n80device.attr.invalid();
			n80device.crtc._n80_text_vis &= 0xfd;
			break;
	case 4:	// ocw5
			n80device.crtc._prev_cmd = _OCW5;
			n80device.crtc._param_count = 0;
			if (cmd & 0x01)
				n80device.cursor.enable();
			else
				n80device.cursor.disable();
			break;

	case 2:	// ocw3
	case 3:	// ocw4
	case 5:	// ocw6
	case 6:	// ocw7
	case 7:	// ocw8
	default:
			n80device.crtc._prev_cmd = _NONE;
			n80device.crtc._param_count = 0;
			break;
	}
}

static void _n80_crtc_data(BYTE dat) {
	USHORT	cnt = n80device.crtc._param_count++;
	BYTE	al, ah;

	switch (n80device.crtc._prev_cmd) {
	case _OCW1:	if (cnt < 5) {
					if (cnt == 1) {	// _ocw1_fmt2
						ah = dat & 0x7f;
						al = ah == 24 ? 1 : 0;
						if (n80device.crtc._n80_line_mode != al) {
							n80device.crtc._n80_line_mode = al;
							if (n80device.crtc._n80_width_mode == 1)
								al += 0x02;
							n80device.crtc._n80_scrn_mode = al;
							n80device.attr.invalid();
#ifdef _MSC_VER
							chg_bltfunc(al);
#else
							n80device.chg_crt_mode(al);
#endif
						}
					}
				} else
					n80device.crtc._prev_cmd = _NONE;
				break;

	case _OCW5:	if (cnt == 0)
					n80device.cursor._n80_cur_newx = dat & 0x7f;
				else {
					n80device.cursor._n80_cur_newy = dat & 0x3f;
					n80device.crtc._prev_cmd = _NONE;
				}
				n80device.cursor.move();
				break;
	}
}

static void _n80_width_mode(BYTE dat) {
	BYTE	ah;
	int		changed = 0;

	// 桁変更チェック
	ah = dat & 0x01;
	if (n80device.crtc._n80_width_mode != ah) {
		n80device.crtc._n80_width_mode = ah;
		ah <<= 1;
		ah += n80device.crtc._n80_line_mode;
		n80device.crtc._n80_scrn_mode = ah;
#ifdef _MSC_VER
		chg_bltfunc(ah);
#else
		n80device.chg_crt_mode(ah);
#endif
		changed = 1;
	}
	// カラーモード変更チェック
	ah = (dat & 0x02) >> 1;
	if (n80device.crtc._n80_color_mode != ah) {
		n80device.crtc._n80_color_mode = ah;
		n80device.attr.chg_converter(ah);
		changed = 1;
	}
	if (changed)
		n80device.attr.invalid();
}

static void _n80_reset_crtc() {
	n80device.crtc._attr_reverse     = 0;
	n80device.crtc._n80_color_mode   = 1;
	n80device.crtc._n80_line_mode    = 1;
	n80device.crtc._n80_text_vis	 = 0;	//visible
}

export void _n80_init_crtc() {
	n80device.crtc.reset = _n80_reset_crtc;
	n80device.crtc.cmd = _n80_crtc_cmd;
	n80device.crtc.data = _n80_crtc_data;
	n80device.crtc.mode = _n80_width_mode;
	_n80_reset_crtc();
}

#ifdef __cplusplus
}	// extern "C"
#endif
