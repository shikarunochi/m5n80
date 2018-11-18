/**--------------------------------------------------------------------------
 **
 **		Z80 Emulator Ver 3.00
 **		Copyright (C)1993-2010 H.Kanemoto
 **
 **		This is a free software. You may copy or distribute
 **		it without any permissions. But there are NO WARRANTY
 **		of use. Entire risk of use is with you.
 **
 **------------------------------------------------------------------------*/
#ifndef _Z80_H_
#define _Z80_H_
#ifndef _MSC_VER
#define __cdecl
#define __inline inline
#include "n80types.h"
#else
#include <windows.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif
/**--------------------------------------------------------------------------
 **		Z80 Flag bits
 **------------------------------------------------------------------------*/
#define _CF	(0x01)
#define _NF	(0x02)
#define _PF	(0x04)
#define _VF	(0x04)
#define _XF	(0x08)
#define _HF	(0x10)
#define _YF	(0x20)
#define _ZF	(0x40)
#define _SF	(0x80)

#define	_Z80HALT_76	(1)		// HALT by HALT instruction
#define	_Z80HALT_IL	(2)		//      by illegal instruction
#define _Z80HALT_UI	(3)		//      by unimplemented instruction
#define _Z80HALT_BUS (4)	//		by interrupt data bus error
#define _Z80HALT_INT (5)	//		by unexpected interrupt mode error
#define _NMIBIT		(0x80)
#define _INTBIT		(0x40)
/**--------------------------------------------------------------------------
 **		Z80 レジスタインデックス
 **------------------------------------------------------------------------*/
enum Z80REGIDX {Z80_BC, Z80_DE, Z80_HL, Z80_AF,
				Z80_BC2,Z80_DE2,Z80_HL2,Z80_AF2,
				Z80_IX, Z80_IY, Z80_PC, Z80_SP, Z80_HEAD};
enum Z80REG8IDX {Z80_C = 0,Z80_B,Z80_E,Z80_D,Z80_L,Z80_H,Z80_F,Z80_A,
				 Z80_C2,Z80_B2,Z80_E2,Z80_D2,Z80_L2,Z80_H2,Z80_F2,Z80_A2,
				 Z80_IXL,Z80_IXH,Z80_IYL,Z80_IYH,
				 Z80_PCL,Z80_PCH,Z80_SPL,Z80_SPH};
/**--------------------------------------------------------------------------
 **		Z80 レジスタ構造体
 **------------------------------------------------------------------------*/
typedef struct  R8 {
	BYTE	C;			// BC register pare
	BYTE	B;
	BYTE	E;			// DE register pare
	BYTE	D;
	BYTE	L;			// HL register pare
	BYTE	H;
	BYTE	F;			// AF register pare
	BYTE	A;
	BYTE	C2;			// BC' register pare
	BYTE	B2;
	BYTE	E2;			// DE' register pare
	BYTE	D2;
	BYTE	L2;			// HL' register pare
	BYTE	H2;
	BYTE	F2;			// AF' register pare
	BYTE	A2;
	BYTE	IXL;		// IX register
	BYTE	IXH;
	BYTE	IYL;		// IY register
	BYTE	IYH;
	BYTE	PCL;		// PC register
	BYTE	PCH;
	BYTE	SPL;		// SP
	BYTE	SPH;
}	R8;

typedef struct R16 {
	USHORT	BC;			// general registers
	USHORT	DE;
	USHORT	HL;
	USHORT	AF;
	USHORT	altBC;		// alternate general registers
	USHORT	altDE;
	USHORT	altHL;
	USHORT	altAF;
	USHORT	IX;			// index register
	USHORT	IY;
	USHORT	PC;			// instruction pointer
	USHORT	SP;			// stack pointer
	DWORD	T;			// state in M1 mode
	DWORD	W;			// memory access wait
	DWORD	M;			// interrupt machine cycle
	BYTE	I;			// interrupt vector register
	BYTE	IM;			// interrupt mode 0..2
	BYTE	R;			// re-flesh register
	BYTE	IFF1;		// interrupt flip flop #1
	BYTE	IFF2;		//					   #2
	BYTE	HALT;		// decode "HALT" code flag
	USHORT	head;		// headder PC of current code stream
	USHORT	IPC;		// interrupt jump PC
}	R16;

typedef union  Z80REGS {	// registers of Z80 Micro Processor
	R8		h;
	R16		x;
	BYTE	br[24];
	USHORT	xr[12];
}	Z80REGS, *LPZ80REGS;

typedef struct Z80CPU_tag {
	Z80REGS			r;
	volatile BYTE	clk;		// z80 system clock
	volatile BYTE	signal;		// z80 control signals
	BYTE			rreg[256];	// random sheet for R register
	volatile BYTE	req_mode0;	// interrupt mode 0 fetch request
	BYTE			int_dbus[4];// interrupt mode 0 data bus buffer
	int16_t			int_dbus_size;
	BYTE			dbg_mode_on;
	BYTE			dbg_print_on;
	char			dbg_mnemonic[32];
#ifdef _MSC_VER
	volatile LONG	Ts;
#else
	#ifdef _ZAURUS_
	volatile DWORD	lock;		// pxa270 does not have ldrex/strex, so use swp
#else
	volatile DWORD	Ts;
	#endif
#endif

	int		(*step)(struct Z80CPU_tag *cpu);
	void	(*reset)(struct Z80CPU_tag *cpu);
	int		(*int_signal)(struct Z80CPU_tag *cpu, int vector);
	void	(*nmi_signal)(struct Z80CPU_tag *cpu);

	BYTE	(*mbread)(USHORT Adr, DWORD *W);
	void	(*mbwrite)(USHORT Adr, BYTE v, DWORD *W);
	BYTE	(*ioread)(USHORT port);
	void	(*iowrite)(USHORT port, BYTE v);
}	Z80CPU, *LPZ80CPU;
/**--------------------------------------------------------------------------
 **		Z80 外部関数ポインタ定義
 **------------------------------------------------------------------------*/
typedef BYTE	(__cdecl *LPZ80FNCMBR)(USHORT Adr, DWORD *W);
typedef void	(__cdecl *LPZ80FNCMBW)(USHORT Adr, BYTE b, DWORD *W);
typedef BYTE	(__cdecl *LPZ80FNCIOR)(USHORT port);
typedef void	(__cdecl *LPZ80FNCIOW)(USHORT port, BYTE b);

#ifdef __cplusplus
}	// extern "C"
#endif
#endif//_Z80_H_
