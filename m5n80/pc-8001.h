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

#ifndef _PC_8001_H_
#define _PC_8001_H_
#include "z80.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		定数定義
 **------------------------------------------------------------------------*/
#define _PO40_BEEP	(0x20)		// Beep bit
#define _PO40_CLDS	(0x04)		// Calender data bit
#define _PO40_CLK	(0x02)		// Calender clock
#define _PO40_CSTB	(0x01)		// Calender strobe
#define IVLSEC		(0.01)		// CPU loop interval in sec
#define IVLMSEC		(10)		// CPU loop interval in msec
/**--------------------------------------------------------------------------
 **		構造体
 **------------------------------------------------------------------------*/
typedef struct N80MEMORY_tag {	// Memory map
	BYTE	*_rom;//[24576];		// ROM
	BYTE	*_4th;//[8192];			// 4th ROM
	BYTE	*_ram;//[32768];		// RAM
}	N80MEM, *LPN80MEM;

typedef union N80MEMMAP_tag {
	N80MEM	map;
	BYTE	*arr;//[65536];			// rom/ram flat array
}	N80MEMMAP, *LPN80MEMMAP;

typedef struct N80IOPORT_tag {
	BYTE	_i40;
	BYTE	_i5C;
	BYTE	_i71;				// 4th ROM select (corrected)

	BYTE	_o10;				// PRINTER/CALENDER data
	BYTE	_o20;				// USART data
	BYTE	_o21;				// USART command
	BYTE	_o30;				// CONTROL #1
	BYTE	_o31;				// CONTROL #2
	BYTE	_o40;				// STROBE
	BYTE	_o50;				// CRTC parameter
	BYTE	_o51;				// CRTC command
	BYTE	_o52;				// boarder/background color
	BYTE	_o53;				// TEXT/GVRAM show flag
	BYTE	_o54;				// color palette
	BYTE	_o70;				// TEXT WINDOW offset address
	BYTE	_o71;				// 4th ROM select (raw)
	BYTE	_o64;				// DMA ch.2 address register
	BYTE	_o68;				// DMA command register
	BYTE	_oE4;				// interrupt leveled mask
	BYTE	_oE6;				// interrupt enable/disable mask
}	N80IOPORT, *LPN80IOPORT;

typedef struct N80CMT_tag {
	LPBYTE	_buff;				// cmt data buff ptr
	DWORD	_size;				// buff size
	DWORD	_index;				// buff current offset ptr
	BYTE	_reset;				// reset
	void	(*init)();
	BYTE	(*read)();
	void	(*write)(BYTE);
	void	(*cmd)(BYTE);
}	N80CMT, *LPN80CMT;

typedef struct N80PD1990_tag {	// uPD1990 calender
	BYTE	_sec;				// uPD1990 40bits shift register
	BYTE	_min;
	BYTE	_hour;
	BYTE	_day;
	BYTE	_month;
	BYTE	_cstrb;				// ｶﾚﾝﾀﾞｺﾏﾝﾄﾞｽﾄﾛｰﾌﾞ  2:ON,0:OFF
	BYTE	_cclk;				// ｶﾚﾝﾀﾞｼﾌﾄｸﾛｯｸ      4:ON,0:OFF
	BYTE	_shiften;			// shift enable flag 1:EN,0:DIS
	void	(*init)();
	void	(*reset)();
	void	(*mode)();
}	N80PD1990, *LPN80PD1990;

typedef struct N80CRTC_tag {
	DWORD	_n80_scrn_mode;		// 0:40/20, 1:40/25, 2:80/20, 3:80/25
	BYTE	_n80_color_mode;	// color mode        0:color, 1:mono
	BYTE	_n80_width_mode;	// 80/40 colmun mode 0:40, 1:80
	BYTE	_n80_line_mode;		// 20/25 line mode   0:20, 1:25
	BYTE	_attr_reverse;		// attribute color reverse mode flag
	USHORT	_param_count;		// amount of params already accepted.
	BYTE	_prev_cmd;			// current command which is waiting for params
	BYTE	_n80_text_vis;		// text visible: 0:visible, not 0:invisible b0:TXTDS/b1:crtc stop/b2:dmac stop
	void	(*init)();
	void	(*reset)();
	void	(*cmd)(BYTE);		// uPD1330 command
	void	(*data)(BYTE);		// uPD1330 data
	void	(*mode)(BYTE);		// width mode change
}	N80CRTC, *LPN80CRTC;

typedef struct N80CURSOR_tag {
	BYTE	_n80_cur_col;		// cursor location x
	BYTE	_n80_cur_row;		// cursor location y
	BYTE	_n80_cur_newx;		// cursor new location x
	BYTE	_n80_cur_newy;		// cursor new location y
	BYTE	_n80_cur_enable;	// cursor enable
	BYTE	_n80_cur_blink;		// cursor blink status
	void	(*init)();
	void	(*reset)();
	void	(*enable)();
	void	(*disable)();
	void	(*move)();
}	N80CURSOR, *LPN80CURSOR;

typedef struct N80VRTC_tag {
	BYTE	_vrtc_flag;			// vertical blanking flag
	BYTE	_vrtc_blink;		// blanking interval
	int		vsync_line;			// current line
	DWORD	vsync_state;		// v-freq 16.02ms=64080T(disp:12.52ms=50080T/blank:3.504ms=14016T)
	DWORD	hsync_state;		// h-freq 62.58us=250T(disp:44.70us=179T/blank:17.88us=71T)
	void	(*init)();			// initialize
	void	(*reset)();			// reset
	int		(*blanking)(DWORD);	// blanking operation
	void	(*edge)();			// notify entering vertical blank
	BYTE	(*poll)(BYTE);		// poll vertical blank
}	N80VRTC, *LPN80VRTC;

typedef struct N80DMAC_tag {
	DWORD	_penarty;			// DMA transfer penarty
	DWORD	_begin;				// DMA begin addr
	DWORD	_end;				// DMA end addr
	BYTE	_dmaact;			// DMA active flag
	BYTE	_ch2_low;			// ch.2 bagin addr low byte
	BYTE	_ch2_hi;			// ch.2 begin addr hi byte
	BYTE	_ch2_wait_hl;		// 0:expect low, 1:expect high
	void	(*init)();			// initialize
	void	(*reset)();			// reset
	void	(*cmd)();			// DMA command
	void	(*ch2adr)();		// ch.2 param
}	N80DMAC, *LPN80DMAC;

typedef struct N80ATTR_tag {
	BYTE	_eval_attr[80];		// evaluated attribute
	BYTE	_textcach[80];		// text cach
	BYTE	_attrcach[40];		// attr cach
	BYTE	_attrchg[25];		// last changed attr
	BYTE	_sortstk[22];		// attr area sort stack
	BYTE	_headzero;			// area start zero flag
	BYTE	_nextcolor[25];		// attr effective color
	BYTE	_nextfunc[25];		// attr effective func
	BYTE	_prevcolor;			// previous color
	BYTE	_prevfunc;			// previous func
	void	(*init)();
	void	(*reset)();
	int		(*eval)(int);		// eval attribute
	void	(*cache)(int);		// copy vram to cache array
	void	(*chg_converter)(int);	// change attribute converter function
	BYTE	(*_convfunc)(BYTE);	// pointer to attribute converter function
	void	(*invalid)();		// set all attrchg invalid
	void	(*set)(int, BYTE);	// set attrchg of line
}	N80ATTR, *LPN80ATTR;

typedef struct N80PCG_tag {
	USHORT	_pcg_freq;			// PCG i8253 timmer #2 (adjust)
	BYTE	_timmr_hi;			// PCG i8253  low byte
	BYTE	_timmr_lo;			// PCG i8253  low byte
	BYTE	_timmr_mode;		// PCG i8253 mode register
	BYTE	_timmr_cont;
	BYTE	_pcg_lo;			// PCG RAM  low address
	BYTE	_pcg_hi;			// high address
	BYTE	_pcg_dat;			// PCG char data
	BYTE	_sound_is_on;		// sound playing
	void	(*mode)(BYTE, BYTE);
}	N80PCG, *LPN80PCG;

typedef struct N80EMUPARAM_tag {
	DWORD		hz;				// System Clock 4MHz
	DWORD		sync_interval;	// speed sync interval in msec
	DWORD		sync_ticks;		// speed sync interval in Z80 state clocks
	DWORD		flips;			// flips per second 60fps
}	N80EMUPARAM, *LPN80EMUPARAM;

typedef struct N80DEVICE_tag {
	N80MEMMAP	mem;			// PC-8001 memory
	BYTE		*font;//[4096];		// font datas
	BYTE		*pcg;//[2048];		// pcg datas
	BYTE		keys[10];		// key on/off bit datas
	BYTE		pcg_mode;		// PCG ON/OFF mode
	N80IOPORT	io;				// i/o port cache
	N80PD1990	calender;		// uPD1990 Calender
	N80CMT		cmt;			// uPD8251 USART
	N80CRTC		crtc;			// uPD1330 CRTC
	N80CURSOR	cursor;			// uPD1330 Cursor
	N80VRTC		vrtc;			// uPD1330 VRTC
	N80DMAC		dmac;			// DMAC
	N80ATTR		attr;			// uPD1330 Attribute
	N80PCG		hal;			// HAL PCG-8100
	N80EMUPARAM	param;

	void		(*reset)(struct N80DEVICE_tag *);
	void		(*clear_io)(struct N80DEVICE_tag *);
	void		(*clear_ram)(struct N80DEVICE_tag *);
	void		(*clear_keys)(struct N80DEVICE_tag *);
	void		(*calc_clk_sync)(struct N80DEVICE_tag * dev, DWORD hz, DWORD interval);
	void		(*beep_on)();
	void		(*beep_off)();
	void		(*sound_on)(USHORT freq);
	void		(*sound_off)();
	void		(*vram_sync_enter)();
	void		(*vram_sync_leave)();
#ifndef _MSC_VER
	void		(*chg_crt_mode)(int);
#endif
	char		save_cmt_path[512];	// save cmt path
}	N80DEVICE, *LPN80DEVICE;

typedef void (*SYNCFNC)();
#ifndef _MSC_VER
typedef void (*CMODFNC)(int);
#endif
/**--------------------------------------------------------------------------
 **		グローバル変数
 **------------------------------------------------------------------------*/
extern N80DEVICE	n80device;

#ifdef __cplusplus
}	// extern "C"
#endif
#endif//_PC_8001_H_
