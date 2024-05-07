// setup.h

#ifndef SETUP_H
#define SETUP_H

void relaySetup(void)
{
     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     pinMode(I2C_1_2_RELAY , OUTPUT);
     pinMode(I2C_3_4_RELAY , OUTPUT);
     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // AV RECEIVER OFF POWER UP NC CONTACT
}

       
void blynkInit(void)
{
     myBlynk.init();    
     blynkConnected=myBlynk.blynkStatus();
     wifiAvailable = myBlynk.wifiStatus();
     if (blynkConnected) 
              {
                myBlynk.sendAvRxIndex(Av_Rx);
                dvrOnOff (1);
                 myBlynk.TerminalPrint(WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) );
                int rssi = WiFi.RSSI();
                myBlynk.wifiRSSI(WiFi.RSSI());
                myBlynk.sendVersion(VERSION_ID + WiFi.SSID()  );
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
    Router_24_hoursTimer       = millis();
}

void i2cSetup(void)
{
   bool ta9548a = false;
     Wire.begin();
     delay(500);
     Wire1.begin(SDA_2, SCL_2);

 #ifdef CSR      
     ta9548a = !TCA9548A(0);
     DEBUG_PRINTLN ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
     myBlynk.TerminalPrint ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
 #endif    
}

void resetBoardID(void)
  {       
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
  }
       

#endif
