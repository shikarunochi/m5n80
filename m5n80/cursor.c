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
static void _n80_cursor_enable() {
	n80device.cursor._n80_cur_enable = 1;
}

static void _n80_cursor_disable() {
	if (n80device.cursor._n80_cur_enable != 0) {
		n80device.cursor._n80_cur_enable = 0;
		n80device.attr.set(n80device.cursor._n80_cur_row, 1);
	}
}

static void _n80_move_cursor() {
	if (n80device.cursor._n80_cur_enable == 0) {
		n80device.cursor._n80_cur_col = n80device.cursor._n80_cur_newx;
		n80device.cursor._n80_cur_row = n80device.cursor._n80_cur_newy;
	} else {
		if (n80device.cursor._n80_cur_newy != n80device.cursor._n80_cur_row) {
			n80device.attr.set(n80device.cursor._n80_cur_newy, 1);
			n80device.attr.set(n80device.cursor._n80_cur_row, 1);
			n80device.cursor._n80_cur_col = n80device.cursor._n80_cur_newx;
			n80device.cursor._n80_cur_row = n80device.cursor._n80_cur_newy;
			n80device.cursor._n80_cur_blink = 1;
		} else if (n80device.cursor._n80_cur_newx != n80device.cursor._n80_cur_col) {
			n80device.attr.set(n80device.cursor._n80_cur_row, 1);
			n80device.cursor._n80_cur_col = n80device.cursor._n80_cur_newx;
			n80device.cursor._n80_cur_blink = 1;
		}
	}
}

static void _n80_reset_cursor() {
	n80device.cursor._n80_cur_enable = 1;
	n80device.cursor._n80_cur_blink  = 1;
}

export void _n80_init_cursor() {
	n80device.cursor.reset = _n80_reset_cursor;
	n80device.cursor.enable = _n80_cursor_enable;
	n80device.cursor.disable = _n80_cursor_disable;
	n80device.cursor.move = _n80_move_cursor;
	_n80_reset_cursor();
}
	
#ifdef __cplusplus
}	// extern "C"
#endif
