// setup.h

#ifndef SETUP_H
#define SETUP_H
int card = 0;
 
void relaySetup(void)
{
      pinMode(AV_RX_DVR_PIN_2, OUTPUT);
      delay(50); // let signals stabilize
      digitalWrite(AV_RX_DVR_PIN_2, LOW);  // AV RECEIVER OFF POWER UP NC CONTACT

      pinMode(I2C_1_2_RELAY , OUTPUT);
      pinMode(I2C_3_4_RELAY , OUTPUT);

      pinMode(DIP1, INPUT_PULLUP);
      pinMode(DIP2, INPUT_PULLUP);
      delay(200); // let signals stabilize
      // Read switches
      int b0 = !digitalRead(DIP1); // invert because pullup
      int b1 = !digitalRead(DIP2);
    
      // Convert to mode number
      card = (b1 << 1) | b0;
    
      Serial.print("Card Selecter: ");
      Serial.println(card);
    
      switch(card) {
        case 0:
          ActiveBoard = ESP1;
          break;
    
        case 1:
          ActiveBoard = ESP2;
          break;
    
        case 2:
          ActiveBoard = ESP3;
          break;
    
        case 3:
          ActiveBoard = TEST4;
          break;
      }
     
     BoardDefines(); 
}
  

bool blynkInit(void)
{
   StaticJsonDocument<54> doc; //Json to send from
     blynkConnected = myBlynk.init();    
     String str = WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI());
     if (blynkConnected) 
              {
                myBlynk.sendAvRxIndex(Av_Rx);
                myBlynk.streamSelect(streamWebDdns);
                dvrOnOff (1);
                int rssi = WiFi.RSSI();
                myBlynk.wifiRSSI(WiFi.RSSI());
                myBlynk.sendVersion(VERSION_ID + WiFi.SSID()  );
                myBlynk.TerminalPrint(str );
             }
    
    awsTerminal(awsConnected, str ) ;
return  blynkConnected;
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

 if(ActiveBoard == ESP1 )
 {      
     ta9548a = !TCA9548A(0);
     DEBUG_PRINTLN ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
     myBlynk.TerminalPrint ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
 }    
}

void resetBoardID(void)
  {       
       if(ActiveBoard == ESP1 )    
       {
          V_Remote_ESP1=false;
          RC_Remote_ESP1=false;
       } 
       
       if(ActiveBoard == ESP2 ) 
       {   
          V_Remote_ESP2=false;
          RC_Remote_ESP2=false;
       } 
       
       if(ActiveBoard == ESP3 ) 
       { 
          V_Remote_ESP3=false;
          RC_Remote_ESP3=false;
       }  
  }
       

#endif
