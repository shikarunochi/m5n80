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

//#include <memory.h>
#include "pc-8001.h"
#ifdef _MSC_VER
#include <windows.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#define export
static BYTE	GRB2RGB[8] = //  GRB to RGB order
	{0x00,0x01,0x04,0x05,0x02,0x03,0x06,0x07};
 /**--------------------------------------------------------------------------
 **		ASM Modules
 **------------------------------------------------------------------------*/
static void _n80_attr_cache(int line) {
	int		i, s, r;
	LPBYTE	sort = &n80device.attr._attrcach[0];
	LPBYTE	src  = &n80device.mem.arr[n80device.dmac._begin + (120*line)];
	// VRAM 80桁分をコピー
	for (i = 0; i < 80; ++i)
		n80device.attr._textcach[i] = *src++;

	// VRAM 属性40桁を桁昇順でコピーする
	for (i = 0; i < 20; ++i) {
		sort[i*2]   = 0x50;
		sort[i*2+1] = 0;
	}
	for (i = 0, s = 0; i < 20; ++i) {
		BYTE colv = (*src++ & 0x7f);
		BYTE attr = *src++;
		sort[i*2+1] = attr;	//機能属性はメモリ中の並び通り
		//桁指定部は昇順ソート
		if (i == 0) {
			sort[s*2] = colv;
			++s;
		} else if (colv == sort[(s-1)*2]) {
			;	//同一桁の指定は無視
		} else if (colv > sort[(s-1)*2]) {
			sort[s*2] = colv;
			++s;
		} else {
			int find = 0;
			//重複桁指定は無視する
			for (r = 0; r < s; ++r) {
				if (sort[r*2] == colv) {
					find = 1;
					break;
				}
			}
			if (!find) {
				//昇順並べ替え
				for (r = s; r > 0; --r) {
					if (sort[(r-1)*2] > colv) {
						BYTE c = sort[(r-1)*2];
						sort[r*2] = c;
						sort[(r-1)*2] = colv;
					} else
						break;
				}
				++s;
			}
		}
	}
}
	
static __inline void _movcolor() {
	int i, j;
	BYTE pac, prev_color = n80device.attr._prevcolor;

	// 色属性
	if (n80device.crtc._n80_color_mode == 1) {
		// モノクロ時は白一色
		memset(&n80device.attr._eval_attr[0], 7, 80);
	} else {
		int	start_col = -1;
		memset(&n80device.attr._eval_attr[0], 0, 80);
		for (i = 0; i < 20; ++i) {
			BYTE attr  = n80device.attr._attrcach[i*2+1];
			// 色属性指定の場合
			if (attr & 0x08) {
				int cols = n80device.attr._attrcach[i*2];
				// 色属性開始位置の保存
				if (start_col == -1)
					start_col = cols > 80 ? 80 : cols;
				
				if (cols < 80) {
					// GRB -> RGB
					pac = GRB2RGB[(attr >> 5) & 0x07];
					// 簡易グラフィック属性
					if (attr & 0x10)
						pac |= 0x10;
					// 次行繰越色
					n80device.attr._prevcolor = pac;
					// プレーン属性配列書き込み
					for (j = cols; j < 80; ++j)
						n80device.attr._eval_attr[j] = pac;
				}
			}
		}
		// 途中から開始されている場合は最終色属性で埋める
		if (start_col > 0) {
			for (i = 0; i < start_col; ++i) {
				n80device.attr._eval_attr[i] = prev_color;
			}
		}
	}
}

static __inline void _movattr() {
	int i, j;
	BYTE prev_func = n80device.attr._prevfunc;
	BYTE mask = n80device.crtc._n80_color_mode == 0 ? 0x17 : 0x07;
	
	int	start_col = -1;
	for (i = 0; i < 20; ++i) {
		BYTE attr = n80device.attr._attrcach[i*2+1];
		// 機能属性指定の場合
		if (n80device.crtc._n80_color_mode == 1 || !(attr & 0x08)) {
			int cols = n80device.attr._attrcach[i*2];
			// 機能属性開始位置の保存
			if (start_col == -1)
				start_col = cols > 80 ? 80 : cols;
			
			if (cols < 80) {
				// PD3301属性 -> PAC
				BYTE pac = n80device.attr._convfunc(attr);
				//点滅が指定されている場合は、ブリンク時に消す
				//但し、消去属性が指定されている場合は、そちらを優先する
				if ((attr & 0x03) == 0x02 && !n80device.cursor._n80_cur_blink) {
					pac |= 0x40;
				}
				for (j = cols; j < 80; ++j) {
					n80device.attr._eval_attr[j] &= mask;
					n80device.attr._eval_attr[j] |= pac;
				}
				// 次行繰越機能
				n80device.attr._prevfunc = pac;// & 0x70;
			}
		}
	}
	// 途中から開始されている場合は最終色属性で埋める
	if (start_col > 0) {
		for (i = 0; i < start_col; ++i) {
			n80device.attr._eval_attr[i] &= mask;
			n80device.attr._eval_attr[i] |= prev_func;
		}
	}
	// 強制反転モードの場合
	if (n80device.crtc._attr_reverse) {
		for (i = 0; i < 80; ++i)
			n80device.attr._eval_attr[i] ^= 0x20;
	}
}
//-----------------------------------------------------------------------------
//	Plane Attribute Code (PAC) Format;	*ERG*RGB
//                                      ||||+上線
//                                      |||+ 簡易グラフィック
//                                      ||+- 反転モード
//                                      |+-- 消去モード
//                                      +--- 下線モード
//-----------------------------------------------------------------------------
static __inline BYTE _chk_funcbit(BYTE al, BYTE cl) {
	if (al & 0x01)	// 消去
		cl |= 0x40;
	if (al & 0x04)	// 反転
		cl |= 0x20;
	if (al & 0x80)	// グラフィック
		cl |= 0x10;
	if (al & 0x10)	// 上線
		cl |= 0x08;
	if (al & 0x20)	// 下線
		cl |= 0x80;
	return cl;
}

static BYTE _convmono(BYTE attr) {
	return _chk_funcbit(attr & 0xf7, 0);
}

static BYTE _convcolor(BYTE attr) {
	return _chk_funcbit(attr & 0x37, 0);
}

static int _n80_eval_attrib(int line) {
	int next = line == 24 ? 0 : line + 1, result = 0;

	if (line < 0 || line > 24)
		return 0;

	n80device.attr._prevcolor = n80device.attr._nextcolor[line];
	n80device.attr._prevfunc  = n80device.attr._nextfunc[line];
	
	// 属性評価
	_movcolor();
	_movattr();

	// カーソル明滅の更新
	if (n80device.cursor._n80_cur_row == line	&&
		n80device.cursor._n80_cur_blink			&&
		n80device.cursor._n80_cur_enable) {
		n80device.attr._eval_attr[n80device.cursor._n80_cur_col] ^= 0x20;	// reverse
		n80device.attr._eval_attr[n80device.cursor._n80_cur_col] |= 0x07;	// color white
	}

	// 次行への繰越色が変更になった場合
	if (n80device.attr._nextcolor[next] != n80device.attr._prevcolor)
		result = 1;
	n80device.attr._nextcolor[next] = n80device.attr._prevcolor;
	n80device.attr._nextfunc[next]  = n80device.attr._prevfunc;

	return result;
}

static void _n80_chg_converter(int mode) {
	n80device.attr._convfunc = mode == 0 ? _convcolor : _convmono;
}

static void _n80_attr_invalid() {
	int i;
	n80device.vram_sync_enter();
	for (i = 0; i < 25; ++i)
		n80device.attr._attrchg[i] = 1;
	n80device.vram_sync_leave();
}

static void _n80_attr_set(int line, BYTE flg) {
	if (line < 0 || line > 24)
		return;
	n80device.vram_sync_enter();
	n80device.attr._attrchg[line] = flg;
	n80device.vram_sync_leave();
}

static void _n80_reset_attr() {
	//attribute convert mode init (mono)
	_n80_chg_converter(1);
}

export void _n80_init_attr() {
	n80device.attr.reset = _n80_reset_attr;
	n80device.attr.chg_converter = _n80_chg_converter;
	n80device.attr.invalid = _n80_attr_invalid;
	n80device.attr.set = _n80_attr_set;
	n80device.attr.cache = _n80_attr_cache;
	n80device.attr.eval = _n80_eval_attrib;
	_n80_reset_attr();
}
	
#ifdef __cplusplus
}	// extern "C"
#endif
