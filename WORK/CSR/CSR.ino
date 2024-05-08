#include "MainCalls.h"

blynk myBlynk;

void setup() 
{
     Serial.begin(115200);
     initWDG(MIN_5,EN);
     relaySetup();
     
     Serial.println(VERSION_ID);
     Serial.println(THINGNAME);
     
     mySwitch.enableTransmit(RC_TX_PIN);
     i2cSetup();
     wifiAvailable = myBlynk.wifi_init();
     if(wifiAvailable)
        {
            awsConnected = connectAWS();
            blynkConnected = blynkInit();
        }
     timersMillis();
     createHandleGroup();
     looadRoomData();
     bool PowerOn = false;
}


void loop(void) 
{
       resetWdg();
       internetCheck();
       resetBoardID();
       awsLoop();
       blynkLoop();
       
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
      
}
