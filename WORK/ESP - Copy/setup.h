// setup.h

#ifndef SETUP_H
#define SETUP_H
extern void loadCrashCount();
extern uint32_t crashCount;

int card = 0;
extern IPAddress blynkLocalServer;

bool blynkInit(void)
{
   StaticJsonDocument<54> doc; //Json to send from
   String str = VERSION_ID  + "  ...  " + WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI());   
     blynkConnected = myBlynk.init();    
     if (blynkConnected) 
              {
                myBlynk.sendAvRxIndex(Av_Rx);
                myBlynk.streamSelect("ch01");
                dvrOnOff (POWER_ON);
                int rssi = WiFi.RSSI();
                myBlynk.wifiRSSI(WiFi.RSSI());
                
                myBlynk.sendVersion(VERSION_ID);
                loadCrashCount();
                myBlynk.TerminalPrint("Connecion Stablished " + VERSION_ID + " " + String(crashCount) + " Craches" );
             }
    
    awsTerminal(awsConnected, str ) ;
return  blynkConnected;
}     

tm printLocalTime() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");

    // return empty structure
    struct tm emptyTime = {};
    return emptyTime;
  }

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  return timeinfo;
}

 
void gpioSetup(void)
{
      DIP1 = CommonPins[0];
      DIP2 = CommonPins[1];
      pinMode(DIP1, INPUT_PULLUP);
      pinMode(DIP2, INPUT_PULLUP);
      delay(200); // let signals stabilize
      // Read switches
      int b0 = !digitalRead(DIP1); // invert because pullup
      int b1 = !digitalRead(DIP2);
    
      // Convert to mode number
      card = (b1 << 1) | b0;

      card = 2;

      Serial.print("Esp Selecter:ESP");
      Serial.println(card+1);

      // Configure time
     configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

   
      
      switch(card) {
        case 0:
          ActiveBoard = ESP1;
                            ActiveBoard == ESP1 ;
                            BOARD ="ESP1";
                            VERSION_ID =" ESP1 ";
                            BLYNK_AUTH_TOKEN                = BLYNK_AUTH_TOKEN_ESP1 ; //ESP1
          break;
    
        case 1:
                            ActiveBoard = ESP2;
                            BOARD ="ESP2";
                            VERSION_ID =" ESP2 ";
                            BLYNK_AUTH_TOKEN                = BLYNK_AUTH_TOKEN_ESP2 ; //ESP2
          break;
    
        case 2:
                            ActiveBoard = ESP3;
                            BOARD ="ESP3";
                            VERSION_ID =" ESP3 ";
                            BLYNK_AUTH_TOKEN         =        BLYNK_AUTH_TOKEN_ESP3  ; //ESP3
           break;
    
        case 3:
                            ActiveBoard = TEST4;
                            BOARD ="ESP0";
                            VERSION_ID =" ESP0 - ";
                            BLYNK_AUTH_TOKEN   =              BLYNK_AUTH_TOKEN_TEST ;
            break;
      }
 gitHubURL  ="https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin" ;// URL to download the firmware from  
// IPAddress ip4;
// IPAddress igateway;
    // Lillygo Realy-8
    if(ActiveBoard == ESP2) {
        for (int i = 0; i < 8; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], LOW);
      }
      selectRelay(TCA9548A_CH1);// Relay K1 ON
      
      I2C_SDA           =LilluGoPins[0]; //green
      I2C_SCL           =LilluGoPins[1]; //yellow
      BOARD_LED         =LilluGoPins[2];
      RC_TX_PIN         =LilluGoPins[3];
      AV_RX_DVR_PIN     =LilluGoPins[4];
//           // Configure static IP
//#ifdef METEOR_ETH_PLS
//    ip4 = local_IP_Relays;
//    igateway = gateway;
//#endif
// 
//  
//#ifdef CH
//    ip4 = local_IP_NICE_CH;
//    igateway = gateway_NICE_CH;
//#endif
//
//#ifdef NICE
//   ip4 = local_IP_NICE_CH;
//   igateway = gateway_NICE_CH;
//#endif               
//     if (!WiFi.config(ip4, igateway, subnet, primaryDNS, secondaryDNS)) {
//     Serial.println("STA Failed to configure");
//     }
    }

    else
    {
      I2C_SDA           =Esp32Pins[0]; //green
      I2C_SCL           =Esp32Pins[1]; //yellow
      BOARD_LED         =Esp32Pins[2];
      RC_TX_PIN         =Esp32Pins[3];
      AV_RX_DVR_PIN     =Esp32Pins[4];
           // Configure static IP
//#ifdef METEOR_ETH_PLS
//    ip4 = local_IP;
//    igateway = gateway;
//#endif
//
//  
//#ifdef CH
//    ip4 = local_IP_NICE_CH;
//    igateway = gateway_NICE_CH;
//#endif
//
//#ifdef NICE
//   ip4 = local_IP_NICE_CH;
//   igateway = gateway_NICE_CH;
//#endif           
//           
////     if (!WiFi.config(ip4, igateway, subnet, primaryDNS, secondaryDNS)) {
////     Serial.println("STA Failed to configure");
////     }
    }
      pinMode(AV_RX_DVR_PIN, OUTPUT);
      pinMode(BOARD_LED, OUTPUT);
      digitalWrite(BOARD_LED, HIGH);
      delay(50); // let signals stabilize
      digitalWrite(AV_RX_DVR_PIN, LOW);  // AV RECEIVER OFF POWER UP NC CONTACT
  
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
     Wire.begin(I2C_SDA ,I2C_SCL );
     delay(500);
     ta9548a = !TCA9548A(0);
     DEBUG_PRINTLN ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
    if(blynkConnected) myBlynk.TerminalPrint ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
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
