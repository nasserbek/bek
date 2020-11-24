#include "main.h"
#include <ESP32Ping.h>
 


 reciever av;
 sim800L sim; 
 otaUpload ota; 
 ntpServerUtil util;
 blynk myBlynk;

void setup() 
{
    // Netgerr reset pin , to be checked if on 0 or 2 physically
     pinMode(NETGEER_PIN_0, OUTPUT);
     digitalWrite(NETGEER_PIN_0, LOW);
     pinMode(NETGEER_PIN_2, OUTPUT);
     digitalWrite(NETGEER_PIN_2, LOW);
  
    // Sim800 Set-up modem reset, enable, power pins
     pinMode(MODEM_PWKEY, OUTPUT);
     pinMode(MODEM_RST, OUTPUT);
     pinMode(MODEM_POWER_ON, OUTPUT);

     digitalWrite(MODEM_PWKEY, LOW);
     digitalWrite(MODEM_RST, HIGH);
     digitalWrite(MODEM_POWER_ON, HIGH);
     delay(1000);
  
     av.bluLed(ON);
     Serial.begin(115200);
     av.init();
     EEPROM.begin(EEPROM_SIZE);
     byte gitHub = EEPROM.read(EEPROM_GITHUB_ADD);
     
     initWDG(MIN_5,EN);

     sim800Available = sim.init();
     mySwitch.enableTransmit(RC_TX_PIN);
     av.bluLed(OFF);
     wifiAvailable = myBlynk.wifiConnect();
     
     myBlynk.init();    
     blynkConnected=myBlynk.blynkConnected();
     DEBUG_PRINT("Blynk: ");DEBUG_PRINTLN( blynkConnected ? F("Connected") : F("Not Connected"));
     if (blynkConnected) 
              {
                 if (gitHub) 
                  {
                    getDateTimeNTP(gitHub); 
                    sendToHMI(util.dateAndTimeChar, "Version : ", String(util.dateAndTimeChar),FB_NOTIFIER,String(util.dateAndTimeChar));
                    DEBUG_PRINTLN(String(util.dateAndTimeChar));
                  }
                myBlynk.sendToBlynk = true; 
                myBlynk.sendToBlynkLeds = true;
                myBlynk.blynkSmsLed (sim800Available);
                myBlynk.sendAvRxIndex(Av_Rx);
             }
      else  sendToHMI("Internet failure", "Internet failure : ", "Internet failure",FB_NOTIFIER, "Internet failure" );
    
      if (!wifiAvailable) sendToHMI("Wifi failure", "Wifi failure: ", "Wifi failure",FB_NOTIFIER, "Wifi failure" );
    
    
    NetgeerResetTimer       = millis();
    wifiSurvilanceTimer     = millis();
    internetSurvilanceTimer = millis();
    liveTimerOff            = millis();
    liveTimerOn             = millis();
    wifiIDETimer            = millis();
    restartAfterResetNG     = millis();
    NetgeerResetGooglLostTimer = millis();
    blynkNotActiveTimer     = millis();
    resetNetgeerAfterInternetLossTimer = millis();
    
    DEBUG_PRINT("Wifi: ");DEBUG_PRINTLN(wifiAvailable ? F("Available") : F("Not Available"));
    String startString = String( "Restarting "   NOTIFIER_ID   VERSION_ID ) ;
    sendToHMI("Starting after reset", "Starting after reset : ", startString ,FB_NOTIFIER, "Starting after reset" );
    enableWDG(DIS);
    initWDG(SEC_60,EN);
}


void loop(void) 
{
       resetWdg();    //reset timer (feed watchdog) 
 
       if( smsEvent =sim.smsRun()) processSms();
 
       netgeerCtrl();

       if ( blynkConnected )
          {
            myBlynk.blynkRun();
            if(blynkEvent = myBlynk.getData () ) processBlynk();
            InternetLoss = false;   resetNetgeerAfterInternetLossTimer = millis();
            netGeerReset = false;   restartAfterResetNG = millis();
          }

       if( !InternetLoss && !blynkConnected)  
          {
            InternetLoss = true; 
            resetNetgeerAfterInternetLossTimer = millis();
            blynkEvent=false; 
            myBlynk.sendToBlynk = false;
            myBlynk.sendToBlynkLeds = false;
          }
    
      if (zapOnOff ) zappingAvCh (zapOnOff, zapTimer , zapCh1, zapCh2, zapCh3,zapCh4, zapCh5, zapCh6, zapCh7, zapCh8);      

}

void netgeerCtrl(void)
{
       if ( (  (millis() - internetSurvilanceTimer) >= PING_GOOGLE_BLYNK_TIMER))
              {
                pingGoogle = pingGoogleConnection();
                if (! (blynkConnected=myBlynk.blynkConnected() ) )  myBlynk.blynkConnect();
                DEBUG_PRINT("blynk: ");DEBUG_PRINTLN(blynkConnected ? F("Connected!") : F("Disconnected!"));
                internetSurvilanceTimer= millis();
              }

       if ( ( (millis() - resetNetgeerAfterInternetLossTimer) >= INTERNET_LOSS_TO_RESET_NG_TIMER) && InternetLoss && !blynkConnected && !netGeerReset)
        {
              sim.SendSMS("Blynk Disconnected for 5 min, Reset Netgeer");
              DEBUG_PRINTLN("Blynk Disconnected for 5 min, Reset Netgeer");
              ResetNetgeer();
        }

       if (  ( (millis() - restartAfterResetNG) >=  RESTART_AFTER_NG_RESET_TIMER) && netGeerReset )
          {
            sim.SendSMS("Resetaring 5 min after Netgeer Reset");
            DEBUG_PRINTLN("Resetaring 5 min after Netgeer Rreset");
            ESP.restart(); 
          }
          
}     



void ResetNetgeer(void)
          {
              delay(2000);
              digitalWrite(NETGEER_PIN_0, HIGH);
              digitalWrite(NETGEER_PIN_2, HIGH);
              delay(2000);
              digitalWrite(NETGEER_PIN_0, LOW); 
              digitalWrite(NETGEER_PIN_2, LOW);
              DEBUG_PRINTLN("Netgeer Reset done: ");
              restartAfterResetNG     = millis();
              netGeerReset = true;
              blynkInitDone = false;
              pingGoogle =false;
          }


bool pingGoogleConnection(void)
{
       bool pingInternet= Ping.ping("www.google.com");
       DEBUG_PRINT("Ping Google: ");DEBUG_PRINTLN(pingInternet ? F("succesiful") : F("failed"));
       return (pingInternet);
}

void processBlynk(void)
{
        switch (myBlynk.blynkEventID)
          {
            case FB_WIFI_IDE_ID:
               wifiIde = false;         
               wifiIDETimer = millis();
               wifiUploadCtrl();
             break;
             
            case FB_WIFI_OTA_ID:
               otaWifiGithub = false;         
               wifiIDETimer = millis();
               otaWifi();
             break;
                          
            case FB_WIFI_WEB_ID:
               wifiWebUpdater = false;
               wifiIDETimer = millis();
               webUpdateOta ();
             break;
            
            case FB_AV_7SEG_ID:
                recevierCh=myBlynk.blynkData;
                DEBUG_PRINT("FB_AV_7SEG: ");DEBUG_PRINTLN(myBlynk.blynkData);
                if (recevierCh >= 1 && recevierCh <= 9) receiverAvByCh (recevierCh);
            break;
            case FB_FREQ_ID:
              recevierFreq=myBlynk.blynkData;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(myBlynk.blynkData);
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_T433_CH_NR_ID:
              remoteControlRcCh=myBlynk.blynkData;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(myBlynk.blynkData);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 15) {remoteControl(remoteControlRcCh );}
            break;
            case FB_T315_CH_NR_ID:
              remoteControlRcCh=myBlynk.blynkData;
              DEBUG_PRINT("FB_T315_CH_NR: ");DEBUG_PRINTLN( (myBlynk.blynkData) -15);
              if (remoteControlRcCh >= 16 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );}
            break;
 
            case FB_RESET_ID:
              rebootCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(myBlynk.blynkData);
              rebootSw();
            break;
            case FB_OTA_ID:
              otaCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(myBlynk.blynkData);
              otaGsm ();
            break;
            
            case FB_SEND_TO_BLYNK_ID:
                myBlynk.sendToBlynk = myBlynk.sendToBlynkLeds= myBlynk.blynkData;
                myBlynk.sendToBlynkLed(myBlynk.sendToBlynk);
             break;
             
            case FB_VERSION_ID:
            break;
            
            case FB_FB_OFF_ID:
            break;
            
            case FB_BLYNK_ON_OFF_ID:
            break;
            
            case FB_WIFI_OFF_ID:
            break;
            
            case FB_SETTINGS_ID :
            break;

            case FB_ZAP_ID:
              zapOnOff=myBlynk.blynkData;
              DEBUG_PRINT("ZAP IS : ");
              DEBUG_PRINTLN(zapOnOff ? F("On") : F("Off"));
              myBlynk.zapLed(zapOnOff);
            break;

            case FB_ZAP_TIMER_ID :
              zapTimer=myBlynk.blynkData;
            break;

            case FB_ZAP_CHANNEL_ID1 :
              zapCh1=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID2 :
              zapCh2=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID3 :
              zapCh3=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID4 :
              zapCh4=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID5 :
              zapCh5=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID6 :
              zapCh6=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID7 :
              zapCh7=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID8 :
              zapCh8=myBlynk.blynkData;
            break;
             case FB_NETGEER_ID  :
             myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset from Blynk");
              ResetNetgeer();
            break;

            case FB_AV_CH_PLUS_ID:
                recevierCh += 1;
                if (recevierCh >= 1 && recevierCh <= 9) receiverAvByCh (recevierCh);
            break;
            case FB_AV_CH_MINUS_ID:
                recevierCh -= 1;
                if (recevierCh >= 1 && recevierCh <= 9) receiverAvByCh (recevierCh);
            break;
            case FB_AV_FR_PLUS_ID:
              recevierFreq += 1;
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_AV_FR_MINUS_ID:
              recevierFreq -= 1;
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_RC_REPETION_ID:
             repetionRC=myBlynk.blynkData;
             EEPROM.write(RC_REPETION_ADD, repetionRC); EEPROM.commit();
             mySwitch.setRepeatTransmit(repetionRC);
            break;

            case FB_RC_PULSE_ID:
             pulseRC=myBlynk.blynkData;
             mySwitch.setPulseLength(pulseRC);
            break;

            case ROOM_AV_RC:
             Av_Rx=myBlynk.blynkData;
             myBlynk.sendAvRxIndex(Av_Rx);
            break;
            
            case FB_SLEEP_TIMER_ID:
             deepSleepTimerHours=myBlynk.blynkData;
             goToDeepSleep(deepSleepTimerHours);
            break;

                        
            case ROOM_201_TO_205:
                  
                  switch (myBlynk.blynkData)
                    {
                      case 1:
                      break;
                      
                      case 2: // ROOM 202
                            remoteControlRcCh = 17;
                            recevierCh=2;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 203
                            remoteControlRcCh = 18;
                            recevierCh=3;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;      
                      
                      case 4:// ROOM 204
                            remoteControlRcCh = 19;
                            recevierCh=4;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 205
                            remoteControlRcCh = 5;
                            recevierCh=5;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }
                    
            break;
            
            case ROOM_206_TO_210:
                   switch (myBlynk.blynkData)
                    {
                      case 1:
                      break;
                      
                      case 2:// ROOM 207
                            remoteControlRcCh = 7;
                            recevierCh=7;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 208
                            remoteControlRcCh = 8;
                            recevierCh=8;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;      
                      
                      case 4:// ROOM 209
                            remoteControlRcCh = 9;
                            recevierCh=1;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 210
                            remoteControlRcCh = 10;
                            recevierCh=2;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }

            break;
            
            case ROOM_211_TO_215:
                   switch (myBlynk.blynkData)
                    {
                      case 1:
                      break;
                      
                      case 2:// ROOM 212
                            remoteControlRcCh = 28;
                            recevierCh=7;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 213
                      break;      
                      
                      case 4:// ROOM 214
                            remoteControlRcCh = 29;
                            recevierCh=6;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 214
                            remoteControlRcCh = 14;
                            recevierCh=6;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }

            break;
            
            case ROOM_216_TO_220:
                   switch (myBlynk.blynkData)
                    {
                      case 1:// ROOM 216
                            remoteControlRcCh = 1;
                            recevierCh=1;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 2:// ROOM 217
                            remoteControlRcCh = 2;
                            recevierCh=2;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:
                      break;      
                      
                      case 4:// ROOM 219
                            remoteControlRcCh = 4;
                            recevierCh=4;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:

                      break;   
                    }

            break;                                    
            
    }  
}



void processSms(void)
{
      boolean isValidNumber =false;
      int smsID=0;
      
      smsReceived =  sim.smsString;
      
       for(byte i=0;i<smsReceived.length();i++)
          {
            if(isDigit(smsReceived.charAt(i))) isValidNumber =true;
            else isValidNumber=false;
          }
        if(isValidNumber)
        {
          smsValue = stringToInteger(smsReceived);
          if (smsValue >= 1 && smsValue <= 30)     smsID =FB_T433_CH_NR_ID;
          if (smsValue >= 41 && smsValue <= 48)    smsID =FB_AV_7SEG_ID;
          if (smsValue >= 900 && smsValue <= 1360) smsID =FB_FREQ_ID;
        }
        else
        {
          if      (smsReceived =="Reset")smsID = FB_RESET_ID;
          else if (smsReceived =="Firebase") smsID = FB_FB_OFF_ID;
          else if (smsReceived =="Blynk") smsID = FB_BLYNK_ON_OFF_ID;
          else if (smsReceived =="Wifi") smsID = FB_WIFI_OFF_ID;
          else if (smsReceived =="Ota") smsID = FB_OTA_ID;
          else if (smsReceived =="Sms") smsID = FB_SMS_ON_ID;
          else if (smsReceived =="Ver") smsID =FB_VERSION_ID;
          else if (smsReceived == "Settings" ) smsID = FB_SETTINGS_ID ;
          else if (smsReceived == "Netgeer" ) ResetNetgeer();
          else if (smsReceived == "Ide" ) smsID = FB_WIFI_IDE_ID ;
          else if (smsReceived == "Web" ) smsID = FB_WIFI_WEB_ID ;
          else if (smsReceived == "Otaweb" ) smsID = FB_WIFI_OTA_ID ;
        }
        switch (smsID)
          {
            case FB_WIFI_IDE_ID:
               wifiIde = false;         
               wifiIDETimer = millis();
               wifiUploadCtrl();
             break;

            case FB_WIFI_OTA_ID:
               otaWifiGithub = false;         
               wifiIDETimer = millis();
               otaWifi();
             break;
                        
            case FB_WIFI_WEB_ID:
               wifiWebUpdater = false;
               wifiIDETimer = millis();
               webUpdateOta ();
             break;
             
            case FB_AV_7SEG_ID:
                recevierCh=smsValue-40;
                DEBUG_PRINT("FB_AV_7SEG: ");DEBUG_PRINTLN(recevierCh);
                if (recevierCh >= 1 && recevierCh <= 8) receiverAvByCh (recevierCh);
            break;
            case FB_FREQ_ID:
              recevierFreq=smsValue;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(recevierFreq);
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_T433_CH_NR_ID:
              remoteControlRcCh=smsValue;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(remoteControlRcCh);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );}
            break;
            case FB_RESET_ID:
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(smsReceived);
              rebootSw();
            break;
            case FB_OTA_ID:
              otaCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(smsReceived);
              otaGsm ();
            break;
            case FB_SMS_ON_ID:
            break;
            
            case FB_VERSION_ID:
              DEBUG_PRINT("FB_VERSION: ");DEBUG_PRINTLN(smsReceived);
              sendVersion();
            break;
            
            case FB_FB_OFF_ID:
            break;
            
            case FB_BLYNK_ON_OFF_ID:
            break;
            
            case FB_WIFI_OFF_ID:
            break;
            
            case FB_SETTINGS_ID :
            break;
    }  
}

void zappingAvCh (bool zapCmd, int zapTimer, bool ch1, bool ch2, bool ch3,bool ch4, bool ch5, bool ch6, bool ch7, bool ch8)
{
         switch (stateMachine)
          {
            case 0:
            case 1:
                if (ch1 ) 
                  {
                    if (stateMachine == 0) {zaptime= millis();stateMachine =1;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (1);stateMachine =2;}
                  }
                else stateMachine =2;
            break;

            case 2:
            case 3:
                if (ch2 ) 
                  {
                    if (stateMachine == 2) {zaptime= millis();stateMachine =3;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (2);stateMachine =4;}
                  }
                else stateMachine =4;
            break;
            
            case 4:
            case 5:
                if (ch3 ) 
                  {
                    if (stateMachine == 4) {zaptime= millis();stateMachine =5;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (3);stateMachine =6;}
                  }
                else stateMachine =6;
            break;

            case 6:
            case 7:
                if (ch4 ) 
                  {
                    if (stateMachine == 6) {zaptime= millis();stateMachine =7;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (4);stateMachine =8;}
                  }
                else stateMachine =8;
            break;
                       
            case 8:
            case 9:
                if (ch5 ) 
                  {
                    if (stateMachine == 8) {zaptime= millis();stateMachine =9;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (5);stateMachine =10;}
                  }
                else stateMachine =10;
            break;
                       
            case 10:
            case 11:
                if (ch6 ) 
                  {
                    if (stateMachine == 10) {zaptime= millis();stateMachine =11;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (6);stateMachine =12;}
                  }
                else stateMachine =12;
            break;
                       
            case 12:
            case 13:
                if (ch7 ) 
                  {
                    if (stateMachine == 12) {zaptime= millis();stateMachine =13;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (7);stateMachine =14;}
                  }
                else stateMachine =14;
            break;
                       
            case 14:
            case 15:
                if (ch8) 
                  {
                    if (stateMachine == 14) {zaptime= millis();stateMachine =15;}
                    if (millis() - zaptime > zapTimer) {receiverAvByCh (8);stateMachine =0;}
                  }
                else stateMachine =0;
            break;
          }

}



void remoteControl(int cmd )
{
     if (blynkConnected)    
      {
        if (cmd >= 1 && cmd <= 15)  {myBlynk.blynkRCLed(1); }
        if (cmd >= 16 && cmd <= 30) {myBlynk.blynkRCLed315(1);}
      }

     mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);
     DEBUG_PRINT("ch433:");DEBUG_PRINTLN(cmd);
     delay(500);
     
     if (blynkConnected)    
      {
        if (cmd >= 1 && cmd <= 15) {myBlynk.blynkRCLed(0);myBlynk.resetT433Cmd(cmd);}
        if (cmd >= 16 && cmd <= 30) {myBlynk.blynkRCLed315(0);myBlynk.resetT315Cmd(cmd);}
      }

}




        
void receiverAvByCh (int Ch)
{
  bool ack;
  int PLL_value;
       if (blynkConnected) myBlynk.blynkAckLed(true);
       
       if (Ch != 9) {ack = av.Tuner_PLL(av_pll_addr, PLL[Ch]);}
       else 
        {
          PLL_value =( 512 * ( 1000000 * (990 + 479.5) ) ) / (16*4000000) ;
          ack = av.Tuner_PLL(av_pll_addr, PLL_value);
        }
       delay(500);
       if (blynkConnected) {myBlynk.blynkAckLed(ack); myBlynk.sevenSegValue(Ch );}
      
        switch (Ch)
          {
            case 1:
               if (blynkConnected) myBlynk.frequencyValue(1080 );
               recevierFreq =1080;
            break;

            case 2:
                if (blynkConnected)myBlynk.frequencyValue(1120 );
                recevierFreq =1120;
            break;

            
            case 3:
               if (blynkConnected) myBlynk.frequencyValue(1160 );
               recevierFreq =1160;
            break;


            case 4:
               if (blynkConnected) myBlynk.frequencyValue(1200 );
               recevierFreq =1200;
            break;

                       
            case 5:
              if (blynkConnected)  myBlynk.frequencyValue(1240 );
              recevierFreq =1240;
            break;

                       
            case 6:
              if (blynkConnected)  myBlynk.frequencyValue(1280 );
              recevierFreq =1280;
            break;

                       
            case 7:
              if (blynkConnected)  myBlynk.frequencyValue(1320 );
              recevierFreq =1320;
            break;

                       
            case 8:
              if (blynkConnected)  myBlynk.frequencyValue(1360 );
              recevierFreq =1360;
            break;

          }
       DEBUG_PRINT("Received freq channel:");DEBUG_PRINTLN(Ch);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}



void receiverAvByFreq (int Freq)
{
  bool ack=0;
       recevierFreq =Freq;
       if (blynkConnected) myBlynk.blynkAckLed(true);
       int PLL_value =( 512 * ( 1000000 * (Freq + 479.5) ) ) / (16*4000000) ;
       ack = av.Tuner_PLL(av_pll_addr, PLL_value);
       if (blynkConnected)  { myBlynk.blynkAckLed(ack);myBlynk.frequencyValue(Freq );}
       DEBUG_PRINT("Received manual_freq:");DEBUG_PRINTLN(manual_freq);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}






void otaGsm(void)
{
  DEBUG_PRINTLN("Firmware Upload..."); 
  enableWDG(DIS);
  ota.init(true);
  ota.startOtaUpdate(overTheAirURL);
}






void room (int RC, int AV, int sel)
{
     switch (sel)
          {
            case 1:
                receiverAvByCh (AV);
            break;

            case 2:
                remoteControl(RC);
            break;

            
            case 3:
                receiverAvByCh (AV);
                remoteControl(RC);
            break;
          }
}                            

void getDateTimeNTP(bool ver)
{
   if (ver)util.printLocalTime(true); 
   else util.sendDateTime(false);
}    



void IRAM_ATTR resetModule() 
{
EEPROM.write(EEPROM_ERR_ADD, WDG_ERR); EEPROM.commit();
DEBUG_PRINTLN("Watch Dog Timer Timout, rebooting....");
ESP.restart();
}

void initWDG(int wdtTimeout,bool _enable) 
{
   _timer = timerBegin(0, 80, true);                                    //timer 0, div 80
   timerAttachInterrupt(_timer, &resetModule, true);                  //attach callback
   timerAlarmWrite(_timer, wdtTimeout * 1000, false);              //set time in us
   enableWDG(_enable);                                            //enable interrupt
}

void enableWDG(bool _enable)
  {
   if (_enable) timerAlarmEnable(_timer);                          //enable interrupt
   else timerAlarmDisable(_timer);                                //Disable interrupt
   resetWdg();                                        //reset timer (feed watchdog)  
  }
  
void resetWdg(void)
  {
   timerWrite(_timer, 0);                                        //reset timer (feed watchdog)  
  }

 
void liveCtrl(void)
{
   if ( (millis() - liveTimerOn > LIVE_TIMER_ON) && !liveBit ) 
          {
            liveBit = true ;
            av.bluLed(liveBit);
            liveTimerOff = millis();
            if ( blynkConnected) myBlynk.sendAlive(liveBit);
          }
    if ( (millis() - liveTimerOff > LIVE_TIMER_OFF) && liveBit ) 
          {
            liveBit = false ;
            av.bluLed(liveBit);
            liveTimerOn = millis();
           if ( blynkConnected)  myBlynk.sendAlive(liveBit);
          }
}

/**********************************************************************************************************************************************/
void  getSettingsFromEeprom(void)
{
}

void sendToHMI(char *smsmsg, String notifier_subject, String notifier_body,String fb_path,String fb_cmdString)
{
  if(sim800Available)sim.SendSMS(smsmsg);
  if (blynkConnected) myBlynk.notifierDebug(NOTIFIER_ID, notifier_body);
  DEBUG_PRINTLN(notifier_body);
}


void rebootSw(void)
{
 ESP.restart();
}

void smsActivation(int activate)
{
}

void firebaseOnActivation(int activation)
{
} 


void wifiActivation(int activation)
{
}

void blynkActivation (int activation)
{
}

void sendVersion(void)
{
 util.printLocalTime(false); 
 sendToHMI(util.dateAndTimeChar, "Version : ", String(util.dateAndTimeChar),FB_NOTIFIER, String(util.dateAndTimeChar) );
}

int stringToInteger(String str)
{
char carray[5]; 
      str.toCharArray(carray, sizeof(carray));
      return ( atoi(carray));  
}

void goToDeepSleep(int sleepTimer)
{
      EEPROM.write(EEPROM_ERR_ADD, DEEP_SLEEP ); EEPROM.commit(); 
      sendToHMI("Going to Deep Sleep", "Going to Deep Sleep", "Going to Deep Sleep",FB_NOTIFIER, "Going to Deep Sleep" );
      sim.sim800PowerOn(false)  ;
      DEBUG_PRINT("Sleep for: ");  DEBUG_PRINT(sleepTimer * 60* 1000000);DEBUG_PRINTLN(" uSec");
      esp_sleep_enable_timer_wakeup(sleepTimer * 60 * 1000000); // in microseconds
      Serial.flush(); 
      esp_deep_sleep_start();
}


void webOtaSetup(void)
{

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });

    server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

 /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
 }

 void webUpdateOta (void)
 {
   webOtaSetup();
   
   while (!wifiWebUpdater) 
       {
        enableWDG(false);
        if (  millis() - wifiIDETimer > WIFI_IDE_TIMER )
        {
           wifiWebUpdater = true;
           resetWdg();
           enableWDG(true);
           wifiIDETimer = millis();
           ESP.restart();
        }
        server.handleClient();
        delay(1);
       }
 }


void otaIdeSetup (void)
     {
        ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 }


void wifiUploadCtrl(void)
{
       otaIdeSetup () ;   
       while (!wifiIde) 
       {
        enableWDG(false);
        if (  millis() - wifiIDETimer > WIFI_IDE_TIMER )
        {
           wifiIde = true;
           resetWdg();
           enableWDG(true);
           wifiIDETimer = millis();
           EEPROM.write(EEPROM_ERR_ADD, IDE_WIFI); EEPROM.commit();
           ESP.restart();
        }
        else ArduinoOTA.handle();
       }
}


void otaWifi(void) {

  DEBUG_PRINTLN("Restarting Ota Web Update from Github");
  sendToHMI("Ota web Started", "Ota Web : ", "Ota web Started",FB_NOTIFIER, "Ota web Started" );
  
  EEPROM.write(EEPROM_GITHUB_ADD, 1); EEPROM.commit();
  EEPROM.write(EEPROM_WIFI_ADD, 1); EEPROM.commit(); //To enable Wifi and get NTP for version date
while (!otaWifiGithub) 
       {
        enableWDG(false);
        if (  millis() - wifiIDETimer > WIFI_IDE_TIMER )
        {
           otaWifiGithub = true;
           resetWdg();
           enableWDG(true);
           wifiIDETimer = millis();
           EEPROM.write(EEPROM_ERR_ADD, IDE_WIFI); EEPROM.commit();
           ESP.restart();
        }
        ota.otaWebGithub();
       }

 }
