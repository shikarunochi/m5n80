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
#include <time.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
static __inline void _lsb_set()
{
	BYTE al, ah;
	al = n80device.io._i40;
	al &= 0xef;
	ah = n80device.calender._sec;
	if (ah & 0x01)
		al |= 0x10;
	n80device.io._i40 = al;
}

static __inline BYTE hex2bcd(USHORT t)
{
	USHORT tt = t % 100;
	USHORT h = tt / 10;
	USHORT l = tt % 10;
	h <<= 4;
	h += l;
	return (BYTE) (h & 0xff);
}

static __inline void _set_sreg()
{	// set current date/time to uPD1990 40bits Shift Register
	#ifdef _MSC_VER
	SYSTEMTIME	st;
	GetLocalTime(&st);
	n80device.calender._sec   = hex2bcd(st.wSecond);
	n80device.calender._min   = hex2bcd(st.wMinute);
	n80device.calender._hour  = hex2bcd(st.wHour);
	n80device.calender._day   = hex2bcd(st.wDay);
	n80device.calender._month = (BYTE) ((st.wMonth << 4) & 0xff);
	#else
	time_t	t;
	struct tm *lt;
	time(&t);
	lt = localtime(&t);
	n80device.calender._sec   = hex2bcd(lt->tm_sec);
	n80device.calender._min   = hex2bcd(lt->tm_min);
	n80device.calender._hour  = hex2bcd(lt->tm_hour);
	n80device.calender._day   = hex2bcd(lt->tm_mday);
	n80device.calender._month = (BYTE) (((lt->tm_mon+1) << 4) & 0xff);
	#endif
}

export void __cdecl _n80_time_mode()
{
	BYTE ah, al, bl;
	ah = n80device.io._o40;
	// calender strobe signal check
	bl = ah & 0x02;
	al = n80device.calender._cstrb & 0x02;
	al ^= bl;
	// strobe edge detect
	if (al != 0) {
		n80device.calender._cstrb = bl;
		// strobe edge type 1 -> 0
		if (bl == 0) {
			// uPC1990 command
			bl = n80device.io._o10 & 0x07;
			switch (bl) {
			case 0x00:	// cmd 0: Register Hold Command
						n80device.calender._shiften = 0; break;
			case 0x01:	// cmd 1: Register Shift Command
						n80device.calender._shiften = 1; break;
			case 0x02:	// cmd 2: Time Set Command
						n80device.calender._shiften = 0; _lsb_set(); break;
			case 0x03:	// cmd 3: Time Read Command
						n80device.calender._shiften = 0; _set_sreg(); _lsb_set(); break;
			case 0x04:	// cmd 4: TP=64Hz Command
			case 0x05:	// cmd 5: TP=256Hz Command
			case 0x06:	// cmd 6: TP=2048Hz Command
			case 0x07:	// cmd 7: Test Command
			default:	break;
			}
		}
	}
	// CLK strobe check
	ah = n80device.io._o40 & 0x04;
	if (n80device.calender._shiften == 0) {
		n80device.calender._cclk = ah;
		return;
	}
	al = n80device.calender._cclk & 0x04;
	al ^= ah;
	if (al == 0)
		return;
	// detect CLK strobe
	n80device.calender._cclk = ah;
	// CLK strobe edge type 1 -> 0
	if (ah == 0) {
		// 40bit register shift 
		USHORT r8 = n80device.io._o10;
		DWORD  r32 = n80device.calender._day;
		r32 <<= 8;
		r32 += n80device.calender._hour;
		r32 <<= 8;
		r32 += n80device.calender._min;
		r32 <<= 8;
		r32 += n80device.calender._sec;
		r32 >>= 1;
		if (n80device.calender._month & 0x01)
			r32 |= 0x80000000;
		n80device.calender._sec  = r32 & 0xff;
		r32 >>= 8;
		n80device.calender._min  = r32 & 0xff;
		r32 >>= 8;
		n80device.calender._hour = r32 & 0xff;
		r32 >>= 8;
		n80device.calender._day  = r32 & 0xff;

		r8 <<= 5;
		r8 &= 0x0100;
		r8 |= n80device.calender._month;
		r8 >>= 1;
		n80device.calender._month = r8 & 0xff;

		_lsb_set();
	}
}

static void _n80_reset_calender() {
	n80device.calender._cstrb = 0;
	n80device.calender._cclk = 0;
	n80device.calender._shiften = 0;
	n80device.io._i40 = 0x0f;
	_set_sreg();
}

export void __cdecl _n80_init_calender() {
	n80device.calender.mode = _n80_time_mode;
	n80device.calender.reset = _n80_reset_calender;
	_n80_reset_calender();
}
	
#ifdef __cplusplus
}	// extern "C"
#endif
