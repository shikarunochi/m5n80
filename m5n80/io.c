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
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
export BYTE __cdecl _n80_ioread(USHORT port)
{
	switch ((port & 0xf0)) {
	case 0x00:	if ((port & 0x0f) < 10)
					return n80device.keys[(port & 0x0f)];
				else
					return 0xff;

	case 0x20:	if (port & 0x01)
					return 0x07;
				else
					return n80device.cmt.read();

	case 0x40:	return n80device.vrtc.poll(n80device.io._i40);

	default:	return 0xff;
	}
}

export void __cdecl _n80_iowrite(USHORT port, BYTE v)
{
	BYTE portno = port & 0x0f;
	switch ((port & 0xf0)) {
	case 0x00:
			n80device.hal.mode(portno, v);
			break;

	case 0x10:
			n80device.io._o10 = v;
			break;

	case 0x20:
			if ((portno & 0x01) == 0) {
				n80device.io._o20 = v;
				n80device.cmt.write(v);
			} else {
				n80device.io._o21 = v;
				n80device.cmt.cmd(v);
			}
			break;

	case 0x30:
			n80device.crtc.mode(v);
			break;

	case 0x40:
			n80device.io._o40 = v;
			n80device.calender.mode();
			//if (v & _PO40_BEEP)
			//	n80device.beep_on();
			//else
			//	n80device.beep_off();
			break;

	case 0x50:
			if (port & 0x01)
				n80device.crtc.cmd(v);
			else
				n80device.crtc.data(v);
			break;
			
	case 0x60:
			if (portno == 4) {
				n80device.io._o64 = v;
				n80device.dmac.ch2adr();
			} else if (portno == 8) {
				n80device.io._o68 = v;
				n80device.dmac.cmd();
			}
			break;
	}
}
	
#ifdef __cplusplus
}	// extern "C"
#endif
