#include "MainCalls.h"

blynk myBlynk;

extern void BLYNK_WRITE_V8_boot();

void setup() 
{
     Serial.begin(115200);
     
     initWDG(MIN_5,EN);
     
     createHandleGroup();

     looadRoomData();     

     safeModeCheck();

     i2cSetup();
     mySwitch.enableTransmit(RC_TX_PIN);  
     
}





void loop(void) 
{
       stateDVR = digitalRead(AV_RX_DVR_PIN);
       
       resetWdg();
       internetCheck();
       resetBoardID();
       blynkLoop();
       
       BLYNK_WRITE_V8_boot();
       
       safeModeLoop();
       if(inactivityCtrl == MILLS)restartForInactivity(); //This checks inactivity once per second only controlled by mills().
       
       bool zapScan = zapOnOff || zapScanOnly;
       
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}
