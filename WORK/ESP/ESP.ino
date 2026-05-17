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

     wifiAvailable = myBlynk.wifi_init();
     relaySetup();
     if(wifiAvailable)
        {
            awsConnected = connectAWS();
            blynkConnected = blynkInit();
        }
     timersMillis();    
     Serial.print("Version: ");     Serial.println(VERSION_ID);
     Serial.print("AWS IOT This is: ");     Serial.println(THINGNAME); 
     resetInactivityTimer();
}


void loop(void) 
{
       stateDVR = digitalRead(AV_RX_DVR_PIN_2);
       resetWdg();
       internetCheck();
       resetBoardID();
       awsLoop();
       blynkLoop();
       
       if(inactivityCtrl == MILLS)restartForInactivity(); //This checks inactivity once per second only controlled by mills().
       
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}
