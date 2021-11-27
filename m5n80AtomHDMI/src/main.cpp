/**--------------------------------------------------------------------------
 **
 **		NEC PC-8001 Emulator "N80" for Zaurus(Spitz)
 **		--------------------------------------------
 **		Copyright (c) 1993-2010 H.Kanemoto
 **
 **		main
 **
 **		Rev		Date		Descriptions
 **		-----	----------	-----------------------------------------------
 **		1.00	2010/12/08	Zaurus SL-C3000 Fedora Core 10 ARM版 porting
 **		2.00	2017/03/27	Raspberry Pi2 Raspbian(wheezy) ARM版 porting
 **------------------------------------------------------------------------
**------------------------------------------------------------------------
**  For  M5Stack 2018/11/18 modified by @shikarunochi 
**------------------------------------------------------------------------*/
#include <M5Atom.h>  
#include "m5n80lgfx.h"
#include "FS.h"
#include <SPIFFS.h>

#include "hid_server/hid_server.h"
#include "hid_server/hci_server.h"

extern "C" {
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
//#include <memory.h>
#include <locale.h>
#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <linux/input.h>
//#include <linux/uinput.h>
}
#include "pc-8001.h"
#include "n80.h"
#define	export
/**--------------------------------------------------------------------------
 **	external variables/functions
 **------------------------------------------------------------------------*/
export N80LX	n80lx;
extern int		init_font();						/** fb.cpp **/
extern void		enter();
extern void		leave();
extern void		chg_bltfunc(int mode);
extern void		reflesh_screen();
extern "C" void		onkey_down(BYTE code);				/** key.c **/
extern "C" void		onkey_up(BYTE code);

extern "C" void		onkey_down_HID(BYTE code);				/** key.c **/
extern "C" void		onkey_up_HID(BYTE code);

extern void		chooser(int fd);					/** file.c **/
//extern int		init_alsa();						/** alsa.c **/
//extern void		dispose_alsa();
//extern void*	alsa_sound_fnc(void *param);
//extern "C" void 	_n80_beep_on();
//extern "C" void 	_n80_beep_off();
//extern "C" void 	_n80_sound_on(USHORT freq);
//extern "C" void		_n80_sound_off();
//extern void 	_n80_append_sound_ticks(DWORD T);

extern "C" void		_n80_device_initialize(LPN80DEVICE, SYNCFNC enter, SYNCFNC leave, CMODFNC chgmod);
extern "C" BYTE		_n80_mbread(USHORT Adr, DWORD *W);
extern "C" void		_n80_mbwrite(USHORT Adr, BYTE v, DWORD *W);
extern "C" BYTE		_n80_ioread(USHORT Port);
extern "C" void		_n80_iowrite(USHORT Port, BYTE d);
extern "C" void		__z80_set_external_method(LPZ80CPU cpu, LPZ80FNCMBR mbr, LPZ80FNCMBW mbw, LPZ80FNCIOR ior, LPZ80FNCIOW iow);

bool needDrawFlag;
bool suspendMachine;
bool btKeyboardConnect;

#define MAX_FILES 255 // this affects memory

String chooseN80File();
void sortList(String fileList[], int fileListCount);
void systemMenu();
static bool saveN80Image();

static void keyInput(BYTE serialKeyCode);

static bool intFlag = false;

static uint8_t tenkeyMode = 0;

static BYTE cmt_buff[1024*32];

#define SP_VAL      (0x7f3e)
#define PC_VAL      (0xff3d)

//int buttonApin = 26; //赤ボタン
//int buttonBpin = 36; //青ボタン

bool joyPadPushed_U;
bool joyPadPushed_D;
bool joyPadPushed_L;
bool joyPadPushed_R;
bool joyPadPushed_A;
bool joyPadPushed_B;
bool joyPadPushed_Press;
bool enableJoyPadButton;

void checkJoyPad();

#define CARDKB_ADDR 0x5F
void checkI2cKeyboard();

static void keyboard(const uint8_t* d, int len);
void gui_hid(const uint8_t* hid, int len);  // Parse HID event

LGFX_Sprite menuCanvas(&m5lcd);

CRGB dispColor(uint8_t g, uint8_t r, uint8_t b) {
  return (CRGB)((g << 16) | (r << 8) | b);
}

//--------------------------------------------------------------
// シグナル関連
//--------------------------------------------------------------
void sighandler(int act)
{
  intFlag = true;
}

bool intByUser(void)
{
  return intFlag;
}

void pc80_exit(int arg)
{
  sighandler(0);
}

//--------------------------------------------------------------
//シフト入力が必要なキーの場合、シフトと併用して押すキーコードを返す。
//--------------------------------------------------------------
BYTE check_shift(BYTE code){
  BYTE  key = 0x00;
  if(code >= 'A' && code <= 'Z'){
    return code + 0x20;
  }
  switch (code) {
  case '!': key = '1';break;
  case '"': key = '2';break;
  case '#': key = '3';break;
  case '$': key = '4';break;
  case '%': key = '5';break;
  case '&': key = '6';break;
  case '\'': key = '7';break;
  case '(': key = '8';break;
  case ')': key = '9';break;
  case '=': key = '-';break;
  case '~': key = '^';break;
  case '`': key = '@';break;
  case '{': key = '[';break;
  case '}': key = ']';break;
  case '+': key = ';';break;
  case '*': key = ':';break;
  case '<': key = ',';break;
  case '>': key = '.';break;
  case '?': key = '/';break;
  
  case 0x11: key = 0x12;break; // DOWN CURSOR
  case 0x14: key = 0x13;break; // LEFT CURSOR
  case 0x18: key = 0x17;break; // INST
  }
  return key;
}

/**--------------------------------------------------------------------------
 ** Z80 thread
 **------------------------------------------------------------------------*/
static void *z80_thread_func(void *param){
  volatile double clock_sync = getclock() + IVLSEC;
  volatile DWORD  state = 0, req;
  volatile int  quit = 0;
  needDrawFlag = false;

  while(!intByUser()){
    if(suspendMachine == true){
      delay(100);
      clock_sync  = getclock() + IVLSEC;
      state       = 0;
      n80device.dmac._penarty = 0;
      continue;
    }
    // z80 step execute
    if (n80device.dmac._penarty > 0) {
      if (n80device.dmac._penarty > 20) {
        n80lx.cpu.r.x.T  = 20;
        n80device.dmac._penarty -= 20;
      } else {
        n80lx.cpu.r.x.T  = n80device.dmac._penarty;
        n80device.dmac._penarty = 0;
      }
      n80lx.cpu.r.x.W = 0;
    } else {
      // Z80 step execute
      if (n80lx.cpu.step(&n80lx.cpu)) {
        // notify if detect illegal instructions
        Serial.println("HALT");
        quit = 1;
        break;
      }
    }
    n80lx.cpu.r.x.T += n80lx.cpu.r.x.W;
    // vrtc operation
    if (n80device.vrtc.blanking(n80lx.cpu.r.x.T)) {
      // draw screen
      //write(pipe->write, "d", 1); // request z80 to disp
      needDrawFlag = true;
    }
    // beep/pcg ticks
    //_n80_append_sound_ticks(n80lx.cpu.r.x.T);
    
    // cpu clock sync
    state += n80lx.cpu.r.x.T;
    if (state >= n80device.param.sync_ticks) {
      // let alsa play sound
      //interlocked_exchange(&n80lx.request_alsa_signal, 2);

      double present = getclock();
      while (present < clock_sync) {
        double diff = clock_sync - present;
        if (diff >= 0.001) {  // 1ms以上ある場合はsleep
          DWORD  slp  = (DWORD) (diff * 1000);
          delay(slp);
          break;
        } else
          present = getclock();
      }
      delay(1);
      state -= n80device.param.sync_ticks;
      clock_sync += IVLSEC; // 20ms
      if ((present = getclock()) >= clock_sync)
        clock_sync = present + IVLSEC;
    }  
  }

  return NULL;
}
/**--------------------------------------------------------------------------
 **  display thread
 **------------------------------------------------------------------------*/
static void *disp_thread_func(void *param){
    //int overCount = 0;
    //int totalCount = 0;
    long synctime = 50;// 20fps
    //long synctime = 100;// 10fps
    long timeTmp;
    long vsyncTmp;
    while(!intByUser()){
      if(needDrawFlag == true && suspendMachine==false){
        //totalCount++;
        timeTmp = millis();
        pthread_mutex_lock(&n80lx.display_sync);
        reflesh_screen();
        renderDisplay();
        needDrawFlag = false;
        pthread_mutex_unlock(&n80lx.display_sync);
        vsyncTmp = millis();
        if(synctime - (vsyncTmp - timeTmp) > 0){
          delay(synctime - (vsyncTmp - timeTmp));
        }else{
          delay(1);
          //overCount++;
          //if(overCount % 100 == 0){
          //  Serial.printf("OverCount:%d / %d : %d \n",overCount, totalCount, (vsyncTmp - timeTmp));
          //}
        }
      }else{
        delay(synctime);
      }
    }
    return NULL;
}

static void keyInput(BYTE serialKeyCode){
  if(serialKeyCode ==2){ //CTRL+B だけ対応
    onkey_down(KEY_CTRL);//コントロール押下
    delay(10);
    if(serialKeyCode == 0){
      onkey_down(0x20); //スペース
    }else{
      onkey_down(serialKeyCode + 0x60);//該当アルファベット
    }
    delay(60);
    if(serialKeyCode == 0){
       onkey_up(0x20);
    }else{
      onkey_up(serialKeyCode + 0x60);
    }
    delay(10);
    onkey_up(KEY_CTRL);
    delay(60);
    return;
  }
  
  BYTE checkShiftKeyCode = check_shift(serialKeyCode);
  if(checkShiftKeyCode != 0x00){
    onkey_down(KEY_SHIFT); //SHIFT
    delay(10);
    onkey_down(checkShiftKeyCode);
    delay(60);
    onkey_up(checkShiftKeyCode);
    delay(10);
    onkey_up(KEY_SHIFT);
    delay(60);
  }else{
    onkey_down(serialKeyCode);
    delay(60);
    onkey_up(serialKeyCode);
    delay(60);
  }
  
}

static void n80main(){
  Serial.println("Start Main");
  BYTE serialKeyCode;
  int r, quit = 0;
  while(!intByUser()){
    if(suspendMachine == true){
      delay(100);
      continue;
    }
    
  //キー入力チェック
    if (Serial.available() > 0) {
      serialKeyCode = Serial.read();
      //Special Key
      switch( serialKeyCode ){
        case 27: //ESC
          serialKeyCode = Serial.read();
          if(serialKeyCode == 91){
            serialKeyCode = Serial.read();
            switch(serialKeyCode){
              case 65:
                serialKeyCode = 0x12;  //UP
                break;
              case 66:
                serialKeyCode = 0x11;  //DOWN
                break;
              case 67:
                serialKeyCode = 0x13;  //RIGHT
                break;
              case 68:
                serialKeyCode = 0x14;  //LEFT
                break;
              case 49:
                serialKeyCode = 0x15;  //HOME
                break;
              case 52:
                serialKeyCode = 0x16;  //END -> CLR
                break;
              case 50:
                serialKeyCode = 0x18;  //INST
                break;
              default:
                serialKeyCode = 0;
            }
          }
          break;
        case 127://BackSpace
          serialKeyCode = 0x17;
          break;
        case 0x30:
          serialKeyCode = KEY_TENKEY_0;
          break;
        case 0x31:
          serialKeyCode = KEY_TENKEY_1;
          break;
        case 0x32:
          serialKeyCode = KEY_TENKEY_2;
          break;
        case 0x33:
          serialKeyCode = KEY_TENKEY_3;
          break;
        case 0x34:
          serialKeyCode = KEY_TENKEY_4;
          break;
        case 0x35:
          serialKeyCode = KEY_TENKEY_5;
          break;
        case 0x36:
          serialKeyCode = KEY_TENKEY_6;
          break;
        case 0x37:
          serialKeyCode = KEY_TENKEY_7;
          break;
        case 0x38:
          serialKeyCode = KEY_TENKEY_8;
          break;
        case 0x39:
          serialKeyCode = KEY_TENKEY_9;
          break;
      }
      
      
      while(Serial.available() > 0 && Serial.read() != -1);
      //Serial.printf("KEY: %x",serialKeyCode);
      //Serial.println();
      keyInput(serialKeyCode);
    }else{
      delay(10);
    }

    hid_update();
    uint8_t buf[64];
    int n = hid_get(buf,sizeof(buf));    // called from emulation loop
    if(n != -1){ //Bluetoothキーボード接続完了…と思われる。
      if(btKeyboardConnect == false){
        btKeyboardConnect = true;
        //接続したらLEDを緑に。
        M5.dis.drawpix(0, dispColor(0,50,0));
        updateStatusArea("");
      }
    }
    if (n > 0){
          gui_hid(buf,n);
    }

    M5.update();

    if (M5.Btn.wasReleasefor(1000)) {
      //メニュー表示
      suspendMachine = true;
      delay(100);//描画サスペンド待ち
      systemMenu();
      m5lcd.fillScreen(TFT_BLACK);
      suspendMachine = false;
    }else if(M5.Btn.wasReleased()){
        suspendMachine = true;
        delay(100); //描画サスペンド待ち
        chooseN80File();
        m5lcd.fillScreen(TFT_BLACK);
        suspendMachine = false;
    }          

    //キーボードチェック
    checkI2cKeyboard();
    //ジョイパッドチェック
    checkJoyPad();
  }
  
  return;
}

int branch() {
 
//スレッド開始
  pthread_t th;
  suspendMachine = false;
  Serial.println("create z80 thread");
  if (pthread_create(&th, NULL, z80_thread_func, NULL /*&z80toDisp*/) != 0) {
      Serial.println("can not create z80 thread");
      return 0;
  }
  pthread_t disp;
  Serial.println("create Disp thread");
  if (pthread_create(&disp, NULL, disp_thread_func, NULL/*&disp2main*/) != 0) {
      Serial.println("can not create display thread");
      return 0;
  }

  n80main();

  pthread_join(disp, NULL);
  pthread_join(th, NULL);

}

int main_core()
{
  m5lcd.println("PC-8001 for M5Atom + AtomDisplay START");
  int   result = 1, len;
  File romFile;
  // N80 structure clean
  memset(&n80lx, 0, sizeof(n80lx));

  // Z80 Initialize
  __z80_set_external_method(&n80lx.cpu, _n80_mbread, _n80_mbwrite, _n80_ioread, _n80_iowrite);
 
//  n80device.beep_on   = _n80_beep_on;
//  n80device.beep_off  = _n80_beep_off;
//  n80device.sound_on  = _n80_sound_on;
//  n80device.sound_off = _n80_sound_off;


  // PC-8001 Device Initialize
  _n80_device_initialize(&n80device, enter, leave, chg_bltfunc);

  // font file load
  if (init_font())
    return 1;


  loadConfig();

  hid_init("emu32"); 
  m5lcd.println("HID START");
  btKeyboardConnect = false;
  M5.dis.drawpix(0, dispColor(50,0,0));

  menuCanvas.setColorDepth(8);
  menuCanvas.setTextSize(1);
  menuCanvas.createSprite(320,8); 

  // rom file load
  if (SPIFFS.exists(String(ROM_DIRECTORY) + "/PC-8001.ROM") == true){
    romFile = SPIFFS.open(String(ROM_DIRECTORY) + "/PC-8001.ROM", FILE_READ);
    m5lcd.println("read:PC-8001.ROM");
  }else if (SPIFFS.exists(String(ROM_DIRECTORY) + "/8801-N80.ROM") == true){
    romFile = SPIFFS.open("/8801-N80.ROM", FILE_READ);
    m5lcd.println("read:8801-N80.ROM");
  }

  if(!romFile){
    Serial.println("can not open ROM file");
    return 1;
  }
  
  int offset = 0;
  while (romFile.available()) {
     *(n80device.mem.map._rom + offset) = romFile.read();
     offset++;
  }
  romFile.close();

  if (offset != (1024*32) && offset != (1024*24)) {
    Serial.print("invalid rom file size:");
    Serial.println(offset);
    m5lcd.println("invalid rom file size:");
    m5lcd.println(offset);
    return 1;
  }

delay(2000); //表示確認用
//For JoyPad
  //pinMode(buttonApin, INPUT_PULLUP);
  //pinMode(buttonBpin, INPUT_PULLUP);
  joyPadPushed_U = false;
  joyPadPushed_D = false;
  joyPadPushed_L = false;
  joyPadPushed_R = false;
  joyPadPushed_A = false;
  joyPadPushed_B = false;
  joyPadPushed_Press = false;
  enableJoyPadButton = false;


//  n80lx.video = (DWORD*) ps_calloc(4, 640 * 400);
//  if (n80lx.video != NULL) {
//  }

//  // alsa init
//  if (init_alsa()) {
//    fprintf(stderr, "can not open alsa\n");
//    goto err_exit;
//  }
  // create mutex
  pthread_mutex_init(&n80lx.vram_sync, NULL);
  pthread_mutex_init(&n80lx.display_sync, NULL);
  pthread_mutex_init(&n80lx.sound_sync, NULL);
  pthread_mutex_init(&n80lx.pause_sync, NULL);

  initDisplay(); 

  // z80/alsa/disp threads start
  Serial.print("Branch!");
   branch();

  // dispose mutex
  pthread_mutex_destroy(&n80lx.vram_sync);
  pthread_mutex_destroy(&n80lx.display_sync);
  pthread_mutex_destroy(&n80lx.sound_sync);
  pthread_mutex_destroy(&n80lx.pause_sync);

  // free alsa
//  dispose_alsa();

  //free(n80lx.video);
  result = 0;
  
err_exit:
  return result;
}

int m5n80_main()
{
  Serial.println("[M5N80]Main Start");
  
  return main_core();
}

void selectN80File(String n80Filename){
    int     fsiz;
    uint16_t  spval;
    Serial.println("LOAD Start!");
    suspendMachine = true;
    delay(1000);
    File n80File;
    String n80FilePath = String(N80_DIRECTORY) + "/" + n80Filename;
    n80File = SPIFFS.open(n80FilePath , FILE_READ);
    if (n80File) {
      int offset = 0;
      while (n80File.available()) {
        *(n80device.mem.map._ram + offset) = n80File.read();
        offset++;
      }
      Serial.println("LOAD End");
      n80File.close();
      memcpy(&spval, n80device.mem.map._ram + SP_VAL,2);
      Serial.println("Set");
      n80lx.cpu.r.x.PC = PC_VAL;
      n80lx.cpu.r.x.SP = spval;
      Serial.println("SetEnd");
    }else{
      Serial.println("File not found");
    }
    suspendMachine = false;      
}

void selectCMTFile(String cmtFilename){
    Serial.println("selectCMTFile Start");
    m5lcd.println();
    suspendMachine = true;
    delay(100);

    File cmtFile;

    String cmtFilePath = String(CMT_DIRECTORY) + "/" + cmtFilename;
    cmtFile = SPIFFS.open(cmtFilePath , FILE_READ);
    if (cmtFile) {
      int offset = 0;
      while (cmtFile.available()) {
        cmt_buff[offset] = cmtFile.read();
        offset++;
      }
      n80device.cmt._size = offset;
      n80device.cmt._buff = cmt_buff;
      n80device.cmt._index = 0;
      Serial.printf("length: %d", offset);
      Serial.println("CMT LOAD End");
      m5lcd.printf("[length: %d]", offset);
      m5lcd.println("CMT Data Set OK!");
      cmtFile.close();
    }else{
      Serial.println("File not found");
      m5lcd.println("File not found");
    }
    delay(2000);
    suspendMachine = false;      
}

//--------------------------------------------------------------
// JoyPadLogic
//--------------------------------------------------------------

void checkJoyPad(){
//ジョイパッドは対応なし
/*
  int joyX,joyY,joyPress,buttonA,buttonB;
  if(Wire.requestFrom(0x52,3) < 3){
    return;
  }

  while(Wire.available()){
    joyX = Wire.read();//X（ジョイパッド的には Y）
    joyY = Wire.read();//Y（ジョイパッド的には X）
    joyPress = Wire.read();//Press    
  }
  if(joyX == 0){
    return; //接続されていない
  }

//  Serial.printf("%d:%d:%d", joyX,joyY,joyPress);
//  Serial.println();

  if (digitalRead(buttonApin) == LOW)
  {
    buttonA = 1;
  }else{
    buttonA = 0;
  }
  if (digitalRead(buttonBpin) == LOW)
  {
    buttonB = 1;
  }else{
    buttonB = 0;
  }

  if(enableJoyPadButton == false){
    if(buttonA == 1){ //ボタン接続されていなければボタンAが0、Bが1のままなので、1度Aが1になれば、以降はボタン接続アリとする。
      enableJoyPadButton = true;
    }else{
      //接続されていない場合は押されてない扱いとする。
      buttonA = 0;
      buttonB = 0;
    }
  }

  if(joyPadPushed_U == false && joyX < 80){
    //テンキーの8を押下
    onkey_down(KEY_TENKEY_8);
    joyPadPushed_U = true;
    delay(60);
  }else if(joyPadPushed_U == true && joyX > 80){
    //テンキーの8を戻す
    onkey_up(KEY_TENKEY_8);
    joyPadPushed_U = false;
  }
  if(joyPadPushed_D == false && joyX > 160){
    //テンキーの2を押下
    onkey_down(KEY_TENKEY_2);
    joyPadPushed_D  =true;
    delay(60);
  }else if(joyPadPushed_D == true && joyX < 160){
    //テンキーの2を戻す
    onkey_up(KEY_TENKEY_2);
    joyPadPushed_D = false;
  }
  if(joyPadPushed_L == false && joyY < 80){
    //テンキーの4を押下
    onkey_down(KEY_TENKEY_4);
    joyPadPushed_L = true;
    delay(60);
  }else if(joyPadPushed_L == true && joyY > 80){
    //テンキーの4を戻す
    onkey_up(KEY_TENKEY_4);
    joyPadPushed_L = false;
  }
  if(joyPadPushed_R == false && joyY > 160){
    //テンキーの6を押下
    onkey_down(KEY_TENKEY_6);
    joyPadPushed_R = true;
    delay(60);
  }else if(joyPadPushed_R == true && joyY < 160){
    //テンキーの6を戻す
    onkey_up(KEY_TENKEY_6);
    joyPadPushed_R = false;
  }
  if(joyPadPushed_A == false && buttonA == 1){
    //スペース押下
    onkey_down(' ');
    joyPadPushed_A = true;
    delay(60);
    onkey_up(' ');
    joyPadPushed_A = false;
  }else if(joyPadPushed_R == true && buttonA == 0){
    //スペースを戻す
    onkey_up(' ');
    joyPadPushed_A = false;
  }
  if(joyPadPushed_B == false && buttonB == 1){
    //GRAPH押下
    onkey_down(KEY_GRAPH);
    joyPadPushed_B = true;
    delay(60);
  }else if(joyPadPushed_B == true && buttonB == 0){
    //GRAPHを戻す
    onkey_up(KEY_GRAPH);
    joyPadPushed_B = false;
  }
  if(joyPadPushed_Press == false && joyPress == 1){
    //Return押下
    onkey_down(0x0D);
    joyPadPushed_Press = true;
    delay(60);
  }else if(joyPadPushed_Press == true && joyPress == 0){
    //Returnを戻す
    onkey_up(0x0D);
    joyPadPushed_Press = false;
  }
*/
}

//--------------------------------------------------------------
// I2C Keyboard Logic
//--------------------------------------------------------------
void checkI2cKeyboard(){
BYTE i2cKeyCode = 0;  
  if(Wire.requestFrom(CARDKB_ADDR, 1)){  // request 1 byte from keyboard
    while (Wire.available()) {
      i2cKeyCode = Wire.read();                  // receive a byte as 
      break;
    }
  }
  if(i2cKeyCode == 0){
    return;
  }
  Serial.println(i2cKeyCode, HEX);

  //特殊キー
  switch(i2cKeyCode){
    case 0xB5:
      i2cKeyCode = 0x12;  //UP
      break;
    case 0xB6:
      i2cKeyCode = 0x11;  //DOWN
      break;
    case 0xB7:
      i2cKeyCode = 0x13;  //RIGHT
      break;
    case 0xB4:
      i2cKeyCode = 0x14;  //LEFT
      break;
    case 0x99: //Fn + UP
      i2cKeyCode = 0x15;  //HOME
      break;
    case 0xA4: //Fn + Down
      i2cKeyCode = 0x16;  //END -> CLR
      break;
    case 0x7F:  //Shift + BS
      i2cKeyCode = 0x18;  //INST
      break;
    case 0x08: //BS
      i2cKeyCode = 0x17;
      break;
    case 0x1B: //ESC->STOP
      i2cKeyCode = 0x1a;
      break;
    //数字キーはテンキーとして扱う
    case 0x30:
      i2cKeyCode = KEY_TENKEY_0;
      break;
    case 0x31:
      i2cKeyCode = KEY_TENKEY_1;
      break;
    case 0x32:
      i2cKeyCode = KEY_TENKEY_2;
      break;
    case 0x33:
      i2cKeyCode = KEY_TENKEY_3;
      break;
    case 0x34:
      i2cKeyCode = KEY_TENKEY_4;
      break;
    case 0x35:
      i2cKeyCode = KEY_TENKEY_5;
      break;
    case 0x36:
      i2cKeyCode = KEY_TENKEY_6;
      break;
    case 0x37:
      i2cKeyCode = KEY_TENKEY_7;
      break;
    case 0x38:
      i2cKeyCode = KEY_TENKEY_8;
      break;
    case 0x39:
      i2cKeyCode = KEY_TENKEY_9;
      break;
    case 0xAA: //mon から抜けるため、Fn + B は Ctrl + B にしておく。
      i2cKeyCode = 0x02;
      break;
    default:
      break;
  }
  keyInput(i2cKeyCode);
}

String chooseN80File(){
  m5lcd.fillScreen(BLACK);
  m5lcd.setCursor(0, 0);
  m5lcd.setTextSize(1);
  File fileRoot;
  String fileList[MAX_FILES];
  String fileDir = "/"; 
  fileRoot = SPIFFS.open(fileDir);
  int fileListCount = 0;
  while (1)
  {
    File entry = fileRoot.openNextFile();
    if (!entry)
    { // no more files
      break;
    }
    //ファイルのみ取得
    if (!entry.isDirectory())
    {
      String fullFileName = entry.name();
      String fileName = fullFileName.substring(fullFileName.lastIndexOf("/") + 1);
      String ext = fileName.substring(fileName.lastIndexOf(".") + 1);
      ext.toUpperCase();
      if(ext.equals("N80")==false && ext.equals("CMT")==false){
        continue;
      }

      fileList[fileListCount] = fileName;
      fileListCount++;
      //Serial.println(fileName);
    }
    entry.close();
  }
  fileRoot.close();


  int startIndex = 0;
  int endIndex = startIndex + 10;
  if (endIndex > fileListCount)
  {
    endIndex = fileListCount;
  }

  sortList(fileList, fileListCount);

  bool needRedraw = true;
  int selectIndex = 0;
  int preStartIndex = 0;
  bool isLongPress = false;
  bool isButtonLongPress = false;
  while (true)
  {
    if (needRedraw == true)
    {
      m5lcd.setCursor(0, 0);
      startIndex = selectIndex - 5;
      if (startIndex < 0)
      {
        startIndex = 0;
      }
      endIndex = startIndex + 24;
      if (endIndex > fileListCount)
      {
        endIndex = fileListCount;
        //startIndex = endIndex - 12;
        startIndex = endIndex - 24;
        if (startIndex < 0) {
          startIndex = 0;
        }
      }
      if (preStartIndex != startIndex) {
        //スクロールで全画面書き換え
        m5lcd.fillScreen(BLACK);
        preStartIndex = startIndex;
      }
      int y = 0;
      for (int index = startIndex; index < endIndex + 1; index++)
      {
        menuCanvas.fillScreen(BLACK);
        menuCanvas.setCursor(0, 0);
        if (index == selectIndex)
        {
          if (isButtonLongPress == true) {
            menuCanvas.setTextColor(TFT_RED);
          } else {
            menuCanvas.setTextColor(TFT_GREEN);
          }
        }
        else
        {
          menuCanvas.setTextColor(TFT_WHITE);
        }
        if (index == 0)
        {
          menuCanvas.println("[BACK]");
        }
        else
        {
          menuCanvas.println(fileList[index - 1].substring(0, 26));
        }
        menuCanvas.pushRotateZoom(320 + 2, 8 * y + 10, 0, 2, 1);
        y++;
      }
      m5lcd.setTextColor(TFT_WHITE);

      m5lcd.drawString("LONG PRESS:SELECT", 0, 200 - 17, 1);
      needRedraw = false;
    }
    M5.update();
    if(M5.Btn.pressedFor(1000)){ //長押しになった場合色を変える
      if( isButtonLongPress == false){
             isButtonLongPress = true; 
            needRedraw = true;
      }
    }
    if (M5.Btn.wasReleasefor(1000))
    {
      if (selectIndex == 0)
      {
        //何もせず戻る
        m5lcd.fillScreen(TFT_BLACK);
        delay(10);
        return "";
      }
      else
      {
        String fileName = fileList[selectIndex - 1];
        String ext = fileName.substring(fileName.lastIndexOf(".") + 1);
        ext.toUpperCase();
        bool setMemoryFlag = false;
        if(ext.equals("N80")==true ){
          setMemoryFlag = true;
        }

        
        Serial.print("select:");
        Serial.println(fileName);
        delay(10);
        m5lcd.fillScreen(TFT_BLACK);
        
        m5lcd.setCursor(0, 0);
        if(setMemoryFlag == true){
          m5lcd.print("SET n80 TO MEMORY:");
        }else{
          m5lcd.print("SET CMT TO CASETTE DATA:");
        }
        m5lcd.print(fileName);
        delay(2000);
        m5lcd.fillScreen(TFT_BLACK);
        delay(10);
        //メモリに読み込む
        if(setMemoryFlag == true){
          selectN80File(fileName);
        }else{
          selectCMTFile(fileName);
        }
        return fileName;
      }
    }else if (M5.Btn.wasReleased()){
      selectIndex++;
      if (selectIndex > fileListCount)
      {
        selectIndex = 0;
      }
      needRedraw = true;
    }
    delay(100);
  }
}

//https://github.com/tobozo/M5Stack-SD-Updater/blob/master/examples/M5Stack-SD-Menu/M5Stack-SD-Menu.ino
void sortList(String fileList[], int fileListCount) {
  bool swapped;
  String temp;
  String name1, name2;
  do {
    swapped = false;
    for (int i = 0; i < fileListCount - 1; i++ ) {
      name1 = fileList[i];
      name1.toUpperCase();
      name2 = fileList[i + 1];
      name2.toUpperCase();
      if (name1.compareTo(name2) > 0) {
        temp = fileList[i];
        fileList[i] = fileList[i + 1];
        fileList[i + 1] = temp;
        swapped = true;
      }
    }
  } while (swapped);
}

#define PCG_INDEX 1
#define TEN_KEY_MODE_INDEX 2
#define MEMORY_SAVE_INDEX 3
void systemMenu()
{
  static String menuItem[] =
  {
    "[BACK]",
    "PCG",
    "TEN KEY MODE",
    "MEMORY SAVE",
    ""
  };
  delay(10);
  m5lcd.fillScreen(BLACK);
  delay(10);
  m5lcd.setTextSize(1);
  bool needRedraw = true;
  bool isButtonLongPress = false;
  int menuItemCount = 0;
  while (menuItem[menuItemCount] != "") {
    menuItemCount++;
  }

  int selectIndex = 0;
  delay(100);
  M5.update();

  while (true)
  {
    if (needRedraw == true)
    {

      int y = 0;
      for (int index = 0; index < menuItemCount; index++)
      {
        menuCanvas.fillScreen(BLACK);
        menuCanvas.setCursor(0, 0);
        if (index == selectIndex)
        {
          if (isButtonLongPress == true) {
              menuCanvas.setTextColor(TFT_RED);
            } else {
              menuCanvas.setTextColor(TFT_GREEN);
            }
        }
        else
        {
          menuCanvas.setTextColor(TFT_WHITE);
        }
        String curItem = menuItem[index];
        if (index == PCG_INDEX) {
          curItem = curItem + (n80device.pcg_mode ? String(": ON") : String(": OFF"));
        }else if(index == TEN_KEY_MODE_INDEX){
          curItem = curItem + (tenkeyMode ? String(": ON") : String(": OFF"));
        }

        menuCanvas.println(curItem);
        menuCanvas.pushRotateZoom(320, 8 * y + 10, 0, 2, 1);
        y++;
      }

      m5lcd.setTextColor(TFT_WHITE);
      m5lcd.drawString("LONG PRESS:SELECT", 0, 200 - 17, 1);
      needRedraw = false;
    }
    M5.update();
    if(M5.Btn.pressedFor(1000)){ //長押しになった場合色を変える
      if( isButtonLongPress == false){
             isButtonLongPress = true; 
            needRedraw = true;
      }
    }

    if (M5.Btn.wasReleasefor(1000))
    {
      if (selectIndex == 0)
      {
        m5lcd.fillScreen(TFT_BLACK);
        delay(10);
        return;
      }
      switch (selectIndex)
      {
        case PCG_INDEX:
          n80device.pcg_mode = (n80device.pcg_mode == 1) ? 0 : 1;
          break;
        case TEN_KEY_MODE_INDEX:
          tenkeyMode = (tenkeyMode == 1) ? 0 : 1;
          break;
        case MEMORY_SAVE_INDEX:
          m5lcd.setTextColor(TFT_WHITE);
          m5lcd.setCursor(0, 0);          
          if(saveN80Image()==true){
             m5lcd.println("SAVE:OK!");
          }else{
            m5lcd.println("SAVE:NG!");
          }
          delay(2000);
          return;
      }
      m5lcd.fillScreen(TFT_BLACK);
      m5lcd.setCursor(0, 0);
      needRedraw = true;
      isButtonLongPress = false;
    }else if (M5.Btn.wasReleased()) {
      selectIndex++;
      if (selectIndex >= menuItemCount)
      {
        selectIndex = 0;
      }
      needRedraw = true;
    }

    delay(100);
  }
}

void gui_msg(const char* msg)         // temporarily display a msg
{
    Serial.println(msg);
    //if(btKeyboardConnect == false){
    //  updateStatusArea(msg);
    //}
}
void sys_msg(const char* msg) {
    Serial.println(msg);
    //if(btKeyboardConnect == false){
   // updateStatusArea(msg);
    //}
}

void gui_hid(const uint8_t* hid, int len)  // Parse HID event
{

    if (hid[0] != 0xA1)
        return;
    /*
    for (int i = 0; i < len; i++)
        printf("%02X",hid[i]);
    printf("\n");
    */
    switch (hid[1]) {
        case 0x01: keyboard(hid+1,len-1);   break;   // parse keyboard and maintain 1 key state
//        case 0x32: wii();                   break;   // parse wii stuff: generic?
//        case 0x42: ir(hid+2,len);           break;   // ir joy
    }

}

static int _last_key = 0;
bool kanaMode = false;
static void keyboard(const uint8_t* d, int len)
{
    updateStatusArea("");
    int mods = d[1];          // can we use hid mods instead of SDL? TODO
    int key_code = d[3];      // only care about first key
    //shift Key
    if((mods & 0b00100010) > 0){
      onkey_down_HID(KEY_SHIFT);
    }else{
      onkey_up_HID(KEY_SHIFT);
    }
    //ctrl Key
    if((mods & 0b00010001) > 0){
      onkey_down_HID(KEY_CTRL);
    }else{
      onkey_up_HID(KEY_CTRL);
    }
    //graph Key(ALTに割り当て)
    if((mods & 0b01000100) > 0){
      onkey_down_HID(KEY_GRAPH);
    }else{
      onkey_up_HID(KEY_GRAPH);
    }

    //カナキーはロックさせる。入力があるごとにON/OFF切り替え。
    if(key_code == 0x87){
      if(kanaMode == false){
        onkey_down_HID(key_code);
        kanaMode = true;
      }else{
        onkey_up_HID(key_code);
        kanaMode = false;
      }
    }else if (key_code != _last_key) {
      //tenkeyModeの場合、カーソルキーをテンキー 2468にする。
      if(tenkeyMode == 1){
          switch(key_code){
            case 0x4F: key_code = 0x5e;break;//right -> 6
            case 0x50: key_code = 0x5c;break;//left  -> 4
            case 0x51: key_code = 0x5a;break;//down  -> 2
            case 0x52: key_code = 0x60;break;//up    -> 8
          }
      }

      if(0 < _last_key && _last_key < 166){
        onkey_up_HID(_last_key);
      }
      if (key_code) {
          _last_key = key_code;
      } else {
          _last_key = 0;
      }
      if(0 < key_code && key_code < 166){
        //カーソル下/左は、カーソル上/右 + シフト
        if(key_code == KEY_DOWN || key_code == KEY_LEFT){
          onkey_down_HID(KEY_SHIFT);
        }
        onkey_down_HID(key_code);
      }
      Serial.printf("mods:%d keyCode:%d\n",mods,key_code);
    }
}

static bool saveN80Image(){
  //上書きで保存
  //http://home1.catvmics.ne.jp/~kanemoto/n80/inside.html
  //N80ファイル形式とはメモリの$8000～$FF3Fまでがダンプされ
  //$FF3D(ファイル中でのオフセットは$7F3D）にはRET命令(0xC9)
  //$FF3E(同オフセット$7F3E)にはSPの値が格納されている
  m5lcd.fillScreen(BLACK);
  File saveFile;
  saveFile = SPIFFS.open("/0_M5N80MEM.N80", FILE_WRITE);
  if(saveFile == false){
    return false;
  }
  BYTE	*mem = (n80device.mem.map._ram);
  m5lcd.println();
  for(int16_t index = 0;index <= 0x7F3C;index++){
    saveFile.write(*(n80device.mem.map._ram + index));
    if(index % 100 == 0){
      m5lcd.print("*");
    }
  }  
  saveFile.write(0xC9);
  saveFile.write(((uint8_t*)&(n80lx.cpu.r.x.SP))[0]);
  saveFile.write(((uint8_t*)&(n80lx.cpu.r.x.SP))[1]);
  saveFile.close();
  m5lcd.println();
  return true;
}

extern "C" void n80_cmt_write_exec(BYTE d) {
  //テープへのセーブも対応してみたけど、たまに落ちる…。
  Serial.printf("save!:%d : ", d);
	File saveCMTFile = SPIFFS.open("/0_M5N80CMT.cmt", "ab");
	if (saveCMTFile) {
		saveCMTFile.write(d);
		saveCMTFile.close();
	}
}
