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
     
     Serial.print("Version: ");     Serial.println(esp.VERSION_ID);
     Serial.print("AWS IOT This is: ");     Serial.println(esp.THINGNAME);
     Serial.print("ActiveBoard: ");     Serial.println(esp.BOARD);
     
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
   //    BoardDefines(); 
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
