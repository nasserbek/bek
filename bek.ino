#include "main.h"

 reciever avReceiver;
 sim800L sms; 
 otaUpload ota; 
 ntpServerUtil util;
 blynk myBlynk;
 cayenne myCayenne;
 
QueueHandle_t g_event_queue_handle = NULL;
EventGroupHandle_t g_event_group = NULL;

int queuData;
int queuDataID;
bool queuValidData=false;
bool streamWebDdns = DDNS;
bool routerResetStart =false;


void looadRoomData()
{
  int freq;

   Ch_1.frequency = 1080;   // 216
   Ch_1.id =1;

   Ch_2.frequency = 1120;   // 202,217
   Ch_2.id =2;

   Ch_3.frequency = 1160;   // 203
   Ch_3.id =3;

   Ch_4.frequency = 1200;   // 204,219
   Ch_4.id =4;

   Ch_5.frequency = 1240;   // 205
   Ch_5.id =5;

   Ch_6.frequency = 1280;   // 215
   Ch_6.id =6;

   Ch_7.frequency = 1320;   // 212,214
   Ch_7.id =7;

   Ch_8.frequency = 1360;   // 208
   Ch_8.id =8;

   Ch_9.frequency = 1000;   // 209
   Ch_9.id =9;

   Ch_10.frequency = 1040;   // 207
   Ch_10.id =10;

   Ch_11.frequency = 1060;   // 208 NEW
   Ch_11.id =11;
/*
   Ch_12.frequency = 1080;
   Ch_12.id =12;

   Ch_13.frequency = 1080;
   Ch_13.id =13;

   Ch_14.frequency = 1080;
   Ch_14.id =14;
*/

   xRoom_202.frequency = freq =Ch_2.frequency;
   xRoom_202.pll = _pll[Ch_2.id] = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_202.vCh = Ch_2.id;
   xRoom_202.rCh=17;
   xRoom_202.queuId = 202;

   xRoom_203.frequency = freq =Ch_3.frequency;
   xRoom_203.pll =  _pll[Ch_3.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_203.vCh = Ch_3.id;
   xRoom_203.rCh=18;
   xRoom_203.queuId = 203;

   xRoom_204.frequency = freq =Ch_4.frequency;
   xRoom_204.pll = _pll[Ch_4.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_204.vCh = Ch_4.id;
   xRoom_204.rCh=19;
   xRoom_204.queuId = 204;


   xRoom_205.frequency = freq =Ch_5.frequency;
   xRoom_205.pll =  _pll[Ch_5.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_205.vCh = Ch_5.id;
   xRoom_205.rCh=5;
   xRoom_205.queuId = 205;

   xRoom_206.frequency = freq =Ch_6.frequency;
   xRoom_206.pll =  _pll[Ch_6.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_206.vCh = Ch_6.id;
   xRoom_206.rCh=6;
   xRoom_206.queuId = 206;

   xRoom_207.frequency = freq =Ch_10.frequency;
   xRoom_207.pll =  _pll[Ch_10.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_207.vCh = Ch_10.id;
   xRoom_207.rCh=7;
   xRoom_207.queuId = 207;

   xRoom_208.frequency = freq =Ch_8.frequency;
   xRoom_208.pll =  _pll[Ch_8.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_208.vCh = Ch_8.id;
   xRoom_208.rCh=8;
   xRoom_208.queuId = 208;

   xRoom_209.frequency = freq =Ch_9.frequency;
   xRoom_209.pll =  _pll[Ch_9.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_209.vCh = Ch_9.id;
   xRoom_202.rCh=9;
   xRoom_202.queuId = 209;

   xRoom_210.frequency = freq =Ch_2.frequency;
   xRoom_210.pll =  _pll[Ch_2.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_210.vCh = Ch_2.id;
   xRoom_210.rCh=10;
   xRoom_210.queuId = 210;

   xRoom_211.frequency = freq =1080;
   xRoom_211.pll = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_211.vCh = 2;
   xRoom_211.rCh=17;
   xRoom_211.queuId = 211;

   xRoom_212.frequency = freq =Ch_7.frequency;
   xRoom_212.pll =  _pll[Ch_7.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_212.vCh = Ch_7.id;
   xRoom_212.rCh=28;
   xRoom_212.queuId = 212;

   xRoom_214.frequency = freq =Ch_7.frequency;
   xRoom_214.pll =  _pll[Ch_7.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_214.vCh = Ch_7.id;
   xRoom_214.rCh=29;
   xRoom_214.queuId = 214;

   xRoom_215.frequency = freq =Ch_6.frequency;
   xRoom_215.pll =  _pll[Ch_6.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_215.vCh = Ch_6.id;
   xRoom_215.rCh=14;
   xRoom_215.queuId = 215;

   xRoom_216.frequency = freq =Ch_1.frequency;
   xRoom_216.pll =  _pll[Ch_1.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_216.vCh = Ch_1.id;
   xRoom_216.rCh=1;
   xRoom_216.queuId = 216;

   xRoom_217.frequency = freq =Ch_2.frequency;
   xRoom_217.pll =  _pll[Ch_2.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_217.vCh = Ch_2.id;
   xRoom_217.rCh=2;
   xRoom_217.queuId = 217;

   xRoom_218.frequency = freq =1080;
   xRoom_218.pll = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_218.vCh = 2;
   xRoom_218.rCh=17;
   xRoom_218.queuId = 218;

   xRoom_219.frequency = freq =Ch_4.frequency;
   xRoom_219.pll =  _pll[Ch_4.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_219.vCh = Ch_4.id;
   xRoom_219.rCh=4;
   xRoom_219.queuId = 219;

}

void createHandleGroup()
{
     //Create a program that allows the required message objects and group flags
    g_event_queue_handle = xQueueCreate(50, sizeof(int)); // Creates a queue of 50 int elements
    g_event_group = xEventGroupCreate();
}

void setup() 
{
//RELE WITH LOW TRIGGER, ON IF LOW AND OFF IF HIGH
     pinMode(NETGEER_PIN_0, OUTPUT);
     digitalWrite(NETGEER_PIN_0, HIGH);// NC ACTIVATE ON POWER ON BY DOING NOTHING

     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // NC DISACTIVATE AV RECEIVER ON POWER ON

     Serial.begin(115200);
     
     avReceiver.init_I2C();
     
     EEPROM.begin(EEPROM_SIZE);
     byte gitHub = EEPROM.read(EEPROM_GITHUB_ADD);
     
     initWDG(MIN_5,EN);
     resetWdg();    //reset timer (feed watchdog) 
     
     sim800Available = sms.init();
     mySwitch.enableTransmit(RC_TX_PIN);

     delay(3000);  // Wait for SIM to stablish connection
     smsSent= sms.SendSMS("Sim800 Ok, Connecting to WIFI and Blynk.....");
     
    
     myBlynk.init();    
     blynkConnected=myBlynk.blynkStatus();
     wifiAvailable = myBlynk.wifiStatus();
     
     if (wifiAvailable) {if(smsSent) smsSent= sms.SendSMS("WIFI Connected..");}
     else {if(smsSent) smsSent= sms.SendSMS("WIFI failed to connect");}
          
     if (blynkConnected) {if(smsSent) smsSent= sms.SendSMS("BLYNK Connected, starting the Loop");}
     else {if(smsSent) smsSent= sms.SendSMS("BLYNK failed to connect, starting the Loop");}


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
                myBlynk.blynkSmsLed (sim800Available & smsSent);
                myBlynk.sendAvRxIndex(Av_Rx);
                myBlynk.streamSelect(streamWebDdns);
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
    routerResetTimer        = millis();
    resetNetgeerAfterInternetLossTimer = millis();
    
    DEBUG_PRINT("Wifi: ");DEBUG_PRINTLN(wifiAvailable ? F("Available") : F("Not Available"));
    
    String smsStatus = smsSent ? F("Sim Available to send") : F("Sim Not Available to send");
    String startString = String( "Restarting "   NOTIFIER_ID   VERSION_ID + smsStatus ) ;
    if (blynkConnected) myBlynk.notifierDebug(NOTIFIER_ID, startString );
    createHandleGroup();
    looadRoomData();
    enableWDG(DIS);
    initWDG(SEC_60,EN);
}


void loop(void) 
{
       resetWdg();    //reset timer (feed watchdog) 
       
       if( smsEvent =sms.smsRun()) processSms();
       
       blynkConnected=myBlynk.blynkStatus(); 
       
       netgeerCtrl();
       
       if ( blynkConnected )
          {
            myBlynk.blynkRun();
      
            queuValidData = (xQueueReceive(g_event_queue_handle, &queuDataID, 5 / portTICK_RATE_MS) == pdPASS);
            if(queuValidData) 
                  {
                    myBlynk.getData ();
                    queuData = myBlynk.blynkData;
                    processBlynkQueu();}
            
            InternetLoss = false;   resetNetgeerAfterInternetLossTimer = millis();
            netGeerReset = false;   restartAfterResetNG = millis();
          }

       if( !InternetLoss && !blynkConnected)  
          {
            if(smsSent) smsSent= sms.SendSMS("Blynk Disconnected , Internet Loss!!!");
            DEBUG_PRINTLN("Blynk Disconnected , Internet Loss!!!");
            InternetLoss = true; 
            resetNetgeerAfterInternetLossTimer = millis();
            blynkEvent=false; 
            myBlynk.sendToBlynk = false;
            myBlynk.sendToBlynkLeds = false;
          }
    
      if (zapOnOff ) zappingAvCh (zapOnOff, zapTimer);  
          
       myBlynk.blynkRunTimer();
}

void netgeerCtrl(void)
{

                 
       if ( (  (millis() - routerResetTimer) >= routerTimer) && routerResetStart)
                {
                digitalWrite(NETGEER_PIN_0, HIGH);
                routerResetStart=false;
                routerResetTimer        = millis();
                restartAfterResetNG     = millis();
                netGeerReset = true;
                blynkInitDone = false;
                pingGoogle =false;
            }

       if ( ( (millis() - resetNetgeerAfterInternetLossTimer) >= INTERNET_LOSS_TO_RESET_NG_TIMER) && InternetLoss && !blynkConnected && !netGeerReset)
        {
              if(smsSent) smsSent= sms.SendSMS("Blynk Disconnected for 2 min, Reset Netgeer");
              DEBUG_PRINTLN("Blynk Disconnected for 2 min, Reset Netgeer");
              ResetNetgeer();
        }

       if (  ( (millis() - restartAfterResetNG) >=  RESTART_AFTER_NG_RESET_TIMER) && netGeerReset )
          {
            if(smsSent) smsSent= sms.SendSMS("Resetaring 3 min after Netgeer Reset");
            DEBUG_PRINTLN("Resetaring 3 min after Netgeer Rreset");
            ESP.restart(); 
          }
          
}     



void ResetNetgeer(void)
          {
              digitalWrite(NETGEER_PIN_0, LOW);
              if(!routerResetStart){routerResetTimer        = millis();routerResetStart = true;DEBUG_PRINTLN("Netgeer Reset done: ");}
          }


bool pingGoogleConnection(void)
{
       bool pingInternet= Ping.ping("www.google.com");
       DEBUG_PRINT("Ping Google: ");DEBUG_PRINTLN(pingInternet ? F("succesiful") : F("failed"));
       return (pingInternet);
}




/***************************************************************************/

void processBlynkQueu(void)
{
        switch (queuDataID)
          {

            case Q_EVENT_FREQ_V0:
              recevierFreq=queuData;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(queuData);
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            
            case Q_EVENT_T433_CH_NR_V1:
              remoteControlRcCh=queuData;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(queuData);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 15) {remoteControl(remoteControlRcCh );}
            break;      
                  
            case Q_EVENT_AV_7SEG_V2:
                recevierCh=queuData;
                DEBUG_PRINT("FB_AV_7SEG: ");DEBUG_PRINTLN(queuData);
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh (recevierCh);
            break;

           case Q_EVENT_OTA_V7:
              otaCmd=queuData;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(queuData);
              otaGsm ();
            break;
 
            case Q_EVENT_RESET_V8:
              rebootCmd=queuData;
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(queuData);
              rebootSw();
            break;
             
            case Q_EVENT_STREAMING_WEB_DDNS_V10:
                  streamWebDdns = queuData;
                  myBlynk.streamSelect(streamWebDdns);
             break;

            case Q_EVENT_T315_CH_NR_V14:
              remoteControlRcCh=queuData;
              DEBUG_PRINT("FB_T315_CH_NR: ");DEBUG_PRINTLN( (queuData) -15);
              if (remoteControlRcCh >= 16 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );}
            break;

            case Q_EVENT_NETGEER_V15  :
             myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset from Blynk");
              ResetNetgeer();
            break;

            case Q_EVENT_ROOM_201_TO_205_V3:
                  
                  switch (queuData)
                    {
                      case 1:
                      break;
                      
                      case 2: // ROOM 202
                            remoteControlRcCh = xRoom_202.rCh;
                            recevierCh        =xRoom_202.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 203
                            remoteControlRcCh = xRoom_203.rCh;
                            recevierCh        =xRoom_203.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;      
                      
                      case 4:// ROOM 204
                            remoteControlRcCh = xRoom_204.rCh;
                            recevierCh        =xRoom_204.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 205
                            remoteControlRcCh = xRoom_205.rCh;
                            recevierCh        =xRoom_205.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }
                    
            break;
            
            case Q_EVENT_ROOM_206_TO_210_V16:
                   switch (queuData)
                    {
                      case 1:
                      break;
                      
                      case 2:// ROOM 207
                            remoteControlRcCh = xRoom_207.rCh;
                            recevierCh        =xRoom_207.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 208
                            remoteControlRcCh = xRoom_208.rCh;
                            recevierCh        =xRoom_208.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;      
                      
                      case 4:// ROOM 209
                            remoteControlRcCh = xRoom_209.rCh;
                            recevierCh        =xRoom_209.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 210
                            remoteControlRcCh = xRoom_210.rCh;
                            recevierCh        =xRoom_210.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }

            break;
            
            case Q_EVENT_ROOM_211_TO_215_V17:
                   switch (queuData)
                    {
                      case 1:
                      break;
                      
                      case 2:// ROOM 212
                            remoteControlRcCh = xRoom_212.rCh;
                            recevierCh        =xRoom_212.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 213
                      break;      
                      
                      case 4:// ROOM 214
                            remoteControlRcCh = xRoom_214.rCh;
                            recevierCh        =xRoom_214.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 215
                            remoteControlRcCh = xRoom_215.rCh;
                            recevierCh        =xRoom_215.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }

            break;
            
            case Q_EVENT_ROOM_216_TO_220_V18:
                   switch (queuData)
                    {
                      case 1:// ROOM 216
                            remoteControlRcCh = xRoom_216.rCh;
                            recevierCh        =xRoom_216.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 2:// ROOM 217
                            remoteControlRcCh = xRoom_217.rCh;
                            recevierCh        =xRoom_217.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:
                      break;      
                      
                      case 4:// ROOM 219
                            remoteControlRcCh = xRoom_219.rCh;
                            recevierCh        =xRoom_219.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:

                      break;   
                    }

            break;                                    

            case Q_EVENT_ROOM_AV_RC_V19:
             Av_Rx=queuData;
             myBlynk.sendAvRxIndex(Av_Rx);
            break;
            case Q_EVENT_ROUTER_RESET_TIMER_V23:
                  routerTimer = queuData;
            break;


            
            case Q_EVENT_ZAP_V71:
              zapOnOff=queuData;
              DEBUG_PRINT("ZAP IS : ");
              DEBUG_PRINTLN(zapOnOff ? F("On") : F("Off"));
              myBlynk.zapLed(zapOnOff);
            break;

            case Q_EVENT_ZAP_TIMER_V72:
              zapTimer=queuData;
            break;

            case Q_EVENT_ZAP_CHANNEL1_V81 :
              Ch_1.zap=zapCh1=queuData;
            break;

             case Q_EVENT_ZAP_CHANNEL2_V82 :
              Ch_2.zap=zapCh2=queuData;
            break;

             case Q_EVENT_ZAP_CHANNEL3_V83 :
              Ch_3.zap=zapCh3=queuData;
            break;

             case Q_EVENT_ZAP_CHANNEL4_V84 :
              Ch_4.zap=zapCh4=queuData;
            break;

             case Q_EVENT_ZAP_CHANNEL5_V85 :
              Ch_5.zap=zapCh5=queuData;
            break;

             case Q_EVENT_ZAP_CHANNEL6_V86 :
              Ch_6.zap=zapCh6=queuData;
            break;

             case Q_EVENT_ZAP_CHANNEL7_V87 :
              Ch_7.zap=zapCh7=queuData;
            break;

             case Q_EVENT_ZAP_CHANNEL8_V88 :
              Ch_8.zap=zapCh8=queuData;
            break;
            
            case Q_EVENT_ZAP_CHANNEL9_V89 :
              Ch_9.zap=zapCh9=queuData;
            break; 

                      
            case Q_EVENT_AV_CH_PLUS_V90:
                recevierCh += 1;
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh (recevierCh);
            break;
                     
            
            case Q_EVENT_AV_CH_MINUS_V91:
                recevierCh -= 1;
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh (recevierCh);
            break;
            
             case Q_EVENT_AV_FR_MINUS_V92:
              recevierFreq -= 1;
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;

           case Q_EVENT_AV_FR_PLUS_V93:
              recevierFreq += 1;
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;


            case Q_EVENT_ZAP_CHANNEL10_V94 :
              Ch_10.zap=zapCh10=queuData;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL11_V95 :
              Ch_11.zap=zapCh11=queuData;
            break; 


            case Q_EVENT_RC_PULSE_V98:
             pulseRC=queuData;
             mySwitch.setPulseLength(pulseRC);
            break;

            case Q_EVENT_WIFI_IDE_V100:
               wifiIde = false;         
               wifiIDETimer = millis();
               wifiUploadCtrl();
             break;
             
            case Q_EVENT_RC_REPETION_V101:
             repetionRC=queuData;
             EEPROM.write(RC_REPETION_ADD, repetionRC); EEPROM.commit();
             mySwitch.setRepeatTransmit(repetionRC);
            break;


            
            case Q_EVENT_SLEEP_TIMER_V102:
             deepSleepTimerHours=queuData;
             goToDeepSleep(deepSleepTimerHours);
            break;

             
                          
            case Q_EVENT_WIFI_WEB_V104:
               wifiWebUpdater = false;
               wifiIDETimer = millis();
               webUpdateOta ();
             break;

             case Q_EVENT_WIFI_OTA_V105:
               otaWifiGithub = false;         
               wifiIDETimer = millis();
               otaWifi();
             break;
    }  
}


void processSms(void)
{
      boolean isValidNumber =false;
      int smsID=0;
      
      smsReceived =  sms.smsString;
      
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
          else if (smsReceived == "Dvron" )  smsID = FB_DVR_ON_ID ;
          else if (smsReceived == "Dvroff" )  smsID = FB_DVR_OFF_ID ;
        }
        switch (smsID)
          {
            case FB_DVR_ON_ID:
               dvrOnOff (true)  ;
             break;
             
             case FB_DVR_OFF_ID:
               dvrOnOff (false)  ;
             break;
                                                 
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
                if (recevierCh > 8) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = 8;
                receiverAvByCh (recevierCh);
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

            case FB_VERSION_ID:
              DEBUG_PRINT("FB_VERSION: ");DEBUG_PRINTLN(smsReceived);
              sendVersion();
            break;
    }  
}

void zappingAvCh (bool zapCmd, int zapTimer)
{
         switch (stateMachine)
          {
            case 0:
            case 1:
                if (Ch_1.zap ) 
                  {
                    if (stateMachine == 0) {zaptime= millis();stateMachine =1;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_1.id;receiverAvByCh (recevierCh);stateMachine =2;}
                  }
                else stateMachine =2;
            break;

            case 2:
            case 3:
                if (Ch_2.zap ) 
                  {
                    if (stateMachine == 2) {zaptime= millis();stateMachine =3;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_2.id;receiverAvByCh (recevierCh);stateMachine =4;}
                  }
                else stateMachine =4;
            break;
            
            case 4:
            case 5:
                if (Ch_3.zap ) 
                  {
                    if (stateMachine == 4) {zaptime= millis();stateMachine =5;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_3.id;receiverAvByCh (recevierCh);stateMachine =6;}
                  }
                else stateMachine =6;
            break;

            case 6:
            case 7:
                if (Ch_4.zap ) 
                  {
                    if (stateMachine == 6) {zaptime= millis();stateMachine =7;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_4.id;receiverAvByCh (recevierCh);stateMachine =8;}
                  }
                else stateMachine =8;
            break;
                       
            case 8:
            case 9:
                if (Ch_5.zap ) 
                  {
                    if (stateMachine == 8) {zaptime= millis();stateMachine =9;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_5.id;receiverAvByCh (recevierCh);stateMachine =10;}
                  }
                else stateMachine =10;
            break;
                       
            case 10:
            case 11:
                if (Ch_6.zap ) 
                  {
                    if (stateMachine == 10) {zaptime= millis();stateMachine =11;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_6.id;receiverAvByCh (recevierCh);stateMachine =12;}
                  }
                else stateMachine =12;
            break;
                       
            case 12:
            case 13:
                if (Ch_7.zap ) 
                  {
                    if (stateMachine == 12) {zaptime= millis();stateMachine =13;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_7.id;receiverAvByCh (recevierCh);stateMachine =14;}
                  }
                else stateMachine =14;
            break;
                       
            case 14:
            case 15:
                if (Ch_8.zap) 
                  {
                    if (stateMachine == 14) {zaptime= millis();stateMachine =15;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_8.id;receiverAvByCh (recevierCh);stateMachine =16;}
                  }
                else stateMachine =16;
            break;

            case 16:
            case 17:
                if (Ch_9.zap) 
                  {
                    if (stateMachine == 16) {zaptime= millis();stateMachine =17;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_9.id;receiverAvByCh (recevierCh);stateMachine =18;}
                  }
                else stateMachine =18;
            break;
            case 18:
            case 19:
                if (Ch_10.zap) 
                  {
                    if (stateMachine == 18) {zaptime= millis();stateMachine =19;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_10.id;receiverAvByCh (recevierCh);stateMachine =20;}
                  }
                else stateMachine =20;
            break; 
             
            case 20:
            case 21:
                if (Ch_11.zap) 
                  {
                    if (stateMachine == 20) {zaptime= millis();stateMachine =21;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_11.id;receiverAvByCh (recevierCh);stateMachine =0;}
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
       
      // ack = avReceiver.Tuner_PLL(av_pll_addr, PLL[Ch]); 
      
       ack = avReceiver.Tuner_PLL(av_pll_addr, _pll[Ch]); 
       delay(500);
       
       if (blynkConnected) {myBlynk.blynkAckLed(ack); myBlynk.sevenSegValue(Ch );}
      
        switch (Ch)
          {
            case 1:
               if (blynkConnected) myBlynk.frequencyValue(1080 );
               recevierFreq =Ch_1.frequency;
            break;

            case 2:
                if (blynkConnected)myBlynk.frequencyValue(1120 );
                recevierFreq =Ch_2.frequency;
            break;

            
            case 3:
               if (blynkConnected) myBlynk.frequencyValue(1160 );
               recevierFreq =Ch_3.frequency;
            break;


            case 4:
               if (blynkConnected) myBlynk.frequencyValue(1200 );
               recevierFreq =Ch_4.frequency;
            break;

                       
            case 5:
              if (blynkConnected)  myBlynk.frequencyValue(1240 );
              recevierFreq =Ch_5.frequency;
            break;

                       
            case 6:
              if (blynkConnected)  myBlynk.frequencyValue(1280 );
              recevierFreq =Ch_6.frequency;
            break;

                       
            case 7:
              if (blynkConnected)  myBlynk.frequencyValue(1320 );
              recevierFreq =Ch_7.frequency;
            break;

                       
            case 8:
              if (blynkConnected)  myBlynk.frequencyValue(1360 );
              recevierFreq =Ch_8.frequency;
            break;
            
            case 9:
              if (blynkConnected)  myBlynk.frequencyValue(1000);
              recevierFreq =Ch_9.frequency;
            break;
            
            case 10:
              if (blynkConnected)  myBlynk.frequencyValue(1040);
              recevierFreq =Ch_10.frequency;
            break;
            
            case 11:
              if (blynkConnected)  myBlynk.frequencyValue(1040);
              recevierFreq =Ch_11.frequency;
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
       ack = avReceiver.Tuner_PLL(av_pll_addr, PLL_value);
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



/**********************************************************************************************************************************************/

void sendToHMI(char *smsmsg, String notifier_subject, String notifier_body,String fb_path,String fb_cmdString)
{
  if(smsSent) smsSent=sms.SendSMS(smsmsg);
  if (blynkConnected) myBlynk.notifierDebug(NOTIFIER_ID, notifier_body);
  DEBUG_PRINTLN(notifier_body);
}


void rebootSw(void)
{
 ESP.restart();
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
      sms.sim800PowerOn(false)  ;
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
  EEPROM.write(EEPROM_GITHUB_ADD, 1); EEPROM.commit();
  server.begin();
 }

 void webUpdateOta (void)
 {
   webOtaSetup();
   EEPROM.write(EEPROM_GITHUB_ADD, 1); EEPROM.commit();
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
  EEPROM.write(EEPROM_GITHUB_ADD, 1); EEPROM.commit();
  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 }


void wifiUploadCtrl(void)
{
       otaIdeSetup () ;   
        EEPROM.write(EEPROM_GITHUB_ADD, 1); EEPROM.commit();
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
  DEBUG_PRINTLN("Starting Ota Web Update from Github");
  sendToHMI("Ota web Started", "Ota Web : ", "Ota web Started",FB_NOTIFIER, "Ota web Started" );
  EEPROM.write(EEPROM_GITHUB_ADD, 1); EEPROM.commit();
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

void  dvrOnOff (bool onOff)
{
   if (onOff) digitalWrite(AV_RX_DVR_PIN_2, LOW); 
   else digitalWrite(AV_RX_DVR_PIN_2, HIGH); 
}
