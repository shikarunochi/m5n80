/**--------------------------------------------------------------------------
 **
 **    NEC PC-8001 Emulator "N80" for Zaurus(Spitz)
 **    --------------------------------------------
 **    Copyright (c) 1993-2017 H.Kanemoto
 **
 **    frame buffer operations
 **
 **    Rev    Date    Descriptions
 **    -----  ----------  -----------------------------------------------
 **    1.00  2010/12/08  Zaurus SL-C3000 Fedora Core 10 ARM版 porting
 **    2.00  2017/03/27  Raspberry Pi2 Raspbian(wheezy) ARM版 porting
 **------------------------------------------------------------------------
***------------------------------------------------------------------------
**  For  M5Stack 2018/11/18 modified by @shikarunochi 
**------------------------------------------------------------------------*/

#include <stdio.h>
#include <M5Atom.h>  
#include "FS.h"
#include <SPIFFS.h>
#include "m5n80lgfx.h"
#include "n80.h"
#include "pc-8001.h"
#define  export
/**--------------------------------------------------------------------------
 **  外部変数／関数
 **------------------------------------------------------------------------*/
extern N80LX    n80lx;
export const int SPITZ_COLOR[8] = {
  BLACK,
  RED,
  GREEN,
  CYAN,
  BLUE,
  MAGENTA,
  YELLOW,
  WHITE
};
/**--------------------------------------------------------------------------
 **  内部変数／関数
 **------------------------------------------------------------------------*/
static void     blt4020(int line);
static void     blt4025(int line);
static void     blt8020(int line);
static void     blt8025(int line);
typedef void     (*BLTFNC)(int);
static BLTFNC    bltfncs[] = {blt4020,blt4025,blt8020,blt8025};
static BLTFNC    fnc = blt4020;

LGFX_Sprite canvas(&m5lcd);//40文字フォント拡大用

void initDisplay(){
  canvas.setColorDepth(8);
  canvas.createSprite(16,8); 
}

void renderDisplay(){
   
}

/**--------------------------------------------------------------------------
 **  N80 main thread
 **------------------------------------------------------------------------*/
export int init_font()
{
  File fontFile;
  int  i, len;
  
  // font file load
  fontFile = SPIFFS.open(String(ROM_DIRECTORY) + "/PC-8001.FON", FILE_READ);
  if (!fontFile) {
    Serial.println("can not open font file");
    return 1;
  }
  int offset = 0;
  while (fontFile.available()) {
     *(n80device.font + offset) = fontFile.read();
     offset++;
  }
  fontFile.close();
  if (offset != (1024*4)) {
    Serial.print("invalid font file size:");
    Serial.println(offset);
    return 1;
  }
   
  Serial.println("Font Read OK!:");
  // フォントデータをPCGにコピー
  for (i = 0; i < 2048; ++i){
    n80device.pcg[i] = n80device.font[i];
  }
  Serial.println("PCG SET OK!:");
 
  Serial.println("Font OK!");
  return 0;
}

export void enter()
{
  pthread_mutex_lock(&n80lx.vram_sync);
}

export void leave()
{
  pthread_mutex_unlock(&n80lx.vram_sync);
}

export void  chg_bltfunc(int mode)
{
  fnc = bltfncs[mode];
}
static const int pitch = 640;
static void blt4020(int line)
{
  if (line < 20) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int col = 0;
    BYTE   ascii; 
    BYTE   attr;
    LPBYTE   fnt;
    int color;
    int r,h;
    
    while(col < 40) {
      ascii = *txts;
      attr  = *pacs;
      fnt = &n80device.font[ascii*8];
      color = attr & 0x07;
        ascii = *txts;
        attr  = *pacs;
        fnt   = &n80device.font[ascii*8];
        if (attr & 0x10)        // graph
           fnt += 2048;
        else if (n80device.pcg_mode)  // pcg
          fnt += 4096;
      
        for (r = 0; r < 10; ++r) {
          BYTE   bit;
          LPDWORD   ptr;
        
          if (r > 0 && r < 9)
            bit = *fnt++;
          else
            bit = 0;
        
          if (attr & 0x40)      // secret
            bit = 0;
          if (attr & 0x80 && r == 9)  // under line
            bit |= 0xff;
          if (attr & 0x08 && r == 0)  // upper line
            bit |= 0xff;
          if (attr & 0x20)      // reverse
          bit = ~bit;

          BYTE wideBit[2];
          BYTE workBit = 0;
          
          for(int i = 0;i < 8;i++){
            if(((bit>>(7-i))& 1) >= 1){
              workBit = workBit << 1;
              workBit = workBit + 1;
              workBit = workBit << 1;
              workBit = workBit + 1;
            }else{
              workBit = workBit << 2;
            }
            if(i == 3){
              wideBit[0] = workBit;
              workBit = 0;
            }else if(i == 7){
              wideBit[1] = workBit;
            }
          }

          m5lcd.fillRect(col * 8 * 2 ,line * 10 + r , 16, 1, BLACK);
          m5lcd.drawBitmap(col * 16 , line * 10 + r , (const uint8_t*)(&wideBit[0]), 8, 1, SPITZ_COLOR[color]);
          m5lcd.drawBitmap(col * 16 + 8, line * 10 + r , (const uint8_t*)(&wideBit[1]), 8, 1, SPITZ_COLOR[color]);
        }
        txts += 2;
        pacs += 2;
        col = col+ 1;
    }
  }
}


static void blt4025(int line)
{
  if (line < 25) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int col = 0;
    BYTE   ascii; 
    BYTE   attr;
    LPBYTE   fnt;
    int color;
    int r,h;
    
    while(col < 40) {
      ascii = *txts;
      attr  = *pacs;
      fnt = &n80device.font[ascii*8];
      color = attr & 0x07;
    
        ascii = *txts;
        attr  = *pacs;
        fnt   = &n80device.font[ascii*8];
        if (attr & 0x10)        // graph
           fnt += 2048;
        else if (n80device.pcg_mode)  // pcg
          fnt += 4096;
      
        for (r = 0; r < 8; ++r) {
          BYTE   bit = *fnt++;
          LPDWORD   ptr;

          if (attr & 0x40)      // secret
            bit = 0;
          if (attr & 0x80 && r == 7)  // under line
            bit |= 0xff;
          if (attr & 0x08 && r == 0)  // upper line
            bit |= 0xff;
          if (attr & 0x20)      // reverse
          bit = ~bit;

          BYTE wideBit[2];
          BYTE workBit = 0;
          
          for(int i = 0;i < 8;i++){
            if(((bit>>(7-i))& 1) >= 1){
              workBit = workBit << 1;
              workBit = workBit + 1;
              workBit = workBit << 1;
              workBit = workBit + 1;
            }else{
              workBit = workBit << 2;
            }
            if(i == 3){
              wideBit[0] = workBit;
              workBit = 0;
            }else if(i == 7){
              wideBit[1] = workBit;
            }
          }

          m5lcd.fillRect(col * 8 * 2 ,line * 8 + r , 16, 1, BLACK);
          m5lcd.drawBitmap(col * 16 , line * 8 + r , (const uint8_t*)(&wideBit[0]), 8, 1, SPITZ_COLOR[color]);
          m5lcd.drawBitmap(col * 16 + 8, line * 8 + r , (const uint8_t*)(&wideBit[1]), 8, 1, SPITZ_COLOR[color]);
        }
        txts += 2;
        pacs += 2;
        
      col = col+ 1;
    }
  }
}

static void blt8020(int line)
{
  if (line < 20) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int  col = 0;
    BYTE ascii = 0; 
    BYTE  attr = 0;
    LPBYTE  fnt = 0;
    int color = 0;
    int r,h;

    while(col < 80) {
    
      ascii = *txts;
      attr  = *pacs;
      fnt   = &n80device.font[ascii*8];
      color = attr & 0x07;
      
      ascii = *txts;
      attr  = *pacs;
      
      fnt   = &n80device.font[ascii*8];      
      if (attr & 0x10)        // graph
        fnt += 2048;
      else if (n80device.pcg_mode)  // pcg
        fnt += 4096;
    
      for (r = 0; r < 10; ++r) {
        BYTE   bit;
        LPDWORD   ptr;
        BYTE   halfBit;
      
        if (r > 0 && r < 9)
          bit = *fnt++;
        else
          bit = 0;
      
        if (attr & 0x40)      // secret
          bit = 0;
        if (attr & 0x80 && r == 9)  // under line
          bit |= 0xff;
        if (attr & 0x08 && r == 0)  // upper line
          bit |= 0xff;
        if (attr & 0x20)      // reverse
          bit = ~bit;

          m5lcd.fillRect(col * 8  ,line * 10 + r , 8, 1, BLACK);
          m5lcd.drawBitmap(col * 8 , line * 10 + r , (const uint8_t*)(&bit), 8, 1, SPITZ_COLOR[color]);
      }
      txts += 1;
      pacs += 1;
      col = col + 1;
    }    
  }
}

static void blt8025(int line)
{
  if (line < 25) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int  col = 0;
    BYTE ascii = 0; 
    BYTE  attr = 0;
    LPBYTE  fnt = 0;
    int color = 0;
    int r,h;

    while(col < 80) {
    
      ascii = *txts;
      attr  = *pacs;
      fnt   = &n80device.font[ascii*8];
      color = attr & 0x07;
      
      ascii = *txts;
      attr  = *pacs;
      
      fnt   = &n80device.font[ascii*8];      
      if (attr & 0x10)        // graph
        fnt += 2048;
      else if (n80device.pcg_mode)  // 
        fnt += 4096;
    
      for (r = 0; r < 8; ++r) {
        BYTE   bit = *fnt++;
        LPDWORD   ptr;
 
        if (attr & 0x40)      // secret
          bit = 0;
        if (attr & 0x80 && r == 7)  // under line
          bit |= 0xff;
        if (attr & 0x08 && r == 0)  // upper line
          bit |= 0xff;
        if (attr & 0x20)      // reverse
          bit = ~bit;

        m5lcd.fillRect(col * 8  ,line * 8 + r , 8, 1, BLACK);
        m5lcd.drawBitmap(col * 8 , line * 8 + r , (const uint8_t*)(&bit), 8, 1, SPITZ_COLOR[color]);
      }
      txts += 1;
      pacs += 1;
      col = col + 1;
    }    
  }
}

export void reflesh_screen()
{
  int i, change_attr = 0, change_prev = 0, work_lines = 0;
  m5lcd.startWrite();
  //m5lcd.fillRect(0, 0, 320, 200, BLACK);
  for (i = 0; i < 25; ++i) {
    n80device.vram_sync_enter();
    if (n80device.attr._attrchg[i] == 1 || change_prev) {
      n80device.attr.cache(i);
      change_attr = 1;
    } else{
      change_attr = 0;
    }
    n80device.vram_sync_leave();

    if (change_attr){
      change_prev = n80device.attr.eval(i) == 1 ? 1 : 0;
    }
    if (change_attr) {
      fnc(i);
      ++work_lines;
    }
  }
  m5lcd.endWrite();
}

export void draw_text(int line, LPBYTE attr, LPBYTE text) {
  memcpy(n80device.attr._textcach, text, 80);
  memcpy(n80device.attr._eval_attr, attr, 80);
  blt8025(line);
}

void updateStatusArea(const char* message){

}
