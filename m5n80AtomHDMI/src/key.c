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


//キーコードの変換
//n80ソースを参考にしています
unsigned char hid_to_emu[] ={
0xff,//0	0x00	Reserved (no event indicated)
0xff,//1	0x01	Keyboard ErrorRollOver?
0xff,//2	0x02	Keyboard POSTFail
0xff,//3	0x03	Keyboard ErrorUndefined?
0x11,//4	0x04	Keyboard a and A
0x12,//5	0x05	Keyboard b and B
0x13,//6	0x06	Keyboard c and C
0x14,//7	0x07	Keyboard d and D
0x15,//8	0x08	Keyboard e and E
0x16,//9	0x09	Keyboard f and F
0x17,//10	0x0A	Keyboard g and G
0x18,//11	0x0B	Keyboard h and H
0x19,//12	0x0C	Keyboard i and I
0x1a,//13	0x0D	Keyboard j and J
0x1b,//14	0x0E	Keyboard k and K
0x1c,//15	0x0F	Keyboard l and L
0x1d,//16	0x10	Keyboard m and M
0x1e,//17	0x11	Keyboard n and N
0x1f,//18	0x12	Keyboard o and O
0x20,//19	0x13	Keyboard p and P
0x21,//20	0x14	Keyboard q and Q
0x22,//21	0x15	Keyboard r and R
0x23,//22	0x16	Keyboard s and S
0x24,//23	0x17	Keyboard t and T
0x25,//24	0x18	Keyboard u and U
0x26,//25	0x19	Keyboard v and V
0x27,//26	0x1A	Keyboard w and W
0x28,//27	0x1B	Keyboard x and X
0x29,//28	0x1C	Keyboard y and Y
0x2A,//29	0x1D	Keyboard z and Z
0x31,//30	0x1E	Keyboard 1 and !
0x32,//31	0x1F	Keyboard 2 and @
0x33,//32	0x20	Keyboard 3 and #
0x34,//33	0x21	Keyboard 4 and $
0x35,//34	0x22	Keyboard 5 and %
0x36,//35	0x23	Keyboard 6 and ^
0x37,//36	0x24	Keyboard 7 and &
0x38,//37	0x25	Keyboard 8 and *
0x39,//38	0x26	Keyboard 9 and (
0x30,//39	0x27	Keyboard 0 and )
0x0f,//40	0x28	Keyboard Return (ENTER)
0x4f,//41	0x29	Keyboard ESCAPE
0x43,//42	0x2A	Keyboard DELETE (Backspace)
0x50,//43	0x2B	Keyboard Tab
0x4e,//44	0x2C	Keyboard Spacebar
0x2f,//45	0x2D	Keyboard - and (underscore)
0x2e,//46	0x2E	Keyboard = and +
0x10,//47	0x2F	Keyboard [ and {
0x2b,//48	0x30	Keyboard ] and }
0x2c,//49	0x31	Keyboard \ and ｜
0x2d,//50	0x32	Keyboard Non-US # and ~
0x3b,//51	0x33	Keyboard ; and :
0x3a,//52	0x34	Keyboard ' and "
0x2e,//53	0x35	Keyboard Grave Accent and Tilde
0x3c,//54	0x36	Keyboard, and <
0x3d,//55	0x37	Keyboard . and >
0x3e,//56	0x38	Keyboard / and ?
0x48,//57	0x39	Keyboard Caps Lock -> STOP KEY
0x49,//58	0x3A	Keyboard F1
0x4a,//59	0x3B	Keyboard F2
0x4b,//60	0x3C	Keyboard F3
0x4c,//61	0x3D	Keyboard F4
0x4d,//62	0x3E	Keyboard F5
0xff,//63	0x3F	Keyboard F6
0xff,//64	0x40	Keyboard F7
0xff,//65	0x41	Keyboard F8
0xff,//66	0x42	Keyboard F9
0xff,//67	0x43	Keyboard F10
0xff,//68	0x44	Keyboard F11
0xff,//69	0x45	Keyboard F12
0xff,//70	0x46	Keyboard PrintScreen
0xff,//71	0x47	Keyboard Scroll Lock
0xff,//72	0x48	Keyboard Pause
0x43,//73	0x49	Keyboard Insert
0x40,//74	0x4A	Keyboard Home
0xff,//75	0x4B	Keyboard PageUp
0xff,//76	0x4C	Keyboard Delete Forward
0xff,//77	0x4D	Keyboard End
0xff,//78	0x4E	Keyboard PageDown
0x42,//79	0x4F	Keyboard RightArrow
0x42,//80	0x50	Keyboard LeftArrow
0x41,//81	0x51	Keyboard DownArrow
0x41,//82	0x52	Keyboard UpArrow
0xff,//83	0x53	Keypad Num Lock and Clear
0xff,//84	0x54	Keypad /
0xff,//85	0x55	Keypad *
0xff,//86	0x56	Keypad -
0xff,//87	0x57	Keypad +
0xff,//88	0x58	Keypad ENTER
0x01,//89	0x59	Keypad 1 and End
0x02,//90	0x5A	Keypad 2 and Down Arrow
0x03,//91	0x5B	Keypad 3 and PageDn?
0x04,//92	0x5C	Keypad 4 and Left Arrow
0x05,//93	0x5D	Keypad 5
0x06,//94	0x5E	Keypad 6 and Right Arrow
0x07,//95	0x5F	Keypad 7 and Home
0x08,//96	0x60	Keypad 8 and Up Arrow
0x09,//97	0x61	Keypad 9 and PageUp?
0x00,//98	0x62	Keypad 0 and Insert
0x0e,//99	0x63	Keypad . and Delete
0x70,//100	0x64	Keyboard Non-US \ and ｜
0xff,//101	0x65	Keyboard Application
0xff,//102	0x66	Keyboard Power
0xff,//103	0x67	Keypad =
0xff,//104	0x68	Keyboard F13
0xff,//105	0x69	Keyboard F14
0xff,//106	0x6A	Keyboard F15
0xff,//107	0x6B	Keyboard F16
0xff,//108	0x6C	Keyboard F17
0xff,//109	0x6D	Keyboard F18
0xff,//110	0x6E	Keyboard F19
0xff,//111	0x6F	Keyboard F20
0xff,//112	0x70	Keyboard F21
0xff,//113	0x71	Keyboard F22
0xff,//114	0x72	Keyboard F23
0xff,//115	0x73	Keyboard F24
0xff,//116	0x74	Keyboard Execute
0xff,//117	0x75	Keyboard Help
0xff,//118	0x76	Keyboard Menu
0xff,//119	0x77	Keyboard Select
0xff,//120	0x78	Keyboard Stop
0xff,//121	0x79	Keyboard Again
0xff,//122	0x7A	Keyboard Undo
0xff,//123	0x7B	Keyboard Cut
0xff,//124	0x7C	Keyboard Copy
0xff,//125	0x7D	Keyboard Paste
0xff,//126	0x7E	Keyboard Find
0xff,//127	0x7F	Keyboard Mute
0xff,//128	0x80	Keyboard Volume Up
0xff,//129	0x81	Keyboard Volume Down
0xff,//130	0x82	Keyboard Locking Caps Lock
0xff,//131	0x83	Keyboard Locking Num Lock
0xff,//132	0x84	Keyboard Locking Scroll Lock
0xff,//133	0x85	Keypad Comma
0xff,//134	0x86	Keypad Equal Sign
0x45,//135	0x87	Keyboard International1 // JIS[_/] → かなキーにしてます
0xff,//136	0x88	Keyboard International2
0x2c,//137	0x89	Keyboard International3
0xff,//138	0x8A	Keyboard International4
0xff,//139	0x8B	Keyboard International5
0xff,//140	0x8C	Keyboard International6
0xff,//141	0x8D	Keyboard International7
0xff,//142	0x8E	Keyboard International8
0xff,//143	0x8F	Keyboard International9
0xff,//144	0x90	Keyboard LANG1
0xff,//145	0x91	Keyboard LANG2
0xff,//146	0x92	Keyboard LANG3
0xff,//147	0x93	Keyboard LANG4
0xff,//148	0x94	Keyboard LANG5
0xff,//149	0x95	Keyboard LANG6
0xff,//150	0x96	Keyboard LANG7
0xff,//151	0x97	Keyboard LANG8
0xff,//152	0x98	Keyboard LANG9
0xff,//153	0x99	Keyboard Alternate Erase
0xff,//154	0x9A	Keyboard SysReq?/Attention
0xff,//155	0x9B	Keyboard Cancel
0xff,//156	0x9C	Keyboard Clear
0xff,//157	0x9D	Keyboard Prior
0xff,//158	0x9E	Keyboard Return
0xff,//159	0x9F	Keyboard Separator
0xff,//160	0xA0	Keyboard Out
0xff,//161	0xA1	Keyboard Oper
0xff,//162	0xA2	Keyboard Clear/Again
0xff,//163	0xA3	Keyboard CrSel/Props
0xff,//164	0xA4	Keyboard ExSel
};
//0x47,//224	0xE0	Keyboard LeftControl
//0x46,//225	0xE1	Keyboard LeftShift
//0x44,//226	0xE2	Keyboard LeftAlt
//0xff,//227	0xE3	Keyboard Left GUI
//0x47,//228	0xE4	Keyboard RightControl
//0x46,//229	0xE5	Keyboard RightShift
//0x44,//230	0xE6	Keyboard RightAlt
//0xff,//231	0xE7	Keyboard Right GUI
//0xff//232-65535	0xE8-0xFFFF	Reserved

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
  case KEY_TENKEY_9:  /* テンキー9 */ key = 0x09; break;
  case KEY_TENKEY_8:  /* テンキー8 */ key = 0x08; break;
  case KEY_TENKEY_7:  /* テンキー7 */ key = 0x07; break;
  case KEY_TENKEY_6:  /* テンキー6 */ key = 0x06; break;
  case KEY_TENKEY_5:  /* テンキー5 */ key = 0x05; break;
  case KEY_TENKEY_4:  /* テンキー4 */ key = 0x04; break;
  case KEY_TENKEY_3:  /* テンキー3 */ key = 0x03; break;
  case KEY_TENKEY_2:  /* テンキー2 */ key = 0x02; break;
  case KEY_TENKEY_1:  /* テンキー1 */ key = 0x01; break;
  case KEY_TENKEY_0:  /* テンキー0 */ key = 0x00; break;
  
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

export void onkey_down_HID(BYTE code)
{
	BYTE key = 0xff;
  if(code  < 166){
    key = hid_to_emu[code];
  }else{
    switch(code){
      case KEY_SHIFT:key = 0x46;break;
      case KEY_CTRL:key = 0x47;break;
      case KEY_GRAPH:key = 0x44;break;
    }
  }
	if (key != 0xff) {
		int  index = down_matorix[key][0];
		BYTE bit   = down_matorix[key][1];
		n80device.keys[index] &= bit;
	}
}

export void onkey_up_HID(BYTE code)
{
	BYTE key = 0xff;
  if(code  < 166){
    key = hid_to_emu[code];
  }else{
    switch(code){
      case KEY_SHIFT:key = 0x46;break;
      case KEY_CTRL:key = 0x47;break;
      case KEY_GRAPH:key = 0x44;break;
    }
  }
	if (key != 0xff) {
		int  index = up_matorix[key][0];
		BYTE bit   = up_matorix[key][1];
		n80device.keys[index] |= bit;
	}
}
