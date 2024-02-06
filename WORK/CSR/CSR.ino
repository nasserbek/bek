#include "MainCalls.h"

blynk myBlynk;

void setup() 
{

     relaySetup();  
     Serial.begin(115200);
     i2cSetup();
     initWDG(MIN_5,EN);
     resetWdg();    //reset timer (feed watchdog) 
     mySwitch.enableTransmit(RC_TX_PIN);
     blynkInit();
     connectAWS();
     timersMillis();
     createHandleGroup();
     looadRoomData();
     enableWDG(DIS);
     initWDG(SEC_60,EN);
}


void loop(void) 
{
       resetWdg();    //reset timer (feed watchdog) 
       internetCheck();
       blynkLoop();
       awsLoop();
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}
