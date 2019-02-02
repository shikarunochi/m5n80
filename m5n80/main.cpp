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
 
#include<M5Stack.h>
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
#include "pc-8001.h"
}
#include "n80.h"
#include <WebServer.h>
#include <WiFiMulti.h>
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

boolean needDrawFlag;
boolean suspendMachine;

PC80_CONFIG pc80Config;

WiFiMulti wifiMulti;
WebServer webServer(80);
void * webserver_thread_func(void *arg);
bool webStarted;
bool startWebServer();
bool stopWebServer();
String makePage(String message);
String urlDecode(String input);

String n80ListOptionHTML;
void updateN80List();
void selectN80File(String n80Filename);
void selectCMTFile(String cmtFilename);

#define MAX_FILES 255 // this affects memory
String n80List[MAX_FILES];
int n80ListCount;
void aSortN80List();
String choseN80File();
boolean btnBLongPressFlag;

static void keyInput(BYTE serialKeyCode);

void selectN80();
void selectCMT();
void setWiFi();
void deleteWiFi();
void sendCommand();
void PCGSetting();

static BYTE cmt_buff[1024*32];

enum{
  WEB_C_NONE = 0,
  WEB_C_SELECT_N80,
  WEB_C_SELECT_CMT,
  WEB_C_SEND_COMMAND
};

int webCommand;
String webCommandParam;

String cmtListOptionHTML;
void updateCMTList();

static bool intFlag = false;

#define SP_VAL      (0x7f3e)
#define PC_VAL      (0xff3d)

int buttonApin = 26; //赤ボタン
int buttonBpin = 36; //青ボタン

bool joyPadPushed_U;
bool joyPadPushed_D;
bool joyPadPushed_L;
bool joyPadPushed_R;
bool joyPadPushed_A;
bool joyPadPushed_B;
bool joyPadPushed_Press;

void checkJoyPad();

#define CARDKB_ADDR 0x5F
void checkI2cKeyboard();

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
// Webからのコマンド処理
//--------------------------------------------------------------
static void processWebCommand(void)
{
  if(webCommand == WEB_C_NONE){
    return;
  }
  
  switch(webCommand){
    case WEB_C_SELECT_N80:
    {
      strncpy(pc80Config.n80File, webCommandParam.c_str(), 50);
      selectN80File(webCommandParam);
      saveConfig();
      webServer.send(200, "text/html", makePage("Set N80[ "+ webCommandParam +"] Done"));
      break;
    }

    case WEB_C_SELECT_CMT:
    {
      strncpy(pc80Config.cmtFile, webCommandParam.c_str(), 50);
      selectCMTFile(webCommandParam);
      saveConfig();
      webServer.send(200, "text/html", makePage("Set CMT[ "+ webCommandParam +"] Done"));
      break;
    }
    
    case WEB_C_SEND_COMMAND:
    {
      String inputStringEx = webCommandParam;
      for(int index=0;index < inputStringEx.length();index++){
        keyInput(inputStringEx.charAt(index));
        delay(100);      
      }
      keyInput(0x0d);
      break;
    }

  }

  webCommand = WEB_C_NONE;
  webCommandParam = "";
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

    //Web経由のコマンド実行
    processWebCommand();
    
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
    M5.update();
    if(M5.BtnA.wasPressed()){
        // toggle PCG mode ON/OFF
        n80device.pcg_mode ^= 0x01;
        n80device.attr.invalid();
    }

    if(M5.BtnB.wasReleased()){
      if(btnBLongPressFlag != true){
        if(strlen(pc80Config.n80File) != 0){
          selectN80File(String(pc80Config.n80File));
        }
      }else{
        btnBLongPressFlag = false;
      }
    }
    if(M5.BtnB.pressedFor(2000)){
      if(btnBLongPressFlag != true){
        btnBLongPressFlag = true;
        suspendMachine = true;
        delay(100);
        choseN80File();
        delay(100);
        selectN80File(String(pc80Config.n80File)); //この中でSuspendは戻る
      }
    }          
    if(M5.BtnC.wasPressed()){ 
      if(webStarted){
        stopWebServer();
      }else{
        startWebServer();
      }
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

  pthread_t webTh;
  Serial.println("create Web thread");
  if (pthread_create(&webTh, NULL, webserver_thread_func, NULL/*&disp2main*/) != 0) {
      Serial.println("can not create Web thread");
      return 0;
  }

  n80main();

  pthread_join(webTh, NULL);
  pthread_join(disp, NULL);
  pthread_join(th, NULL);

}

int main_core()
{
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

  // rom file load
  romFile = SD.open(String(ROM_DIRECTORY) + "/PC-8001.ROM", FILE_READ);
  if(!romFile){
    romFile = SD.open(String(ROM_DIRECTORY) + "/8801-N80.ROM", FILE_READ);
    if(!romFile){
      Serial.println("can not open ROM file");
      return 1;
    }
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
    return 1;
  }

  updateCMTList();
  updateN80List();

  webCommand = WEB_C_NONE;
  webCommandParam = "";

//For JoyPad
  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);
  joyPadPushed_U = false;
  joyPadPushed_D = false;
  joyPadPushed_L = false;
  joyPadPushed_R = false;
  joyPadPushed_A = false;
  joyPadPushed_B = false;
  joyPadPushed_Press = false;

  btnBLongPressFlag = false;

// save cmt path
  sprintf(n80device.save_cmt_path, "%s/cassette.cmt", CMT_DIRECTORY);
  printf("%s\n", n80device.save_cmt_path);

if(strlen(pc80Config.cmtFile) != 0){
  selectCMTFile(String(pc80Config.cmtFile));
  Serial.print("SetCMTFile:");
  Serial.println(pc80Config.cmtFile);
}

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
        n80File = SD.open(n80FilePath , FILE_READ);
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
        Serial.println("CMT LOAD Start!");
        suspendMachine = true;
        delay(1000);

        File cmtFile;

        String cmtFilePath = String(CMT_DIRECTORY) + "/" + cmtFilename;
        cmtFile = SD.open(cmtFilePath , FILE_READ);
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
          cmtFile.close();
        }else{
          Serial.println("File not found");
        }
        suspendMachine = false;      
}

//--------------------------------------------------------------
// Web処理スレッド 
//--------------------------------------------------------------
void * webserver_thread_func(void *arg)
{ 
  long synctime = 500;
  long timeTmp;
  long vsyncTmp;

  while(!intByUser())
  {
    timeTmp = millis();
    if(webStarted == true){
      webServer.handleClient();
    }else{

    }
    vsyncTmp = millis();
    if(synctime - (vsyncTmp - timeTmp) > 0){
      delay(synctime - (vsyncTmp - timeTmp));
    }else{
      delay(1);
    }
  }
  return NULL;
}

//--------------------------------------------------------------
// JoyPadLogic
//--------------------------------------------------------------

void checkJoyPad(){
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
}


//--------------------------------------------------------------
// WebServerLogic
//--------------------------------------------------------------

bool startWebServer(){
  suspendMachine = true;
  delay(100);
  if(strlen(pc80Config.ssid) == 0 || pc80Config.forceAccessPoint == true){
    //アクセスポイントとして動作
    String message = "Starting Access Point...";
    updateStatusArea(message.c_str());
    const IPAddress apIP(192, 168, 4, 1);
    int randNumber = random(99999);
    String adSSIDString = "M5N80_" + String(randNumber);
    const char* apSSID = adSSIDString.c_str();
    Serial.print("Access Point Setting");
    Serial.print("SSID:" + String(apSSID));
    Serial.print("IP Address:" + String(apIP));

    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect(true);
    delay(1000);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(apSSID);
    WiFi.mode(WIFI_MODE_AP);
    message = "[SSID:" + String(apSSID) + "] http://" + apIP.toString();
    updateStatusArea(message.c_str());
  }else{
    String message = "Starting WebServer...";
    updateStatusArea(message.c_str());
    WiFi.disconnect(true);
    delay(1000);
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(pc80Config.ssid, pc80Config.pass);
    Serial.println("Connecting Wifi...");
    Serial.print("Web Server Setting");
    Serial.print("SSID:" + String(pc80Config.ssid));
    if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }else{
      message = "Connection Fail:" + String(pc80Config.ssid);
      updateStatusArea(message.c_str());
      suspendMachine = false;
      return false;
    }
    
    Serial.println();
    Serial.println("WiFi connected");
    //M5.Lcd.println("Wi-Fi Connected");
    delay(1000);
    Serial.println(WiFi.localIP());
    
    message = "http://" + WiFi.localIP().toString();
    updateStatusArea(message.c_str());
  }  

    webServer.on("/selectN80", selectN80);
    webServer.on("/selectCMT",selectCMT);
  webServer.on("/sendCommand", sendCommand) ;
  //webServer.on("/sendBreakCommand", sendBreakCommand) ;
  webServer.on("/setWiFi", setWiFi);
  webServer.on("/deleteWiFi",deleteWiFi);
  //webServer.on("/soundSetting",soundSetting);
  webServer.on("/PCGSetting",PCGSetting);
  
  webServer.on("/", []() {
    webServer.send(200, "text/html", makePage(""));
  });

  delay(100);
  webServer.begin();
  webStarted = true;
  suspendMachine = false;
  return true;
}

bool stopWebServer(){
  suspendMachine = true;
  delay(100);
  Serial.println("WebServer:Stop");
  webStarted = false;
  webServer.stop();
  //webServer.close();
  Serial.println("WiFi:Disconnect");

  WiFi.disconnect(true); 
  updateStatusArea("");
  
  suspendMachine = false;
  
}

void selectN80(){
    webCommand = WEB_C_SELECT_N80;
    webCommandParam =  urlDecode(webServer.arg("n80File"));
}

void selectCMT(){
    webCommand = WEB_C_SELECT_CMT;
    webCommandParam =  urlDecode(webServer.arg("cmtFile"));
}

void sendCommand(){
  String commandString = urlDecode(webServer.arg("commandString"));
  webCommandParam = commandString;
  webCommand = WEB_C_SEND_COMMAND;
  webServer.send(200, "text/html", makePage("sendCommand[" + commandString + "] Done"));
}

void PCGSetting(){
  String enablePCG = urlDecode(webServer.arg("enablePCG"));
  if(enablePCG.toInt()==1){
      n80device.pcg_mode = 1;
  }else{
      n80device.pcg_mode = 0;
  }
  n80device.attr.invalid();
  String message = String("Set PCG[ ") + ((n80device.pcg_mode == 1)?String("ON"):String("OFF"))  + String(" ] Done.");
  webServer.send(200, "text/html", makePage(message));
}

void setWiFi(){
  String ssid = urlDecode(webServer.arg("ssid"));
  String pass = urlDecode(webServer.arg("pass"));
  strncpy(pc80Config.ssid, ssid.c_str(), 50);
  strncpy(pc80Config.pass, pass.c_str(), 50);
  
  webServer.send(200, "text/html", makePage("Set SSID[ "+ ssid +"] and Pass Done."));
  delay(100);
  pc80_exit(0);//再起動する。設定のセーブはプログラム終了時に行われる
}

void deleteWiFi(){
  strncpy(pc80Config.ssid, "", 50);
  strncpy(pc80Config.pass, "", 50);

  webServer.send(200, "text/html", makePage("Delete SSID and Pass Done."));
  delay(100);
  pc80_exit(0);//再起動する。設定のセーブはプログラム終了時に行われる
}


String makePage(String message) {
String s = "<!DOCTYPE html><meta charset=\"UTF-8\" /><meta name=\"viewport\" content=\"width=device-width\"><html><head>" 
"<title>PC-8001 for M5Stack Setting</title>"
"</head>" 
"<body onLoad=\"document.commandForm.commandString.focus()\">" 
"<button onClick=\"location.href='/'\">Reload</button><hr/>";
if (message.length() > 0){
  s+= "<span style='color:red;'>" + message + "</span><hr/>";
}
s += "<span>SD/PC80ROM/N80/*.N80</span>" 
"<form action='/selectN80' method='post'>" 
"<label>N80: </label><select name='n80File'>";
s += n80ListOptionHTML;
s += "</select>" 
"<input type='submit' value='SELECT N80'>" 
"</form>" 
"<hr/>" 
"<span>SD/PC80ROM/CMT/*</span>" 
"<form action='/selectCMT' method='post'>" 
"<label>CMT: </label><select name='cmtFile'>";
s += cmtListOptionHTML;
s += "</select>"
"<input type='submit' value='SELECT TAPE'>" 
"</form>"
"<hr/>" 
"<form name='commandForm' action='/sendCommand' method='post'>" 
"<input type='text' name='commandString'>" 
"<input type='submit' value='SEND COMMAND'>" 
"</form>" 
"<hr/>"
"<div style='display:inline-flex'>"
"<form action='/PCGSetting' method='post'>" 
"<input type='hidden' name='enablePCG' value='1'>"
"<input type='submit' value='PCG ON'>" 
"</form> "
"<form action='/PCGSetting' method='post'>" 
"<input type='hidden' name='enablePCG' value='0'>"
"<input type='submit' value='PCG OFF'>" 
"</form></div>" ;

s += "<hr/>";
s += "N80:" + String(pc80Config.n80File) + "<br/>";
s += "CMT:" + String(pc80Config.cmtFile) + "<br/>";
//s += "PCG:" + ((hw700.pcg700_mode == 1)?String("ON"):String("OFF"))  + "<br/>";
//s += "MZ MODE:" 

s +="<br/><br/><hr/>"
"<form action='/setWiFi' method='post'>" 
"SSID<input type='text' name='ssid'>" 
"<br/>" 
"PASSWORD<input type='text' name='pass'>" 
"<input type='submit' value='SET Wi-Fi SSID Info and REBOOT'>"
"</form>" 
"<hr/>"
"<form action='/deleteWiFi' method='post'>" 
"<input type='submit' value='DELETE Wi-Fi SSID Info and REBOOT'>" 
"</form>" ;

s += "</body>" 
"</html>" ;
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}

void updateN80List(){
  File n80Root;
  delay(100);
  n80Root = SD.open(N80_DIRECTORY);

  n80ListOptionHTML = "";
  n80ListCount = 0;
  while(1){
    File entry =  n80Root.openNextFile();
    if(!entry){// no more files
        break;
    }
    //ファイルのみ取得
    if (!entry.isDirectory()) {
        String fileName = entry.name();
        String n80Name = fileName.substring(fileName.lastIndexOf("/") + 1);
        String checkName = n80Name;
        checkName.toUpperCase();
        if(checkName.endsWith(".N80")){
          n80List[n80ListCount] = n80Name;
          n80ListCount++;
        }
    }
    entry.close();
  }
  n80Root.close();
  
  aSortN80List();

  for(int index = 0;index < n80ListCount;index++){
    n80ListOptionHTML += "<option value=\"";
    n80ListOptionHTML += n80List[index];
    n80ListOptionHTML += "\">";
    n80ListOptionHTML += n80List[index];
    n80ListOptionHTML += "</option>";
  }
}
void updateCMTList(){
  File cmtRoot;
  delay(100);
  cmtRoot = SD.open(CMT_DIRECTORY);

  cmtListOptionHTML = "";
  while(1){
    File entry =  cmtRoot.openNextFile();
    if(!entry){// no more files
        break;
    }
    //ファイルのみ取得
    if (!entry.isDirectory()) {
        String fileName = entry.name();
        String cmtName = fileName.substring(fileName.lastIndexOf("/") + 1);
        
        cmtListOptionHTML += "<option value=\"";
        cmtListOptionHTML += cmtName;
        cmtListOptionHTML += "\">";
        cmtListOptionHTML += cmtName;
        cmtListOptionHTML += "</option>";
    }
    entry.close();
  }
  cmtRoot.close();
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
    default:
      break;
  }
  keyInput(i2cKeyCode);
}

String choseN80File(){
  delay(10);
  M5.Lcd.fillScreen(TFT_BLACK);
  delay(10);
  M5.Lcd.setTextSize(2);
  String curN80File  = String(pc80Config.n80File);
  int selectIndex = 0;
  for(int index = 0;index < n80ListCount;index++){
    if(n80List[index].compareTo(curN80File)==0){
      selectIndex = index;
      break;
    }
  }
  
  int startIndex = 0;
  int endIndex = startIndex + 10;
  if(endIndex > n80ListCount){
    endIndex = n80ListCount;
  }
  //Serial.print("start:");
  //Serial.println(startIndex);
  //Serial.println("end:");
  //Serial.println(endIndex);
  boolean needRedraw = true;
  while(true){

    if(needRedraw == true){
      M5.Lcd.fillScreen(0);
      M5.Lcd.setCursor(0,0);
      startIndex = selectIndex - 5;
      if(startIndex < 0){
        startIndex = 0;
      }

      endIndex = startIndex + 13;
      if(endIndex > n80ListCount -1){
        endIndex = n80ListCount -1;
      }

      for(int index = startIndex;index < endIndex;index++){
          if(index == selectIndex){
             M5.Lcd.setTextColor(TFT_GREEN);
          }else{
            M5.Lcd.setTextColor(TFT_WHITE);
          }
          M5.Lcd.println(n80List[index]);
      }
      M5.Lcd.setTextColor(TFT_WHITE);

      M5.Lcd.drawRect(0, 240 - 19, 100 , 18, TFT_WHITE);
      M5.Lcd.setCursor(35, 240 - 17);
      M5.Lcd.print("U P");
      M5.Lcd.drawRect(110, 240 - 19, 100 , 18, TFT_WHITE);
      M5.Lcd.setCursor(125, 240 - 17);
      M5.Lcd.print("SELECT");
      M5.Lcd.drawRect(220, 240 - 19, 100 , 18, TFT_WHITE);
      M5.Lcd.setCursor(245, 240 - 17);
      M5.Lcd.print("DOWN");
      needRedraw = false;
    }
    M5.update();
    if(M5.BtnA.wasPressed()){
      selectIndex--;
      if(selectIndex < 0){
        selectIndex = 0;
      }
      needRedraw = true;
    }

    if(M5.BtnC.wasPressed()){
      selectIndex++;
      if(selectIndex > n80ListCount - 1){
        selectIndex = n80ListCount -1;
      }
      needRedraw = true;
    }
    
    if(M5.BtnB.wasPressed()){
      Serial.print("select:");
      Serial.println(n80List[selectIndex]);
      delay(10);
      M5.Lcd.fillScreen(TFT_BLACK);

      strncpy(pc80Config.n80File, n80List[selectIndex].c_str(), 50);
      saveConfig();
      M5.Lcd.setCursor(0,0);
      M5.Lcd.print("Set:");
      M5.Lcd.print(n80List[selectIndex]);
      delay(2000);
      M5.Lcd.fillScreen(TFT_BLACK);
      delay(10);
      return n80List[selectIndex];
    }    
    delay(100);
  }
}


/* bubble sort filenames */
//https://github.com/tobozo/M5Stack-SD-Updater/blob/master/examples/M5Stack-SD-Menu/M5Stack-SD-Menu.ino
void aSortN80List() {
  
  bool swapped;
  String temp;
  String name1, name2;
  do {
    swapped = false;
    for(uint16_t i=0; i<n80ListCount-1; i++ ) {
      name1 = n80List[i];
      name1.toUpperCase();
      name2 = n80List[i+1];
      name2.toUpperCase();
      if (name1.compareTo(name2) > 0) {
        temp = n80List[i];
        n80List[i] = n80List[i+1];
        n80List[i+1] = temp;
        swapped = true;
      }
    }
  } while (swapped);
}
