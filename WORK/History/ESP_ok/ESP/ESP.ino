#include "MainCalls.h"

blynk myBlynk;
 


void setup() 
{
     Serial.begin(115200);
     initWDG(MIN_5,EN);
     relaySetup();
      #ifdef LILLYGO_RELAY_8
        LillyGo_Relay_8_Setup();
      #endif
     
     Serial.print("Version: ");     Serial.println(VERSION_ID);
     Serial.print("AWS IOT Thnis is: ");     Serial.println(THINGNAME);
     
     mySwitch.enableTransmit(RC_TX_PIN);  
        
     createHandleGroup();
     looadRoomData();     
     i2cSetup();

     wifiAvailable = myBlynk.wifi_init();
     if(wifiAvailable)
        {
            awsConnected = connectAWS();
            blynkConnected = blynkInit();
        }
     timersMillis();     
}


void loop(void) 
{
        if (wdtTriggered)
        {
          wdtTriggered = 0;
          Serial.println("WDT expired → restarting");
          delay(20);
          ESP.restart();
        }
       
       BoardDefines(); 
       resetWdg();
       internetCheck();
       resetBoardID();
       awsLoop();
       blynkLoop();
    
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 

      if (millis() - lastFeed > 30000)
          {
              Serial.println("WDT timeout");
              ESP.restart();
          }
}
