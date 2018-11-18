/**--------------------------------------------------------------------------
 **
 **		NEC PC-8001 Emulator "N80" for Zaurus(Spitz)
 **		--------------------------------------------
 **		Copyright (c) 1993-2010 H.Kanemoto
 **
 **		header file
 **
 **		Rev		Date		Descriptions
 **		-----	----------	-----------------------------------------------
 **		1.00	2010/12/08	Zaurus SL-C3000 Fedora Core 10 ARM porting
 **		2.00	2017/03/27	Raspberry Pi2 Raspbian(wheezy) ARM porting
 **------------------------------------------------------------------------
 ***------------------------------------------------------------------------
**  For  M5Stack 2018/11/18 modified by @shikarunochi 
**------------------------------------------------------------------------*/
#ifndef _N80_H_
#define _N80_H_
#ifdef __cplusplus
extern "C" {
#endif 
//#include <alsa/asoundlib.h>
#ifdef _ZAURUS_
#include <linux/fb.h>
#endif
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#ifdef _RASPBERRY_
#include <SDL2/SDL.h>
#endif
#include "n80types.h"
#include "z80.h"
#include <stdbool.h>
/**--------------------------------------------------------------------------
 **	declare structure types
 **------------------------------------------------------------------------*/
typedef struct N80LX_tag {

  LPDWORD            video;            // texture pixcels

  //DWORD           *dot_npattern;//[8][256][8];  // narrow(width80) dot patterns [color][bit pattern][horizon]
  //DWORD           *dot_wpattern;//[8][256][16]; // wide(width40) dot patterns [color][bit pattern][horizon]
  
	#ifdef _RASPBERRY_
	DWORD						dot_npattern[8][256][8];	// narrow(width80) dot patterns [color][bit pattern][horizon]
	DWORD						dot_wpattern[8][256][16];	// wide(width40) dot patterns [color][bit pattern][horizon]
	SDL_Window*					window;						// SDL2 Window
	SDL_Renderer*				renderer;					// SDL2 Renderer
	SDL_Texture*				texture;					// SDL2 Texture
	LPDWORD						video;						// texture pixcels
	#endif
	#ifdef _ZAURUS_
	USHORT						dot_pattern[8][256][16];	// fb graphics pattern data (must be alocated in iwmmx align)
	int							fbfd;						// fb descriptor
	LPUSHORT					video;						// fb pointer to mapped memory
	struct fb_var_screeninfo	vinfo;						// fb variable info
	struct fb_fix_screeninfo	finfo;						// fb fixed info
	#endif
	Z80CPU						cpu;						// z80 emulator instance
	pthread_mutex_t				vram_sync;					// PC-8001 vram sync
	pthread_mutex_t				display_sync;				// display sync
	pthread_mutex_t				sound_sync;					// sound sync
	pthread_mutex_t				pause_sync;					// pause sync
	volatile DWORD 				request_cpu_signal;			// signal for z80 thread
	volatile DWORD 				request_alsa_signal;		// signal for alsa thread
	volatile DWORD 				request_disp_signal;		// signal for display thread
	//#if defined(_ZAURUS_) || defined(_RASPBERRY_)
	int							fn_on;						// zaurus [Fn] key press status
	//#endif
//	snd_pcm_t					*handle;					// alsa handle
//	snd_pcm_hw_params_t			*params;					// alsa parameters
//	int							sampling_rate;				// alsa pcm sampling rate
//	int							period_frames;				// alsa period length in samples
//	int							period_bytes;				// alsa period length in byte
//	LPUSHORT					period_buffer;				// alsa local buffer
//	int							period_usec;				// alsa period long in micor sec
//	snd_pcm_sframes_t			ring_buffer_frames;			// alsa ring buffer length in samples
//	double						frame_keisu;				// 44.1khz(PCM) / 4MHz(CPU)
//	int							phase_adjuster;				// wave phase adjust (Beep)
//	int							phase_adjuster2;			// wave phase adjust (PCG Sound)
}	N80LX, *LPN80LX;

enum N80COLOR {N80BLACK = 0, N80BLUE, N80RED, N80PERPLE, N80GREEN, N80CYAN, N80YELLOW, N80WHITE};
/**--------------------------------------------------------------------------
 **	inline functions
 **------------------------------------------------------------------------*/
/*#if defined(_ARMv6_) || defined(__ARM_ARCH_6__)*/
//static inline DWORD interlocked_exchange(DWORD volatile *target, DWORD new_value)
//{
//	ULONG	xcg_value;
//	
//	__asm__ __volatile__ (
//	"		ldr	r0,%1			\n\t"
//	"0:		ldrex	r2,[r0]		\n\t"
//	"		mov	r3,r2			\n\t"
//	"		mov	r2,%2			\n\t"
//	"		strex	r1,r2,[r0]	\n\t"
//	"		cmp	r1,#0			\n\t"
//	"		bne	0b				\n\t"
//	"		str	r3,%0			\n\t"
//	: "=m"(xcg_value)
//	: "m"(target),"r"(new_value)
//	: "r0","r1","r2","r3"
//	);
//	return xcg_value;
//}

//static inline DWORD interlocked_exchangeadd(DWORD volatile *target, DWORD add_value)
//{
//	ULONG	xcg_value;
//	
//	__asm__ __volatile__ (
//	"		ldr	r0,%1			\n\t"
//	"0:		ldrex	r2,[r0]		\n\t"
//	"		mov	r3,r2			\n\t"
//	"		add	r2,r2,%2		\n\t"
//	"		strex	r1,r2,[r0]	\n\t"
//	"		cmp	r1,#0			\n\t"
//	"		bne	0b				\n\t"
//	"		str	r3,%0			\n\t"
//	: "=m"(xcg_value)
//	: "m"(target),"r"(add_value)
//	: "r0","r1","r2","r3"
//	);
//	return xcg_value;
//}

//static inline DWORD interlocked_increment(DWORD volatile *target)
//{
//	ULONG	inc_value;
//	
//	__asm__ __volatile__ (
//	"		ldr	r0,%1			\n\t"
//	"0:		ldrex	r2,[r0]		\n\t"
//	"		add	r2,r2,#1		\n\t"
//	"		strex	r1,r2,[r0]	\n\t"
//	"		cmp	r1,#0			\n\t"
//	"		bne	0b				\n\t"
//	"		str	r2,%0			\n\t"
//	: "=m"(inc_value)
//	: "m"(target)
//	: "r0","r1","r2"
//	);
//	return inc_value;
//}

//static inline DWORD interlocked_decrement(DWORD volatile *target)
//{
//	ULONG	dec_value;
//	
//	__asm__ __volatile__ (
//	"		ldr	r0,%1			\n\t"
//	"0:		ldrex	r2,[r0]		\n\t"
//	"		sub	r2,r2,#1		\n\t"
//	"		strex	r1,r2,[r0]	\n\t"
//	"		cmp	r1,#0			\n\t"
//	"		bne	0b				\n\t"
//	"		str	r2,%0			\n\t"
//	: "=m"(dec_value)
//	: "m"(target)
//	: "r0","r1","r2"
//	);
//	return dec_value;
//}
/*
#else
static inline DWORD interlocked_exchange(DWORD volatile *target, DWORD new_value)
{
	ULONG	xcg_value;
	
	__asm__ __volatile__ (
	"		ldr	r0,%1			\n\t"
	"		swp	r1,%2,[r0]		\n\t"
	"		str	r1,%0			\n\t"
	: "=m"(xcg_value)
	: "m"(target),"r"(new_value)
	: "r0","r1"
	);
	return xcg_value;
}

static inline void spin_lock(DWORD volatile *target)
{
	__asm__ __volatile__ (
	"		ldr	r0,%0			\n\t"
	"		mov	r2,#1			\n\t"
	"0:		swp	r1,r2,[r0]		\n\t"
	"		cmp	r1,#0			\n\t"
	"		bne	0b				\n\t"
	:
	: "m"(target)
	: "r0","r1","r2"
	);
}

static inline void spin_unlock(DWORD volatile *target)
{
	__asm__ __volatile__ (
	"		ldr	r0,%0			\n\t"
	"		eor	r1,r1,r1		\n\t"
	"		swp	r1,r1,[r0]		\n\t"
	:
	: "m"(target)
	: "r0","r1"
	);
}
#endif
*/
static inline double getclock()
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return tv.tv_sec + (double) tv.tv_usec*1e-6;
}

int  m5n80_main();
void initDisplay();
void renderDisplay();
void updateStatusArea(const char* message);

typedef struct
{
  char n80File[50 + 1];
  char cmtFile[50 + 1];
  char ssid[50 + 1];
  char pass[50 + 1];
  bool forceAccessPoint;
} PC80_CONFIG;

extern PC80_CONFIG pc80Config;

void loadConfig();
void saveConfig();

#define ROM_DIRECTORY "/PC80ROM"
#define CMT_DIRECTORY "/PC80ROM/CMT"
#define N80_DIRECTORY "/PC80ROM/N80"

//特別扱いするキー
#define KEY_SHIFT 0xFF
#define KEY_CTRL 0xFC
#define KEY_GRAPH 0xFB
#define KEY_TENKEY_8  0xF8
#define KEY_TENKEY_4  0xF4
#define KEY_TENKEY_6  0xF6
#define KEY_TENKEY_2  0xF2


#ifdef __cplusplus
}
#endif 
#endif
