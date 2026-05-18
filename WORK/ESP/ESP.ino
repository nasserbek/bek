#include "MainCalls.h"

blynk myBlynk;

void setup() 
{
     Serial.begin(115200);
     initWDG(MIN_5,EN);
     mySwitch.enableTransmit(RC_TX_PIN);  
        
     createHandleGroup();
     looadRoomData();     
     i2cSetup();
     
     safeModeCheck();
}


void loop(void) 
{
       stateDVR = digitalRead(AV_RX_DVR_PIN_2);
       
       resetWdg();
       internetCheck();
       resetBoardID();
       awsLoop();
       blynkLoop();
       safeModeLoop();
       if(inactivityCtrl == MILLS)restartForInactivity(); //This checks inactivity once per second only controlled by mills().
       checkSleep();
       
       bool zapScan = zapOnOff || zapScanOnly;
       
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}
