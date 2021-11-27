#ifndef _N80TYPES_H_
#define _N80TYPES_H_
#include <stdint.h>
/**--------------------------------------------------------------------------
 **	declare types
 **------------------------------------------------------------------------*/
typedef uint8_t			BYTE, *LPBYTE;
typedef uint16_t		USHORT, *LPUSHORT;
typedef const uint16_t	CUSHORT, *LPCUSHORT;
typedef uint32_t		DWORD, UINT,  ULONG, *LPDWORD;
typedef int32_t			LONG, *LPLONG;
typedef void			*LPVOID;
#ifndef NULL
#define NULL	((LPVOID) 0)
#endif
#endif
