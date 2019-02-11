//------------------------------------------------------------------------
//     PC-8001 Emulator for M5Stack 2018/11/18 @shikarunochi 
//------------------------------------------------------------------------
#include <M5Stack.h>
#include <M5StackUpdater.h>     // https://github.com/tobozo/M5Stack-SD-Updater/
#include "n80.h"
#include <Preferences.h>

int xorKey = 0x80;

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  delay(1000);
  Wire.begin();
  if(digitalRead(BUTTON_A_PIN) == 0) {
     Serial.println("Will Load menu binary");
     updateFromFS(SD);
     ESP.restart();
  }

  //Cを押下しながら起動された場合は、強制的にアクセスポイントモードにする
  if(digitalRead(BUTTON_C_PIN) == 0) {
    pc80Config.forceAccessPoint = true;
    M5.lcd.println("[C] Pressed : AccessPoint Mode");
    delay(2000);
  }else{
    pc80Config.forceAccessPoint = false;
  }
  Serial.println("Start!");
  m5n80_main();
  saveConfig();
  exit(0);  
}

void loop() {
  // put your main code here, to run repeatedly:

}
//Preferencesはファイルで保持。

void saveConfig(){
  Serial.println("saveConfig");

  String confPath = String(ROM_DIRECTORY) + "/m5n80.conf";
  Serial.println(confPath);
  if (SD.exists(confPath)){
    SD.remove(confPath);
  }
  
  File configFile = SD.open(confPath, FILE_WRITE);
 if (configFile) {
    configFile.print("SSID:");
    configFile.println(pc80Config.ssid);
    configFile.print("PASS:");
    //Serial.println(pc80Config.pass); 
    char xorData[50]={};
    xorEnc(pc80Config.pass, xorData);
    for(int index = 0;index < strnlen(xorData,50);index++){
      configFile.printf("%x", xorData[index]); 
    }
    configFile.println(); 
    configFile.print("N80:");
    configFile.println(pc80Config.n80File);
    configFile.print("CMT:");
    configFile.println(pc80Config.cmtFile);

    configFile.close();
  }else{
    Serial.println("FAIL:saveConfig");   
  }
  delay(100);

}
void loadConfig(){

  String confPath = String(ROM_DIRECTORY) + "/m5n80.conf";
  File configFile = SD.open(confPath, FILE_READ);
  String lineBuf = "";
  if (configFile) {
    while (configFile.available()) {
      char nextChar = char(configFile.read());
      if(nextChar == '\r' || nextChar == '\n'){
        if(lineBuf.length() == 0){ // バッファまだ0文字の場合、何もせずもう一度処理
          continue;
        }else{
          //１行読み終わり
          if(lineBuf.startsWith("SSID:")){
            strncpy(pc80Config.ssid, lineBuf.substring(5).c_str(),sizeof(pc80Config.ssid));
            Serial.print("SSID:");
            Serial.println(pc80Config.ssid);
          }
          if(lineBuf.startsWith("PASS:")){
            char xorData[50] = {};
            String hexData = lineBuf.substring(5);
            for(int index = 0;index < hexData.length() / 2;index++){
              String hexValue = hexData.substring(index * 2, index * 2 + 2);
              xorData[index] = strtol(hexValue.c_str(), NULL, 16);
            }
            xorEnc(xorData, pc80Config.pass);
          }
          if(lineBuf.startsWith("N80:")){
            strncpy(pc80Config.n80File, lineBuf.substring(4).c_str(),sizeof(pc80Config.n80File));
            Serial.print("N80:");
            Serial.println(pc80Config.n80File);
          }
          if(lineBuf.startsWith("CMT:")){
            strncpy(pc80Config.cmtFile, lineBuf.substring(4).c_str(),sizeof(pc80Config.cmtFile));
            Serial.print("CMT:");
            Serial.println(pc80Config.cmtFile);
          }

        }
        lineBuf = "";
      }else{
        lineBuf += nextChar; 
      }
    }
    configFile.close();
  }else{
    Serial.println("ConfigFile not found.");
  }

  //preferences に Wi-Fi Configがあれば、そちらで上書きする。
  Preferences preferences;
  preferences.begin("wifi-config");
  String wifi_ssid = preferences.getString("WIFI_SSID");
  if(wifi_ssid.length() > 0){
    strncpy(pc80Config.ssid, wifi_ssid.c_str(),sizeof(pc80Config.ssid));
  }
  String wifi_password = preferences.getString("WIFI_PASSWD");
  if(wifi_password.length() > 0){
    strncpy(pc80Config.pass, wifi_password.c_str(),sizeof(pc80Config.pass));
  }
  preferences.end();

}

void xorEnc(char fromData[], char toData[] ){
  int index = 0;
  for(index = 0;index < strnlen(fromData, 50);index++) {
    toData[index] = fromData[index] ^ xorKey;
  }
  toData[index+1] = '\0';
}
