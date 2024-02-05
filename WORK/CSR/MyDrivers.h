// MyDrivers.h

#ifndef MYDRIVERS_H
#define MYDRIVERS_H

#include "main.h"
#include "aws.h"
#include "OTA.h"
#include "critical.h"
#include "receivers.h"
#include "zap.h"

extern void connectAWS(void);
extern blynk myBlynk;
extern void processBlynkQueu(void);

 
void blynkLoop(void)
{
  blynkConnected=myBlynk.blynkStatus(); 
       if ( blynkConnected )
          {
            myBlynk.blynkRun();
            queuValidData = (xQueueReceive(g_event_queue_handle, &queuDataID, 5 / portTICK_RATE_MS) == pdPASS);
            if(queuValidData) 
                  {
                  myBlynk.getData ();
                  if (hmi == BLYNK)    
                    {
                     myBlynk.TerminalPrint("queuValidData received from Blynk:");
                     queuData = myBlynk.blynkData; 
                     processBlynkQueu(); 
                    }
                  }
            InternetLoss = false;   resetNetgeerAfterInternetLossTimer = millis();
            netGeerReset = false;   restartAfterResetNG = millis();
         }

       else if( !InternetLoss && !blynkConnected)  
          {
            DEBUG_PRINTLN("Blynk Disconnected , Internet Loss!!!");
            InternetLoss = true; 
            resetNetgeerAfterInternetLossTimer = millis();
            blynkEvent=false; 
            myBlynk.sendToBlynk = false;
            myBlynk.sendToBlynkLeds = false;
          }
       myBlynk.blynkRunTimer();
}



void awsLoop(void)
{
         awsConnected = client.connected();
         if (!awsConnected )
            {
              myBlynk.TerminalPrint("AWS IoT Disonnected, trying to reconnect");
              connectAWS();
           }
        else 
        {
            client.loop();   //AWS MQTT
            queuValidData = (xQueueReceive(g_event_queue_handle, &queuDataID, 5 / portTICK_RATE_MS) == pdPASS);
            if(queuValidData) 
                  {
                      getDataNodeRed ();
                      if (hmi == NODE_RED) { 
                      myBlynk.TerminalPrint("queuValidData received from AWS:");
                      queuData = nodeRedData; 
                      processBlynkQueu();
                      }
                  }
         }
}
     
void relaySetup(void)
{
     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     pinMode(I2C_1_2_RELAY , OUTPUT);
     pinMode(I2C_3_4_RELAY , OUTPUT);

     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // AV RECEIVER ON POWER UP NC CONTACT
     digitalWrite(I2C_1_2_RELAY, HIGH);   
     digitalWrite(I2C_3_4_RELAY, HIGH);  
}

void blynkInit(void)
{
     myBlynk.init();    
     blynkConnected=myBlynk.blynkStatus();
     wifiAvailable = myBlynk.wifiStatus();
     DEBUG_PRINT("Blynk: ");DEBUG_PRINTLN( blynkConnected ? F("Connected") : F("Not Connected"));
     if (blynkConnected) 
              {
                myBlynk.sendAvRxIndex(Av_Rx);
                     #ifdef CSR    
                        myBlynk.RelaySelect(3);
                     #endif 
                       #ifdef CSR2    
                        myBlynk.RelaySelect(1);
                     #endif 
                      #if defined CSR3 || defined TEST    
                        myBlynk.RelaySelect(3);
                     #endif   
                myBlynk.dvrSwitch(1);
                myBlynk.sendVersion(VERSION_ID);
                myBlynk.TerminalPrint(WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) );
             }
}     

void timersMillis(void)
{
    internetSurvilanceTimer = millis();
    liveTimerOff            = millis();
    OtaTimeoutTimer            = millis();
    ackTimer                = millis();
    restartAfterResetNG     = millis();
    NetgeerResetGooglLostTimer = millis();
    blynkNotActiveTimer     = millis();
    routerResetTimer        = millis();
    resetNetgeerAfterInternetLossTimer = millis();
}

void i2cSetup(void)
{
     Wire.begin();
     delay(500);
     Wire1.begin(SDA_2, SCL_2);
}


#endif
