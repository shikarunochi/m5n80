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
#else
#include "n80types.h"
#endif
#include <stdio.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
static void _n80_cmt_command(BYTE cmd) {
	if (cmd == 0x40)
		n80device.cmt._reset = 1;
}

static BYTE _n80_cmt_read() {
	if (n80device.cmt._reset == 1) {
		n80device.cmt._reset = 0;
		return 0xff;
	} else {
		if (n80device.cmt._buff == NULL)
			return 0xff;
		if (n80device.cmt._index >= n80device.cmt._size)
			n80device.cmt._index = 0;
		return n80device.cmt._buff[n80device.cmt._index++];
	}
}

static void _n80_cmt_write(BYTE d) {
	n80_cmt_write_exec(d);
	//FILE *fp = fopen(n80device.save_cmt_path, "ab");
	//if (fp != NULL) {
	//	fputc(d, fp);
	//	fclose(fp);
	//}
}
export void _n80_init_cmt() {
	n80device.cmt.cmd = _n80_cmt_command;
	n80device.cmt.read = _n80_cmt_read;
	n80device.cmt.write = _n80_cmt_write;
}

#ifdef __cplusplus
}	// extern "C"
#endif
