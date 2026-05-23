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
     // Configure static IP
     if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
     Serial.println("STA Failed to configure");
     }
     safeModeCheck();
}


void loop(void) 
{
       stateDVR = digitalRead(AV_RX_DVR_PIN);
       
       resetWdg();
       internetCheck();
       resetBoardID();
    //   awsLoop();
       blynkLoop();
       safeModeLoop();
       if(inactivityCtrl == MILLS)restartForInactivity(); //This checks inactivity once per second only controlled by mills().
       
       bool zapScan = zapOnOff || zapScanOnly;
       
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}
