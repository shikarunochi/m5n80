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
#include <M5Stack.h>
#include "n80.h"
#include "pc-8001.h"
#define  export
/**--------------------------------------------------------------------------
 **  外部変数／関数
 **------------------------------------------------------------------------*/
extern N80LX    n80lx;
export const DWORD  SPITZ_COLOR[8] = {
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

TFT_eSprite fb = TFT_eSprite(&M5.Lcd);
TFT_eSprite statusArea = TFT_eSprite(&M5.Lcd);

void initDisplay(){
  fb.setColorDepth(8);
  fb.createSprite(320, 200);
  fb.fillSprite(TFT_BLACK);
  fb.pushSprite(0, 0);

  statusArea.setColorDepth(8);
  statusArea.createSprite(260,12);
  statusArea.fillSprite(TFT_BLACK);

  //Test
//  int testX = 0;
//  int testY = 0;
//  for(int index = 0;index < 4096;index = index + 8){
//    Serial.println(index);
//     fb.drawBitmap(testX, testY, (const uint8_t*)(n80device.font + index), 8, 8, WHITE);    
//     testX = testX + 10;
//     if(testX > 320){
//      testX = 0;
//      testY = testY + 10;
//     }
//  }
//  fb.pushSprite(0, 0);
//  while(1);

}

void renderDisplay(){
  fb.pushSprite(0, 0);    
}

/**--------------------------------------------------------------------------
 **  N80 main thread
 **------------------------------------------------------------------------*/
export int init_font()
{
  File fontFile;
  int  i, len;
  
  // font file load
  fontFile = SD.open(String(ROM_DIRECTORY) + "/PC-8001.FON", FILE_READ);
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
  BYTE charBitmap[10 * 40];
  if (line < 20) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int col = 0;
    int sameColorCount = 0;
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
    
      //同じcolorが続く場合はまとめて描画
      sameColorCount = 1;
      while((*(pacs + (sameColorCount*2)) & 0x07) == color){
         sameColorCount++;
         if(sameColorCount >= 40 - col){
            break;
         }
      }
      for(int index = 0;index < sameColorCount;index++){
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

          charBitmap[sameColorCount * r + index] = bit;
          //fb.fillRect(col * 8, line * 10 + r , 8, 1, BLACK);
          //fb.drawBitmap(col * 8, line * 10 + r, (const uint8_t*)(&bit), 8, 1, SPITZ_COLOR[color]);
        }
        txts += 2;
        pacs += 2;
      }
      fb.drawBitmap(col * 8, line * 10, (const uint8_t*)(charBitmap), 8 * sameColorCount, 10, SPITZ_COLOR[color]);
      col = col+ sameColorCount;
    }
  }
}

static void blt4025(int line)
{
  BYTE charBitmap[8 * 40];
  if (line < 25) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int      col = 0;
    int sameColorCount = 0;
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

      //同じcolorが続く場合はまとめて描画
      sameColorCount = 1;
      while((*(pacs + (sameColorCount*2)) & 0x07) == color){
         sameColorCount++;
         if(sameColorCount >= 40 - col){
            break;
         }
      }
      for(int index = 0;index < sameColorCount;index++){
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
        
            charBitmap[sameColorCount * r + index] = bit;
        }
        txts += 2;
        pacs += 2;
      }
      fb.drawBitmap(col * 8, line * 8, (const uint8_t*)(charBitmap), 8 * sameColorCount, 8, SPITZ_COLOR[color]);
      col = col+ sameColorCount;
    }
  }
}

static void blt8020(int line)
{
  BYTE charBitmap[10 * 40];
  if (line < 20) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int  col = 0;
    int sameColorCount;
    int fixedSameColorCount;
    BYTE ascii = 0; 
    BYTE  attr = 0;
    LPBYTE  fnt = 0;
    int color = 0;
    int r,h;

    while(col < 80) {
      for(int i = 0;i < 10*40; i++){
        charBitmap[i] = 0;
      }
    
      ascii = *txts;
      attr  = *pacs;
      fnt   = &n80device.font[ascii*8];
      color = attr & 0x07;
      
      //同じcolorが続く場合はまとめて描画
      sameColorCount = 1;
      while((*(pacs + (sameColorCount)) & 0x07) == color){
         sameColorCount++;
         if(sameColorCount >= 80 - col){
            break;
         }
      }

      fixedSameColorCount = sameColorCount;
      //sameColorCountが奇数の場合は、最後に１文字分空白を付与して処理する。
      if(sameColorCount % 2 == 1){
          fixedSameColorCount = fixedSameColorCount + 1;
      }
      
      for(int index = 0;index < sameColorCount;index++){
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

          //bit 8 ビットを、４ビットに圧縮する。（単にOR取るだけなので表示はつぶれます。）
          halfBit = (bit & 0x80) | ((bit << 1) & 0xC0) |((bit << 2) & 0x60)|((bit << 3) & 0x30)|((bit << 4) & 0x10);

          //index 偶数なら前半４バイト、奇数なら後半４バイトに設定
          int halfIndex = (fixedSameColorCount / 2 * r) + index / 2;
          if(index % 2 == 0){
             charBitmap[halfIndex] =  halfBit;
          }else{
             charBitmap[halfIndex] = charBitmap[halfIndex] | (halfBit >> 4);
          }
        }
        txts += 1;
        pacs += 1;
      }

      fb.drawBitmap(col * 4, line * 10, (const uint8_t*)(charBitmap), 8 * (fixedSameColorCount / 2), 10, SPITZ_COLOR[color]);
      col = col+ sameColorCount;
    }    
  }
}

static void blt8025(int line)
{
  BYTE charBitmap[8 * 40];
  if (line < 25) {
    LPBYTE    txts = &n80device.attr._textcach[0];
    LPBYTE    pacs = &n80device.attr._eval_attr[0];
    int col = 0;
    int sameColorCount;
    int fixedSameColorCount;
    BYTE ascii = 0; 
    BYTE  attr = 0;
    LPBYTE  fnt = 0;
    int color = 0;
    int r,h;

  while(col < 80) {
      for(int i = 0;i < 8*40; i++){
        charBitmap[i] = 0;
      }
    
      ascii = *txts;
      attr  = *pacs;
      fnt   = &n80device.font[ascii*8];
      color = attr & 0x07;
      
      //同じcolorが続く場合はまとめて描画
      sameColorCount = 1;
      while((*(pacs + (sameColorCount)) & 0x07) == color){
         sameColorCount++;
         if(sameColorCount >= 80 - col){
            break;
         }
      }

      fixedSameColorCount = sameColorCount;
      //sameColorCountが奇数の場合は、最後に１文字分空白を付与して処理する。
      if(sameColorCount % 2 == 1){
          fixedSameColorCount = fixedSameColorCount + 1;
      }
       
      for(int index = 0;index < sameColorCount;index++){
        ascii = *txts;
        attr  = *pacs;
        
        fnt   = &n80device.font[ascii*8];      
        if (attr & 0x10)        // graph
          fnt += 2048;
        else if (n80device.pcg_mode)  // pcg
          fnt += 4096;
      
        for (r = 0; r < 8; ++r) {
          BYTE   bit;
          LPDWORD  ptr;
          BYTE   halfBit;
        
          bit = *fnt++;
        
          if (attr & 0x40)      // secret
            bit = 0;
          if (attr & 0x80 && r == 9)  // under line
            bit |= 0xff;
          if (attr & 0x08 && r == 0)  // upper line
            bit |= 0xff;
          if (attr & 0x20)      // reverse
            bit = ~bit;

          //bit 8 ビットを、４ビットに圧縮する。（単にOR取るだけなので表示はつぶれます。）
          halfBit = (bit & 0x80) | ((bit << 1) & 0xC0) |((bit << 2) & 0x60)|((bit << 3) & 0x30)|((bit << 4) & 0x10);

          //index 偶数なら前半４バイト、奇数なら後半４バイトに設定
          int halfIndex = (fixedSameColorCount / 2 * r) + index / 2;
          if(index % 2 == 0){
             charBitmap[halfIndex] =  halfBit;
          }else{
             charBitmap[halfIndex] = charBitmap[halfIndex] | (halfBit >> 4);
          }
        }
        txts += 1;
        pacs += 1;
      }

      fb.drawBitmap(col * 4, line * 8, (const uint8_t*)(charBitmap), 8 * (fixedSameColorCount / 2), 8, SPITZ_COLOR[color]);
      col = col+ sameColorCount;
    }    
  }
}

export void reflesh_screen()
{
  int i, change_attr = 0, change_prev = 0, work_lines = 0;
  fb.fillRect(0, 0, 320, 200, BLACK);
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
}

export void draw_text(int line, LPBYTE attr, LPBYTE text) {
  memcpy(n80device.attr._textcach, text, 80);
  memcpy(n80device.attr._eval_attr, attr, 80);
  blt8025(line);
}

void updateStatusArea(const char* message){
  statusArea.fillSprite(TFT_BLACK);
  statusArea.setCursor(0,0);
  statusArea.print(message);
  statusArea.pushSprite(0, 228);
}
