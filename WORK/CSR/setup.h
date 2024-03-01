// setup.h

#ifndef SETUP_H
#define SETUP_H

void relaySetup(void)
{
     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     pinMode(I2C_1_2_RELAY , OUTPUT);
     pinMode(I2C_3_4_RELAY , OUTPUT);

     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // AV RECEIVER ON POWER UP NC CONTACT
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
                myBlynk.sendBoardIndex(activeBoard);
                
                     #ifdef CSR      //DEFAULT RX3
                        myBlynk.RelaySelect(3);
                        selected_Rx = 2; 
                        digitalWrite(I2C_1_2_RELAY, HIGH);   //1,2
                        digitalWrite(I2C_3_4_RELAY, HIGH);   //3,4
                     #endif 
                     
                     #ifdef CSR2    //DEFAULT RX4
                        myBlynk.RelaySelect(4);
                        selected_Rx = 3; 
                        digitalWrite(I2C_1_2_RELAY, HIGH);   //1,2
                        digitalWrite(I2C_3_4_RELAY, LOW);   //3,4                        
                     #endif 
                     
                     #ifdef CSR3     //DEFAULT RX3
                        myBlynk.RelaySelect(3);
                        selected_Rx = 2; 
                        digitalWrite(I2C_1_2_RELAY, HIGH);   //1,2
                        digitalWrite(I2C_3_4_RELAY, HIGH);   //3,4                        
                     #endif   

                myBlynk.dvrSwitch(1);
                myBlynk.sendVersion(VERSION_ID);
                myBlynk.TerminalPrint(WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) );
             }

#ifdef CSR      
    activeBoard = selectedBoard = ESP1;
#endif

#ifdef CSR2     
    activeBoard = selectedBoard =  ESP2;
#endif

#ifdef CSR3    
    activeBoard = selectedBoard =  ESP3;
#endif

#ifdef TEST  
    activeBoard = selectedBoard =  TEST4;
#endif
             
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
