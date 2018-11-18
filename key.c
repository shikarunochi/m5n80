/**--------------------------------------------------------------------------
 **
 **		NEC PC-8001 Emulator "N80" for Zaurus(Spitz)
 **		--------------------------------------------
 **		Copyright (c) 1993-2010 H.Kanemoto
 **
 **		keyboard
 **
 **		Rev		Date		Descriptions
 **		-----	----------	-----------------------------------------------
 **		1.00	2010/12/08	Zaurus SL-C3000 Fedora Core 10 ARM版 porting
 **		2.00	2017/03/27	Raspberry Pi2 Raspbian(wheezy) ARM版 porting
 **------------------------------------------------------------------------
**------------------------------------------------------------------------
**  For  M5Stack 2018/11/18 modified by @shikarunochi 
**------------------------------------------------------------------------*/

#include <stdio.h>
#include "n80.h"
#include "pc-8001.h"
#define	export
/**--------------------------------------------------------------------------
 **	外部変数／関数
 **------------------------------------------------------------------------*/
extern N80LX	n80lx;
/**--------------------------------------------------------------------------
 **	内部変数／関数
 **------------------------------------------------------------------------*/
static BYTE  down_matorix[80][2] =
{
	{0,0xfe}, {0,0xfd}, {0,0xfb}, {0,0xf7},
	{0,0xef}, {0,0xdf}, {0,0xbf}, {0,0x7f},
	{1,0xfe}, {1,0xfd}, {1,0xfb}, {1,0xf7},
	{1,0xef}, {1,0xdf}, {1,0xbf}, {1,0x7f},
	{2,0xfe}, {2,0xfd}, {2,0xfb}, {2,0xf7},
	{2,0xef}, {2,0xdf}, {2,0xbf}, {2,0x7f},
	{3,0xfe}, {3,0xfd}, {3,0xfb}, {3,0xf7},
	{3,0xef}, {3,0xdf}, {3,0xbf}, {3,0x7f},
	{4,0xfe}, {4,0xfd}, {4,0xfb}, {4,0xf7},
	{4,0xef}, {4,0xdf}, {4,0xbf}, {4,0x7f},
	{5,0xfe}, {5,0xfd}, {5,0xfb}, {5,0xf7},
	{5,0xef}, {5,0xdf}, {5,0xbf}, {5,0x7f},
	{6,0xfe}, {6,0xfd}, {6,0xfb}, {6,0xf7},
	{6,0xef}, {6,0xdf}, {6,0xbf}, {6,0x7f},
	{7,0xfe}, {7,0xfd}, {7,0xfb}, {7,0xf7},
	{7,0xef}, {7,0xdf}, {7,0xbf}, {7,0x7f},
	{8,0xfe}, {8,0xfd}, {8,0xfb}, {8,0xf7},
	{8,0xef}, {8,0xdf}, {8,0xbf}, {8,0x7f},
	{9,0xfe}, {9,0xfd}, {9,0xfb}, {9,0xf7},
	{9,0xef}, {9,0xdf}, {9,0xbf}, {9,0x7f}
};
static BYTE  up_matorix[80][2] =
{
	{0,0x01}, {0,0x02}, {0,0x04}, {0,0x08},
	{0,0x10}, {0,0x20}, {0,0x40}, {0,0x80},
	{1,0x01}, {1,0x02}, {1,0x04}, {1,0x08},
	{1,0x10}, {1,0x20}, {1,0x40}, {1,0x80},
	{2,0x01}, {2,0x02}, {2,0x04}, {2,0x08},
	{2,0x10}, {2,0x20}, {2,0x40}, {2,0x80},
	{3,0x01}, {3,0x02}, {3,0x04}, {3,0x08},
	{3,0x10}, {3,0x20}, {3,0x40}, {3,0x80},
	{4,0x01}, {4,0x02}, {4,0x04}, {4,0x08},
	{4,0x10}, {4,0x20}, {4,0x40}, {4,0x80},
	{5,0x01}, {5,0x02}, {5,0x04}, {5,0x08},
	{5,0x10}, {5,0x20}, {5,0x40}, {5,0x80},
	{6,0x01}, {6,0x02}, {6,0x04}, {6,0x08},
	{6,0x10}, {6,0x20}, {6,0x40}, {6,0x80},
	{7,0x01}, {7,0x02}, {7,0x04}, {7,0x08},
	{7,0x10}, {7,0x20}, {7,0x40}, {7,0x80},
	{8,0x01}, {8,0x02}, {8,0x04}, {8,0x08},
	{8,0x10}, {8,0x20}, {8,0x40}, {8,0x80},
	{9,0x01}, {9,0x02}, {9,0x04}, {9,0x08},
	{9,0x10}, {9,0x20}, {9,0x40}, {9,0x80}
};
/**--------------------------------------------------------------------------
 **	関数定義
 **------------------------------------------------------------------------*/
//シリアルコンソールからASCIIコード入力されたものを内部コードに変換
static inline BYTE convert_key(BYTE code)
{
  BYTE  key = 0xff;
  switch (code) {
  case 0x31:  /* 1 */ key = 0x31; break;
  case 0x32:  /* 2 */ key = 0x32; break;
  case 0x33:  /* 3 */ key = 0x33; break;
  case 0x34:  /* 4 */ key = 0x34; break;
  case 0x35:  /* 5 */ key = 0x35; break;
  case 0x36:  /* 6 */ key = 0x36; break;
  case 0x37:  /* 7 */ key = 0x37; break;
  case 0x38:  /* 8 */ key = 0x38; break;
  case 0x39:  /* 9 */ key = 0x39; break;
  case 0x30:  /* 0 */ key = 0x30; break;

  case 0x71:  /* Q */ key = 0x21; break;
  case 0x77:  /* W */ key = 0x27;  break;
  case 0x65:  /* E */ key = 0x15;  break;
  case 0x72:  /* R */ key = 0x22; break;
  case 0x74:  /* T */ key = 0x24;  break;
  case 0x79:  /* Y */ key = 0x29;  break;
  case 0x75:  /* U */ key = 0x25; break;
  case 0x69:  /* I */ key = 0x19; break;
  case 0x6F:  /* O */ key = 0x1F; break;
  case 0x70:  /* P */ key = 0x20; break;

  case 0x61:  /* A */ key = 0x11; break;
  case 0x73:  /* S */ key = 0x23; break;
  case 0x64:  /* D */ key = 0x14; break;
  case 0x66:  /* F */ key = 0x16; break;
  case 0x67:  /* G */ key = 0x17;  break;
  case 0x68:  /* H */ key = 0x18;  break;
  case 0x6A:  /* J */ key = 0x1A; break;
  case 0x6B:  /* K */ key = 0x1B;  break;
  case 0x6C:  /* L */ key = 0x1C; break;

  case 0x7A:  /* Z */ key = 0x2A; break;
  case 0x78:  /* X */ key = 0x28; break;
  case 0x63:  /* C */ key = 0x13; break;
  case 0x76:  /* V */ key = 0x26; break;
  case 0x62:  /* B */ key = 0x12; break;
  case 0x6E:  /* N */ key = 0x1E; break;
  case 0x6D:  /* M */ key = 0x1D; break;

  case 0x0D:  /* ENTER */ key = 0x0F; break;
  
  case 0x12:  /* ARW-U */ key = 0x41;  break;
  case 0x13:  /* ARW-R */ key = 0x42;  break;

  case 0x15:  /* HOME  */ key = 0x40; break;
  case 0x18:  /* INS   */ key = 0x43; break;
  case 0x17:  /* DEL   */ key = 0x43; break;
  case 0x1a:  /* PAUSE/BREAK   */ key = 0x48; break;

  case 0x20:  /* SP    */ key = 0x4E; break;
  case 0x09:  /* TAB   */ key = 0x50; break;
  
  case 0x2D:  /* = -   */ key = 0x2F; break;
  case 0x5E:  /* ^ ~   */ key = 0x2E; break;
  case 0x40:  /* @     */ key = 0x10; break;
  case 0x5B:  /* [     */ key = 0x2B; break;
  case 0x5D:  /* ]     */ key = 0x2D; break;
  case 0x3B:  /* + ;   */ key = 0x3B; break;
  case 0x3A:  /* * :   */ key = 0x3A; break;

  case 0x2C:  /* < ,   */ key = 0x3C; break;
  case 0x2E:  /* > .   */ key = 0x3D; break;
  case 0x2F:  /* / ?   */ key = 0x3E; break;

  case 0x7C:  /* \ |   */ key = 0x2C; break;

  //特別値：このプログラムだけで使っている値です。
  case KEY_SHIFT:  /* SHIFT */ key = 0x46; break;
  case KEY_CTRL:   /* CTRL */ key = 0x47; break;
  case KEY_GRAPH:   /* GRAPH */ key = 0x44; break;
  case KEY_TENKEY_8:  /* テンキー8 */ key = 0x08; break;
  case KEY_TENKEY_4:  /* テンキー4 */ key = 0x04; break;
  case KEY_TENKEY_6:  /* テンキー6 */ key = 0x06; break;
  case KEY_TENKEY_2:  /* テンキー2 */ key = 0x02; break;
  
  default:  break;
  }
  return key;
 
}

export void onkey_down(BYTE code)
{
	BYTE	key = convert_key(code);

	if (code == 0x3D) {		// [Fn]キー
		n80lx.fn_on = 1;
		return;
	}
	if (key != 0xff) {
		int  index = down_matorix[key][0];
		BYTE bit   = down_matorix[key][1];
		n80device.keys[index] &= bit;
	}
}

export void onkey_up(BYTE code)
{
	BYTE	key = convert_key(code);

	if (code == 0x3D) {		// [Fn]キー
		n80lx.fn_on = 0;
		return;
	}
	if (key != 0xff) {
		int  index = up_matorix[key][0];
		BYTE bit   = up_matorix[key][1];
		n80device.keys[index] |= bit;
	}
}
