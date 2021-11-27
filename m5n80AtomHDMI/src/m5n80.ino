//------------------------------------------------------------------------
//     PC-8001 Emulator for M5Stack 2018/11/18 @shikarunochi 
//------------------------------------------------------------------------
#include <M5Atom.h>  
#include "FS.h"
#include <SPIFFS.h>   // https://github.com/tobozo/M5Stack-SD-Updater/
#include "n80.h"
#include <Preferences.h>
#include "m5n80lgfx.h"
int xorKey = 0x80;
M5AtomDisplay m5lcd(640,200,30); 
void setup() {
  M5.begin(true,false,true); 
  Serial.println("MAIN_START");
  Wire.begin(26,32,100000);

    if (!SPIFFS.begin()) { 
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  m5lcd.init();
  m5lcd.setColorDepth(8);
  
  
  Serial.println("Start!");
  m5n80_main();
  saveConfig();
  exit(0);  

}

void loop() {
  // put your main code here, to run repeatedly:

}

//Preferencesは使わなくなりました。
void saveConfig(){
  return;
}
void loadConfig(){
  return;
}

