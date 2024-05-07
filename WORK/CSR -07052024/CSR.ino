#include "MainCalls.h"

blynk myBlynk;

void setup() 
{
     Serial.begin(115200);
     initWDG(MIN_5,EN);
     relaySetup();
     
     mySwitch.enableTransmit(RC_TX_PIN);
     i2cSetup();
     blynkInit();
     connectAWS();
     
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
       blynkLoop();
       awsLoop();
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
      
//      if ( (millis() - Router_24_hoursTimer) >=  ROUTER_24_HOURS  )
//          {
//            Router_24_hoursTimer       = millis();
//            myBlynk.TerminalPrint("RESTARTING ROUTER...");
//            #ifdef CSR      //Use only CSR1 to restart Router
//               myBlynk.Event24();
//                resetRouter();
//             #endif 
//            
//           }
       
}
