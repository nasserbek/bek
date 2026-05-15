// setup.h

#ifndef SETUP_H
#define SETUP_H
int card = 0;

tm printLocalTime() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");

    // return empty structure
    struct tm emptyTime = {};
    return emptyTime;
  }

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  return timeinfo;
}

 
void relaySetup(void)
{
      pinMode(AV_RX_DVR_PIN_2, OUTPUT);
      delay(50); // let signals stabilize
      digitalWrite(AV_RX_DVR_PIN_2, LOW);  // AV RECEIVER OFF POWER UP NC CONTACT

      pinMode(I2C_1_2_RELAY , OUTPUT);
      pinMode(I2C_3_4_RELAY , OUTPUT);

      pinMode(DIP1, INPUT_PULLUP);
      pinMode(DIP2, INPUT_PULLUP);
      delay(200); // let signals stabilize
      // Read switches
      int b0 = !digitalRead(DIP1); // invert because pullup
      int b1 = !digitalRead(DIP2);
    
      // Convert to mode number
      card = (b1 << 1) | b0;
    
      Serial.print("Card Selecter: ");
      Serial.println(card);

      // Configure time
     configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

     struct tm now = printLocalTime();
      
     Serial.print("Hour: ");        Serial.println(now.tm_hour);
     Serial.print("Minute: ");      Serial.println(now.tm_min);
     Serial.print("Month: ");       Serial.println(now.tm_mon);
     Serial.print("Day: ");         Serial.println(now.tm_mday);
     Serial.print("Year: ");        Serial.println(now.tm_year);
     
      switch(card) {
        case 0:
          ActiveBoard = ESP1;
                            ActiveBoard == ESP1 ;
                            BOARD ="ESP1";
                            
                            
                            BLYNK_AUTH_TOKEN                = BLYNK_AUTH_TOKEN_ESP1;  //ESP1
                            THINGNAME ="ESP1"   ;
                            gitHubURL  = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin" ; // URL to download the firmware from
                            
                            #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "esp1/sub/rc"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "esp1/sub/video"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "esp1/sub/zap"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "esp1/sub/rx"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "esp1/sub/avrc"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "esp1/sub/dvr"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "esp1/sub/reboot"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "esp1/sub/zapchanel"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "esp1/sub/localWebOta"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "esp1/sub/GitHubWebOta"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "esp1/sub/ideOta"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "esp1/sub/version"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "esp1/sub/scan"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "esp1/sub/repeat"   
                            #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "esp1/sub/preset"  
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "esp1/sub/zapauto"  
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "esp1/sub/zton"  
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "esp1/sub/ztoff" 
                            #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "esp1/sub/live"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "esp1/sub/blynk"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "esp1/sub/terminal"
          break;
    
        case 1:
                            ActiveBoard = ESP2;
                            BOARD ="ESP2";
                            VERSION_ID =" ESP2 ";
                            BLYNK_AUTH_TOKEN                = BLYNK_AUTH_TOKEN_ESP2 ; //ESP2
                            THINGNAME ="ESP2"  ; 
                            gitHubURL  = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin" ; // URL to download the firmware from          
                            
                            #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "esp2/sub/rc"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "esp2/sub/video"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "esp2/sub/zap"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "esp2/sub/rx"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "esp2/sub/avrc"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "esp2/sub/dvr"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "esp2/sub/reboot"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "esp2/sub/zapchanel"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "esp2/sub/localWebOta"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "esp2/sub/GitHubWebOta"   
                            #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "esp2/sub/ideOta" 
                            #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "esp2/sub/version"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "esp2/sub/scan"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "esp2/sub/repeat"   
                            #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "esp2/sub/preset"  
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "esp2/sub/zapauto"  
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "esp2/sub/zton"  
                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "esp2/sub/ztoff" 
                            #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "esp2/sub/live"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "esp2/sub/blynk"
                            #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "esp2/sub/terminal"
          break;
    
        case 2:
                            ActiveBoard = ESP3;
                            BOARD ="ESP3";
                            VERSION_ID =" ESP3 ";
                            BLYNK_AUTH_TOKEN         =        BLYNK_AUTH_TOKEN_ESP3  ; //ESP3
                            THINGNAME ="ESP3"  ; 
                            gitHubURL = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin" ; // URL to download the firmware from          
                            
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "esp3/sub/rc"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "esp3/sub/video"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "esp3/sub/zap"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "esp3/sub/rx"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "esp3/sub/avrc"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "esp3/sub/dvr"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "esp3/sub/reboot"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "esp3/sub/zapchanel"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "esp3/sub/localWebOta"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "esp3/sub/GitHubWebOta" 
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "esp3/sub/ideOta"   
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "esp3/sub/version" 
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "esp3/sub/scan"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "esp3/sub/repeat"   
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "esp3/sub/preset"  
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "esp3/sub/zapauto"  
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "esp3/sub/zton"  
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "esp3/sub/ztoff" 
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "esp3/sub/live"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "esp3/sub/blynk"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "esp3/sub/terminal"
          break;
    
        case 3:
                            ActiveBoard = TEST4;
                            BOARD ="ESP0";
                            VERSION_ID =" ESP0 - ";
                            BLYNK_AUTH_TOKEN   =              BLYNK_AUTH_TOKEN_TEST ;
                            THINGNAME ="ESP14"   ;
                            gitHubURL  ="https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin"  ;// URL to download the firmware from          
//
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "test/sub/rc"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "test/sub/video"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "test/sub/zap"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "test/sub/rx"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "test/sub/avrc"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "test/sub/dvr"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "test/sub/reboot"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "test/sub/zapchanel"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "test/sub/localWebOta"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "test/sub/GitHubWebOta"   
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "test/sub/ideOta" 
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "test/sub/version"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "test/sub/scan"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "test/sub/repeat"   
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "test/sub/preset"  
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "test/sub/zapauto"  
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "test/sub/zton"  
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "test/sub/ztoff"   
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "test/sub/live"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "test/sub/blynk"
//                            #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "test/sub/terminal"
           break;
      }

     VERSION_ID = String(BOARD) + " " +
             String(now.tm_mday) + "/" +
             String(now.tm_mon + 1) + "/" +
             String(now.tm_year ) + " " +
             String(now.tm_hour) + ":" +
             String(now.tm_min);
 
}


bool blynkInit(void)
{
   StaticJsonDocument<54> doc; //Json to send from
     blynkConnected = myBlynk.init();    
     String str = WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI());
     if (blynkConnected) 
              {
                myBlynk.sendAvRxIndex(Av_Rx);
                myBlynk.streamSelect(streamWebDdns);
                dvrOnOff (1);
                int rssi = WiFi.RSSI();
                myBlynk.wifiRSSI(WiFi.RSSI());
                myBlynk.sendVersion(VERSION_ID + WiFi.SSID()  );
                myBlynk.TerminalPrint(str );
             }
    
    awsTerminal(awsConnected, str ) ;
return  blynkConnected;
}     

void timersMillis(void)
{
    internetSurvilanceTimer = millis();
    liveTimerOff            = millis();
    OtaTimeoutTimer            = millis();
    ackTimer                = millis();
    restartAfterResetNG     = millis();
    NetgeerResetGooglLostTimer = millis();
    blynkNotActiveTimer     = millis();
    routerResetTimer        = millis();
    resetNetgeerAfterInternetLossTimer = millis();
    Router_24_hoursTimer       = millis();
}

void i2cSetup(void)
{
   bool ta9548a = false;
     Wire.begin();
     delay(500);
     Wire1.begin(SDA_2, SCL_2);

 if(ActiveBoard == ESP1 )
 {      
     ta9548a = !TCA9548A(0);
     DEBUG_PRINTLN ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
     myBlynk.TerminalPrint ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
 }    
}

void resetBoardID(void)
  {       
       if(ActiveBoard == ESP1 )    
       {
          V_Remote_ESP1=false;
          RC_Remote_ESP1=false;
       } 
       
       if(ActiveBoard == ESP2 ) 
       {   
          V_Remote_ESP2=false;
          RC_Remote_ESP2=false;
       } 
       
       if(ActiveBoard == ESP3 ) 
       { 
          V_Remote_ESP3=false;
          RC_Remote_ESP3=false;
       }  
  }
       

#endif
