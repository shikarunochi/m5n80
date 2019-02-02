/**--------------------------------------------------------------------------
 **
 **		Z80 Emulator Ver 3.00
 **		Copyright (C)1993-2010 H.Kanemoto
 **
 **		This is a free software. You may copy or distribute
 **		it without any permissions. But there are NO WARRANTY
 **		of use. Entire risk of use is with you.
 **
 **------------------------------------------------------------------------
**------------------------------------------------------------------------
**  For  M5Stack 2018/11/18 modified by @shikarunochi 
**------------------------------------------------------------------------*/

#include "z80.h"
#include "z80mac.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <memory.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#include <mmsystem.h>
#include <windows.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#define export
/**--------------------------------------------------------------------------
 **		Z80 Decoder
 **------------------------------------------------------------------------*/
extern void _z80_decoder(Z80CPU *cpu);
/**--------------------------------------------------------------------------
 **		Z80
 **------------------------------------------------------------------------*/
static int __cdecl __z80_step(LPZ80CPU cpu)
{
	cpu->r.x.T = 0;					// ステートクリア
	cpu->r.x.W = 0;					// メモリウェイトクリア
	cpu->r.x.head = cpu->r.x.PC;	// 現在のインストラクションポインタ退避
	++cpu->clk;						// R register用クロック

	if (cpu->signal & 0xc0) {
		// INT MODE 0: fetch data directry from data bus and PC not changed.
		// INT MODE 1/2: set PC only
		if (cpu->r.x.IM != 0) {
			__z80_pushw(cpu, cpu->r.x.PC);
			cpu->r.x.PC = cpu->r.x.IPC;
			cpu->r.x.head = cpu->r.x.IPC;
		}
	}
	_z80_decoder(cpu);				// １命令実行
	if (cpu->signal & 0xc0) {
		cpu->signal &= 0x3f;
		cpu->r.x.T += cpu->r.x.M;	// 割り込みマシンサイクル加算
		cpu->r.x.M = 0;
	}
	if (cpu->dbg_mode_on == 1) {
		//if (cpu->r.x.PC == 0x00DB)
		//	cpu->dbg_print_on = 1;
		//if (cpu->dbg_print_on == 1)
		//	fprintf(stdout, "%4.4X\t%s\n", cpu->r.x.head, cpu->dbg_mnemonic);
		fprintf(stdout, "%4.4X\t%s\r", cpu->r.x.head, cpu->dbg_mnemonic);
	}
#ifdef _MSC_VER
	InterlockedExchangeAdd(&cpu->Ts, cpu->r.x.T);
#endif
//#ifdef _ZAURUS_
	cpu->Ts += cpu->r.x.T;
//#endif

	if (cpu->signal & 0x07)			// HALT/未実装/無効命令検知/INT Data Bus
		return 1;
	else
		return 0;
}

static void __cdecl __z80_reset(LPZ80CPU cpu)
{
	clock_t	t;
	int		i;

	t = clock();					// R register乱数配列
	srand((int) t);
	for (i = 0; i < 256; ++i)
		cpu->rreg[i] = (BYTE) (rand() % 256);

	memset(&cpu->r, 0, sizeof(Z80REGS));
	cpu->signal = 0;
	cpu->req_mode0 = 0;
	cpu->int_dbus_size = 0;
	cpu->clk = 0;
}

static int __cdecl __z80_int_signal(LPZ80CPU cpu, int vector) {
	USHORT vec;
	if (cpu->r.x.IFF1 == 1) {
		cpu->r.x.IFF1 = 0;
		cpu->r.x.IFF2 = 0;
		cpu->signal |= _INTBIT;

		switch (cpu->r.x.IM) {
		case 0:	// MODE 0
			cpu->req_mode0 = 1;		// request fetch from interrup data bus
			cpu->int_dbus[0] = (BYTE) vector;
			cpu->int_dbus_size = 1;	// data bus buffer size
			cpu->r.x.M = 2;
			break;

		case 1: // MODE 1
			cpu->r.x.IPC = 0x0038;
			cpu->r.x.M = 13;
			break;

		case 2:	// MODE 2
			vec = (USHORT) cpu->r.x.I;
			vec <<= 8;
			vec += (vector * 2);
			cpu->r.x.M = 19;
			cpu->r.x.IPC = __z80_mwread(cpu, vec);
			break;

		default:	// unexpected interrupt mode
			cpu->signal = _Z80HALT_INT;
			return 1;
		}
		return 0;	// ACCEPTED
	}
	return 1;		// IGNORED
}
	
static void __cdecl __z80_nmi_signal(LPZ80CPU cpu) {
	cpu->r.x.IFF2 = cpu->r.x.IFF1;
	cpu->r.x.IFF1 = 0;
	cpu->r.x.M = 11;		// M1(5T)+M2(3T)+M3(3T)
	cpu->signal |= _NMIBIT;
	cpu->r.x.PC = 0x0066;
}

export void __z80_set_external_method(LPZ80CPU cpu, LPZ80FNCMBR mbr, LPZ80FNCMBW mbw, LPZ80FNCIOR ior, LPZ80FNCIOW iow)
{
	cpu->step = __z80_step;
	cpu->reset = __z80_reset;

	cpu->mbread = mbr;		// external Memory access function
	cpu->mbwrite = mbw;
	cpu->ioread = ior;		// external I/O access function
	cpu->iowrite = iow;
	
	cpu->int_signal = __z80_int_signal;
	cpu->nmi_signal = __z80_nmi_signal;

	__z80_reset(cpu);
}

#ifdef __cplusplus
}	// extern "C"
#endif
