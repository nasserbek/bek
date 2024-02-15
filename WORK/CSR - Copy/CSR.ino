#include "MainCalls.h"

blynk myBlynk;

void setup() 
{
     Serial.begin(115200);
     initWDG(SEC_60,EN);
     relaySetup();
     i2cSetup();
     mySwitch.enableTransmit(RC_TX_PIN);
     blynkInit();
     connectAWS();
     timersMillis();
     createHandleGroup();
     looadRoomData();
}


void loop(void) 
{
       resetWdg();
       internetCheck();
       blynkLoop();
       awsLoop();
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}
