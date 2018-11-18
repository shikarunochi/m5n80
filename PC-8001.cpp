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
//#include <memory.h>
#include <math.h>
#include <M5Stack.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define export
extern void _n80_init_attr();						// ATTRIB.c
extern void _n80_init_calender();					// PC1990.c
extern void _n80_init_crtc();						// CRTC.c
extern void _n80_init_dmac();						// DMAC.c
extern void _n80_init_vrtc();						// VRTC.c
extern void _n80_init_cursor();						// CURSOR.c
extern void _n80_init_cmt();						// CMT.c
extern void _n80_pcg_mode(BYTE portno, BYTE data);	// PCG.c
#ifndef _MSC_VER
N80DEVICE	n80device;
#endif
/**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
static void clear_io(LPN80DEVICE dev) {
	memset(&dev->io, 0, sizeof(N80IOPORT));
}

void clear_ram(LPN80DEVICE dev)
{
	BYTE	*mem = dev->mem.map._ram;

	int  i;

  for (i = 0; i < 0x1000; ++ i) {
		*(mem++) = 0x00;
		*(mem++) = 0xff;
	}
	for (i = 0; i < 0x1000; ++ i) {
		*mem++ = 0xff;
		*mem++ = 0x00;
	}
	for (i = 0; i < 128; ++ i) {
		int  j;

		for (j = 0; j < 64; ++j)
		    *mem++ = 0xff;
		for (j = 0; j < 64; ++j)
		    *mem++ = 0x00;
	}
}

static void clear_keys(LPN80DEVICE dev)
{
	int i;
	for (i = 0; i < 10; ++ i)
		dev->keys[i] = 0xff;
}

static void calc_clk_sync(LPN80DEVICE dev, DWORD hz, DWORD interval) {
	dev->param.hz = hz;
	dev->param.sync_interval = interval;

	dev->param.sync_ticks = (UINT) floor(((double) dev->param.hz) * dev->param.sync_interval / 1000.0 + 0.5);
}

static void _n80_device_reset(LPN80DEVICE dev) {
	dev->clear_io(dev);
	dev->clear_ram(&n80device);
	dev->clear_keys(dev);
	
	dev->attr.invalid();
	dev->calender.reset();
	dev->crtc.reset();
	dev->cursor.reset();
	dev->vrtc.reset();
	dev->dmac.reset();
	//dev->cmt.reset();
}
	
#ifdef _MSC_VER
export void _n80_device_initialize(LPN80DEVICE dev, SYNCFNC enter, SYNCFNC leave)
#else
export void _n80_device_initialize(LPN80DEVICE dev, SYNCFNC enter, SYNCFNC leave, CMODFNC chgmod)
#endif
{
  Serial.println("Devide Initialize:Start");
	// clear parameters
	memset(dev, 0, sizeof(N80DEVICE));

  //メモリ確保
  dev->font = (BYTE*)ps_malloc(4096 + 2048);
  dev->pcg = dev->font + 4096;
  
  dev->mem.map._rom = (BYTE*)ps_malloc(65536);
  dev->mem.map._4th = dev->mem.map._rom + 24576;
  dev->mem.map._ram = dev->mem.map._4th + 8192;
  dev->mem.arr = dev->mem.map._rom;
  
  // set methods
	dev->reset				= _n80_device_reset;
	dev->clear_io			= clear_io;
	dev->clear_ram			= clear_ram;
	dev->clear_keys			= clear_keys;
	dev->calc_clk_sync		= calc_clk_sync;
	dev->attr.init			= _n80_init_attr;		// ATTRIB.c
	dev->calender.init		= _n80_init_calender;	// PC1990.c
	dev->crtc.init			= _n80_init_crtc;		// CRTC.c
	dev->cursor.init		= _n80_init_cursor;		// CURSOR.c
	dev->vrtc.init			= _n80_init_vrtc;		// VRTC.c
	dev->dmac.init			= _n80_init_dmac;		// DMAC.c
	dev->cmt.init			= _n80_init_cmt;		// CMT.c
	dev->hal.mode			= _n80_pcg_mode;		// PCG.c
	dev->vram_sync_enter	= enter;				// critical section API
	dev->vram_sync_leave	= leave;
#ifndef _MSC_VER
	dev->chg_crt_mode		= chgmod;
#endif
	
	// call initial methods
	dev->clear_io(&n80device);
	dev->clear_ram(&n80device);
	dev->clear_keys(&n80device);
	dev->attr.init();
	dev->calender.init();
	dev->crtc.init();
	dev->cursor.init();
	dev->vrtc.init();
	dev->dmac.init();
	dev->cmt.init();
//	dev->attr.chg_converter(1);						// attribute convert mode init (mono)
	dev->calc_clk_sync(&n80device, 3993600, IVLMSEC);	// CLK = 4MHz, sync interval = 20ms

	// set default params
	//dev->param.flips = 60;				// flip rate = 60fps
  dev->param.flips = 20;        // flip rate = 20fps
}

#ifdef __cplusplus
}	// extern "C"
#endif
