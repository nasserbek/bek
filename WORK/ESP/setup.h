// setup.h

//#ifndef SETUP_H
//#define SETUP_H

#pragma once

void BoardDefines()
{
  if(ActiveBoard == ESP1)      //TTGO R64 SCATOLA 1CH TTGO
  {
      esp.BOARD = ESP1;
      esp.VERSION_ID  = " ESP1 v0.11 - ";
      esp.BLYNK_AUTH_TOKEN = BLYNK_AUTH_TOKEN_ESP1 ;
      esp.THINGNAME  = "ESP1"   ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RC       = "esp1/sub/rc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VIDEO    = "esp1/sub/video";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAP      = "esp1/sub/zap";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RX       = "esp1/sub/rx";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_AV_RC    = "esp1/sub/avrc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_DVR      = "esp1/sub/dvr";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REBOOT   = "esp1/sub/reboot";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH    = "esp1/sub/zapchanel";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA    = "esp1/sub/localWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA    = "esp1/sub/GitHubWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA    = "esp1/sub/ideOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VERSION    = "esp1/sub/version";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_SCAN    = "esp1/sub/scan";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REPEAT    = "esp1/sub/repeat" ;  
      esp.AWS_IOT_SUBSCRIBE_TOPIC_PRESET   = "esp1/sub/preset"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO    = "esp1/sub/zapauto" ; 
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON    = "esp1/sub/zton"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF    = "esp1/sub/ztoff" ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LIVE    = "esp1/sub/live";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_BLYNK    = "esp1/sub/blynk";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL    = "esp1/sub/terminal";
      esp.gitHubURL   = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP1.ino.esp32.bin" ;
    
  }
  
  
  if(ActiveBoard == ESP2)      // R65 SCATOLA 4CH ESP32S
  {
      esp.BOARD = ESP2;
      esp.VERSION_ID  = " ESP2_1 - ";
      esp.BLYNK_AUTH_TOKEN = BLYNK_AUTH_TOKEN_ESP2 ;
      esp.THINGNAME  = "ESP2"   ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RC       = "esp2/sub/rc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VIDEO    = "esp2/sub/video";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAP      = "esp2/sub/zap";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RX       = "esp2/sub/rx";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_AV_RC    = "esp2/sub/avrc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_DVR      = "esp2/sub/dvr";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REBOOT   = "esp2/sub/reboot";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH    = "esp2/sub/zapchanel";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA    = "esp2/sub/localWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA    = "esp2/sub/GitHubWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA    = "esp2/sub/ideOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VERSION    = "esp2/sub/version";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_SCAN    = "esp2/sub/scan";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REPEAT    = "esp2/sub/repeat" ;  
      esp.AWS_IOT_SUBSCRIBE_TOPIC_PRESET   = "esp2/sub/preset"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO    = "esp2/sub/zapauto" ; 
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON    = "esp2/sub/zton"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF    = "esp2/sub/ztoff" ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LIVE    = "esp2/sub/live";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_BLYNK    = "esp2/sub/blynk";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL    = "esp2/sub/terminal";
      esp.gitHubURL   = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP2.ino.esp32.bin"  ;
 
   }
  
  
  
  if(ActiveBoard == ESP3)      //R66 SWAN CASE 2CH ESP32S
  {
      esp.BOARD = ESP3;
      esp.VERSION_ID  = " ESP3_1 - ";
      esp.BLYNK_AUTH_TOKEN = BLYNK_AUTH_TOKEN_ESP3 ;
      esp.THINGNAME  = "ESP3"   ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RC       = "esp3/sub/rc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VIDEO    = "esp3/sub/video";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAP      = "esp3/sub/zap";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RX       = "esp3/sub/rx";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_AV_RC    = "esp3/sub/avrc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_DVR      = "esp3/sub/dvr";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REBOOT   = "esp3/sub/reboot";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH    = "esp3/sub/zapchanel";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA    = "esp3/sub/localWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA    = "esp3/sub/GitHubWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA    = "esp3/sub/ideOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VERSION    = "esp3/sub/version";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_SCAN    = "esp3/sub/scan";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REPEAT    = "esp3/sub/repeat" ;  
      esp.AWS_IOT_SUBSCRIBE_TOPIC_PRESET   = "esp3/sub/preset"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO    = "esp3/sub/zapauto" ; 
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON    = "esp3/sub/zton"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF    = "esp3/sub/ztoff" ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LIVE    = "esp3/sub/live";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_BLYNK    = "esp3/sub/blynk";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL    = "esp3/sub/terminal";
      esp.gitHubURL   = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP3.ino.esp32.bin"  ;
 
   }  
  
  
  if(ActiveBoard == ESP0)      //R66 SWAN CASE 2CH ESP32S
  {    
      esp.BOARD = ESP0;
      esp.VERSION_ID  = " ESP0_1 - ";
      esp.BLYNK_AUTH_TOKEN = BLYNK_AUTH_TOKEN_ESP0 ;
      esp.THINGNAME  = "ESP0"   ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RC       = "esp0/sub/rc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VIDEO    = "esp0/sub/video";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAP      = "esp0/sub/zap";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_RX       = "esp0/sub/rx";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_AV_RC    = "esp0/sub/avrc";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_DVR      = "esp0/sub/dvr";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REBOOT   = "esp0/sub/reboot";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH    = "esp0/sub/zapchanel";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA    = "esp0/sub/localWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA    = "esp0/sub/GitHubWebOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA    = "esp0/sub/ideOta";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_VERSION    = "esp0/sub/version";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_SCAN    = "esp0/sub/scan";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_REPEAT    = "esp0/sub/repeat" ;  
      esp.AWS_IOT_SUBSCRIBE_TOPIC_PRESET   = "esp0/sub/preset"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO    = "esp0/sub/zapauto" ; 
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON    = "esp0/sub/zton"  ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF    = "esp0/sub/ztoff" ;
      esp.AWS_IOT_SUBSCRIBE_TOPIC_LIVE    = "esp0/sub/live";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_BLYNK    = "esp0/sub/blynk";
      esp.AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL    = "esp0/sub/terminal";
      esp.gitHubURL   = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP0.ino.esp32.bin"  ;
   } 
}

 
void relaySetup(void)
{
     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     pinMode(I2C_1_2_RELAY , OUTPUT);
     pinMode(I2C_3_4_RELAY , OUTPUT);
     pinMode(BOARD_SEL_0 , INPUT); 
     pinMode(BOARD_SEL_1 , INPUT);
     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // AV RECEIVER OFF POWER UP NC CONTACT
     int activeBoard = ( (digitalRead(BOARD_SEL_1) << 1) |   digitalRead(BOARD_SEL_0)) +1;
     switch (activeBoard)
        {
          case 0:
             ActiveBoard = ESP0;
          break;       
             
          case 1:
             ActiveBoard = ESP1;
          break;

          case 2:
             ActiveBoard = ESP2;
          break;    
                         
          case 3:
             ActiveBoard = ESP3;
          break;   

          case 4:
             ActiveBoard = TEST4;
          break;             
        } 
     BoardDefines(); 
}

 void LillyGo_Relay_8_Setup()
{
  pinMode(RELAY_PIN_1, OUTPUT);
    pinMode(RELAY_PIN_2, OUTPUT);
    pinMode(RELAY_PIN_3, OUTPUT);
    pinMode(RELAY_PIN_4, OUTPUT);
    pinMode(RELAY_PIN_5, OUTPUT);
    pinMode(RELAY_PIN_6, OUTPUT);
    pinMode(RELAY_PIN_7, OUTPUT);
    pinMode(RELAY_PIN_8, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    delay(100);

    //Turn off all relays
    digitalWrite(RELAY_PIN_1, LOW);
    digitalWrite(RELAY_PIN_2, LOW);
    digitalWrite(RELAY_PIN_3, LOW);
    digitalWrite(RELAY_PIN_4, LOW);
    digitalWrite(RELAY_PIN_5, LOW);
    digitalWrite(RELAY_PIN_6, LOW);
    digitalWrite(RELAY_PIN_7, LOW);
    digitalWrite(RELAY_PIN_8, LOW);
    digitalWrite(LED_PIN, LOW);

//Turn the relays on and off in turn
    digitalWrite(RELAY_PIN_1, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN_2, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN_3, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN_4, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN_5, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN_6, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN_7, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN_8, HIGH);
    delay(100);

    digitalWrite(RELAY_PIN_1, LOW);
    delay(100);
    digitalWrite(RELAY_PIN_2, LOW);
    delay(100);
    digitalWrite(RELAY_PIN_3, LOW);
    delay(100);
    digitalWrite(RELAY_PIN_4, LOW);
    delay(100);
    digitalWrite(RELAY_PIN_5, LOW);
    delay(100);
    digitalWrite(RELAY_PIN_6, LOW);
    delay(100);
    digitalWrite(RELAY_PIN_7, LOW);
    delay(100);
    digitalWrite(RELAY_PIN_8, LOW);
    delay(100);  
} 

bool blynkInit(void)
{
   StaticJsonDocument<54> doc; //Json to send from
     blynkConnected = myBlynk.init();    
     if (blynkConnected) 
              {
                myBlynk.sendAvRxIndex(Av_Rx);
                myBlynk.streamSelect(streamWebDdns);
                dvrOnOff (1);
                int rssi = WiFi.RSSI();
                myBlynk.wifiRSSI(WiFi.RSSI());
                myBlynk.sendVersion(esp.VERSION_ID + WiFi.SSID()  );
             }
    String str = WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI());
    myBlynk.TerminalPrint(str );
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

 if(esp.BOARD == ESP1 )
 {      
     ta9548a = !TCA9548A(0);
     DEBUG_PRINTLN ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
     myBlynk.TerminalPrint ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
 }    
}

void resetBoardID(void)
  {       
       if(esp.BOARD == ESP1 )    
       {
          V_Remote_ESP1=false;
          RC_Remote_ESP1=false;
       } 
       
       if(esp.BOARD == ESP2 ) 
       {   
          V_Remote_ESP2=false;
          RC_Remote_ESP2=false;
       } 
       
       if(esp.BOARD == ESP3 ) 
       { 
          V_Remote_ESP3=false;
          RC_Remote_ESP3=false;
       }  
  }
       



//#endif
