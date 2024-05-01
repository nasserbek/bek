#include "MainCalls.h"

blynk myBlynk;

void setup() 
{
     Serial.begin(115200);
     initWDG(MIN_5,EN);
     relaySetup();
     
     mySwitch.enableTransmit(RC_TX_PIN);
     blynkInit();
     connectAWS();
     i2cSetup();
     timersMillis();
     createHandleGroup();
     looadRoomData();
}


void loop(void) 
{
       resetWdg();
       internetCheck();
       
       #ifdef CSR    
          V_Remote_CSR1=false;
          RC_Remote_CSR1=false;
       #endif 
       
       #ifdef CSR2    
          V_Remote_CSR2=false;
          RC_Remote_CSR2=false;
       #endif 
       
       #ifdef CSR3  
          V_Remote_CSR3=false;
          RC_Remote_CSR3=false;
       #endif   
       
       blynkLoop();
       awsLoop();
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
      
      if ( (millis() - Router_24_hoursTimer) >=  ROUTER_24_HOURS  )
          {
            Router_24_hoursTimer       = millis();
            myBlynk.TerminalPrint("RESTARTING ROUTER...");
            #ifdef CSR      //Use only CSR1 to restart Router
               myBlynk.Event24();
                resetRouter();
             #endif 
            
           }
}
