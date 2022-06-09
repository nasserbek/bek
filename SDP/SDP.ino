#include "main.h"//Blynk IoT


 reciever avReceiver;
 sim800L sms; 
 otaUpload ota; 
 blynk myBlynk;
 fireBase fb;

#include <ESP32httpUpdate.h>
#define USE_SERIAL Serial
 
bool sendTime_7500ms;
int _days  ;
int _hours;
int _minutes ;
int _seconds ;

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
       for(byte i=1;i<21;i++)
          {
            videoCh[i].frequency = freqTable[i];
            freq=videoCh[i].frequency;
            roomId[i].vCh  = roomId[i].rCh = videoCh[i].id = i;
            _pll[i] = ( 512 * (freq + 479.5) ) / 64 ;
          }
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
     pinMode(AV_RX_DVR_PIN_2, OUTPUT);



     digitalWrite(NETGEER_PIN_0, LOW);// NC ACTIVATE ON POWER ON BY DOING NOTHING
     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // NC DISACTIVATE AV RECEIVER ON POWER ON


     Serial.begin(115200);
     
     avReceiver.init_I2C();
     
     EEPROM.begin(EEPROM_SIZE);
     
     initWDG(MIN_5,EN);
     resetWdg();    //reset timer (feed watchdog) 
     
     sim800Available = sms.init();
     mySwitch.enableTransmit(RC_TX_PIN);

     smsSent= sms.SendSMS("Sim800 Ok, Connecting to WIFI and Blynk.....");
     
     myBlynk.init();    
     blynkConnected=myBlynk.blynkStatus();
     wifiAvailable = myBlynk.wifiStatus();
     
     if (wifiAvailable) {if(smsSent) smsSent= sms.SendSMS("WIFI Connected..");}
     else {if(smsSent) smsSent= sms.SendSMS("WIFI failed to connect");}
          
     if (blynkConnected) {if(smsSent) smsSent= sms.SendSMS("BLYNK Connected...");}
     else {if(smsSent) smsSent= sms.SendSMS("BLYNK failed to connect...");}


     DEBUG_PRINT("Blynk: ");DEBUG_PRINTLN( blynkConnected ? F("Connected") : F("Not Connected"));
     if (blynkConnected) 
              {
                myBlynk.blynkSmsLed (sim800Available & smsSent);
                myBlynk.sendAvRxIndex(Av_Rx);
                myBlynk.streamSelect(streamWebDdns);
                myBlynk.sendPulseRepetetion(pulseRC, repetionRC);
             }
      else  sendToHMI("Internet failure", "Internet failure : ", "Internet failure",FB_NOTIFIER, "Internet failure" );
    
      if (!wifiAvailable) sendToHMI("Wifi failure", "Wifi failure: ", "Wifi failure",FB_NOTIFIER, "Wifi failure" );
      else if(fireBaseOn)
        {
          FBConnected = fb.init();
          if(FBConnected ) smsSent= sms.SendSMS("FireBase connected...");
          else sms.SendSMS("FireBase Failed to connect...!!!");
        }
      
      
    Sms_24_hoursTimer       = millis();
    internetSurvilanceTimer = millis();
    liveTimerOff            = millis();
    liveTimerOn             = millis();
    wifiIDETimer            = millis();
    restartAfterResetNG     = millis();
    NetgeerResetGooglLostTimer = millis();
    blynkNotActiveTimer     = millis();
    routerResetTimer        = millis();
    resetNetgeerAfterInternetLossTimer = millis();
   
    createHandleGroup();
    looadRoomData();
    enableWDG(DIS);
    initWDG(SEC_60,EN);
    
    String ver = VERSION_ID;
    if (blynkConnected) myBlynk.sendVersion(ver);
    
    char buf[10]; //make this the size of the String
    ver.toCharArray(buf, 10);    
    if(smsSent) smsSent= sms.SendSMS(buf);
//    if (blynkConnected) {myBlynk.SyncAll();}
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
                    processBlynkQueu();
                  }

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

      if (FBConnected ) 
          { 
            if(fbEvent = fb.firebaseRun()) processFirebase(); 
            
            if(sendTime_7500ms)
            {
             fb.SendString (FB_SECONDS, String(_seconds) ) ;
             sendTime_7500ms = false;
            }
          }
      
      if (zapOnOff ) zappingAvCh (zapOnOff, zapTimer);  
      
      if ( (millis() - Sms_24_hoursTimer) >=  SMS_24_HOURS  )
          {
            Sms_24_hoursTimer       = millis();
            sms.SendSMS("VTR Alive");
           }
   
       myBlynk.blynkRunTimer();
}

void netgeerCtrl(void)
{

                 
       if ( (  (millis() - routerResetTimer) >= routerTimer) && routerResetStart)
                {

              digitalWrite(NETGEER_PIN_0, LOW);
     
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
//              ResetNetgeer();   //No more Router reset, can be done manually
                if(!routerResetStart){routerResetTimer        = millis();routerResetStart = true;DEBUG_PRINTLN("Netgeer Reset done: ");}
        }

       if (  ( (millis() - restartAfterResetNG) >=  RESTART_AFTER_NG_RESET_TIMER) && netGeerReset )
          {
            if(smsSent) smsSent= sms.SendSMS("Resetaring 3 min after Netgeer Reset");
            DEBUG_PRINTLN("Resetaring 30 min after Netgeer Rreset");
            ESP.restart(); 
          }
          
}     



void ResetNetgeer(void)
          {
              digitalWrite(NETGEER_PIN_0, HIGH);
              if(!routerResetStart){routerResetTimer        = millis();routerResetStart = true;DEBUG_PRINTLN("Netgeer Reset done: ");}
          }


bool pingGoogleConnection(void)
{
       bool pingInternet= Ping.ping("www.google.com");
       DEBUG_PRINT("Ping Google: ");DEBUG_PRINTLN(pingInternet ? F("succesiful") : F("failed"));
       return (pingInternet);
}



/********************************** FireBase **************************************/
void processFirebase(void)
{
        switch (fb.eventID)
          {

            case Q_EVENT_FREQ_V0:
              recevierFreq=fb.eventValue;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(queuData);
              //if (recevierFreq >= 920 && recevierFreq <= 1500) 
              receiverAvByFreq (recevierFreq);
            break;
            
            case Q_EVENT_RC_CH_NR_V1:
              remoteControlRcCh=fb.eventValue;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(queuData);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 15) {remoteControl(remoteControlRcCh );}
            break;      
                  
            case Q_EVENT_VIDEO_CH_V2:
                recevierCh=fb.eventValue;
                DEBUG_PRINT("FB_VIDEO_CH_PATH: ");DEBUG_PRINTLN(queuData);
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh (recevierCh);
            break;

            case Q_EVENT_T315_CH_NR_V14:
              remoteControlRcCh=fb.eventValue;
              DEBUG_PRINT("FB_T315_CH_NR: ");DEBUG_PRINTLN( (queuData) -15);
              if (remoteControlRcCh >= 16 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );}
            break;



            case Q_EVENT_ROOM_ID_1_TO_5_V3:
                  remoteControlRcCh = fb.eventValue;
                  recevierCh        = fb.eventValue;
                  room (remoteControlRcCh, recevierCh , Av_Rx );
           break;
            
            case Q_EVENT_ROOM_ID_6_TO_10_V16:
                  remoteControlRcCh = fb.eventValue+5;
                  recevierCh        = fb.eventValue+5;
                  room (remoteControlRcCh, recevierCh , Av_Rx );            
            break;
            
            case Q_EVENT_ROOM_ID_11_TO_15_V17:
                  remoteControlRcCh = fb.eventValue+10;
                  recevierCh        = fb.eventValue+10;
                  room (remoteControlRcCh, recevierCh , Av_Rx );             
            break;
            
            case Q_EVENT_ROOM_ID_16_TO_20_V18:
                  remoteControlRcCh = fb.eventValue+15;
                  recevierCh        = fb.eventValue+15;
                  room (remoteControlRcCh, recevierCh , Av_Rx );                
            break;                                    
 
            case Q_EVENT_ROOM_ID_21_25_V25:
                  remoteControlRcCh = fb.eventValue+20;
                  recevierCh        = fb.eventValue+20;
                  room (remoteControlRcCh, recevierCh , Av_Rx );               
            break;     
          
  
            case Q_EVENT_ROOM_AV_RC_V19:
                Av_Rx=fb.eventValue;
                myBlynk.sendAvRxIndex(Av_Rx);
            break;

                                             
            case Q_EVENT_ZAP_V71:
              zapOnOff=fb.eventValue;
              DEBUG_PRINT("ZAP IS : ");
              DEBUG_PRINTLN(zapOnOff ? F("On") : F("Off"));
              myBlynk.zapLed(zapOnOff);
            break;

            case Q_EVENT_ZAP_CHANNEL1_V81 :
              videoCh[1].zap=fb.eventValue;
            break;

             case Q_EVENT_ZAP_CHANNEL2_V82 :
              videoCh[2].zap=fb.eventValue;
              
            break;

             case Q_EVENT_ZAP_CHANNEL3_V83 :
              videoCh[3].zap=fb.eventValue;

            break;

             case Q_EVENT_ZAP_CHANNEL4_V84 :
              videoCh[4].zap=fb.eventValue;

            break;

             case Q_EVENT_ZAP_CHANNEL5_V85 :
              videoCh[5].zap=fb.eventValue;

            break;

             case Q_EVENT_ZAP_CHANNEL6_V86 :
              videoCh[6].zap=fb.eventValue;

            break;

             case Q_EVENT_ZAP_CHANNEL7_V87 :
              videoCh[7].zap=fb.eventValue;
              
            break;

             case Q_EVENT_ZAP_CHANNEL8_V88 :
              videoCh[8].zap=fb.eventValue;
              
            break;
            
            case Q_EVENT_ZAP_CHANNEL9_V89 :
              videoCh[9].zap=fb.eventValue;
            break; 

            case Q_EVENT_ZAP_CHANNEL10_V94 :
              videoCh[10].zap=fb.eventValue;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL11_V95 :
              videoCh[11].zap=fb.eventValue;
            break; 

            case Q_EVENT_ZAP_CHANNEL12_V96 :
              videoCh[12].zap=fb.eventValue;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL13_V97 :
              videoCh[13].zap=fb.eventValue;
            break; 
            
            case Q_EVENT_ZAP_CHANNEL14_V106 :
              videoCh[14].zap=fb.eventValue;
            break; 

             case Q_EVENT_ZAP_CHANNEL15_V107 :
              videoCh[15].zap=fb.eventValue;
            break; 
            
            case Q_EVENT_ZAP_CHANNEL16_V108 :
              videoCh[16].zap=fb.eventValue;
            break; 

             case Q_EVENT_ZAP_CHANNEL17_V109 :
              videoCh[17].zap=fb.eventValue;
            break; 

            case Q_EVENT_ZAP_CHANNEL18_V110 :
              videoCh[18].zap=fb.eventValue;
            break;             

             case Q_EVENT_ZAP_CHANNEL19_V111 :
              videoCh[19].zap=fb.eventValue;
            break; 
 
             case Q_EVENT_ZAP_CHANNEL20_V112 :
              videoCh[20].zap=fb.eventValue;
            break; 
                       

             case Q_EVENT_OTA_GITHUB_V105:
               otaWifiGithub = false;         
               wifiIDETimer = millis();
               otaWifi();
             break;
             
           case Q_EVENT_OTA_GSM_V7:
              otaCmd=fb.eventValue;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(queuData);
              otaGsm ();
            break;
 
            case Q_EVENT_REBOOT_V8:
              rebootCmd=fb.eventValue;
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(queuData);
              rebootSw();
            break;
            
            case Q_EVENT_NETGEER_V15  :
          //   myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset from Blynk");
              ResetNetgeer();
            break;                                        
    }  
        
}    

/********************************************Blynk *******************************/

void processBlynkQueu(void)
{
        switch (queuDataID)
          {

            case Q_EVENT_FREQ_V0:
              recevierFreq=queuData;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(queuData);
              //if (recevierFreq >= 920 && recevierFreq <= 1500) 
              receiverAvByFreq (recevierFreq);
            break;
            
            case Q_EVENT_RC_CH_NR_V1:
              remoteControlRcCh=queuData;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(queuData);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 15) {remoteControl(remoteControlRcCh );}
            break;      
                  
            case Q_EVENT_VIDEO_CH_V2:
                recevierCh=queuData;
                DEBUG_PRINT("FB_VIDEO_CH_PATH: ");DEBUG_PRINTLN(queuData);
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh (recevierCh);
                
            break;

           case Q_EVENT_OTA_GSM_V7:
              otaCmd=queuData;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(queuData);
              otaGsm ();
            break;
 
            case Q_EVENT_REBOOT_V8:
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

            case Q_EVENT_ROOM_ID_1_TO_5_V3:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room (remoteControlRcCh, recevierCh , Av_Rx );
           break;
            
            case Q_EVENT_ROOM_ID_6_TO_10_V16:
                  remoteControlRcCh = queuData+5;
                  recevierCh        = queuData+5;
                  room (remoteControlRcCh, recevierCh , Av_Rx );            
            break;
            
            case Q_EVENT_ROOM_ID_11_TO_15_V17:
                  remoteControlRcCh = queuData+10;
                  recevierCh        = queuData+10;
                  room (remoteControlRcCh, recevierCh , Av_Rx );             
            break;
            
            case Q_EVENT_ROOM_ID_16_TO_20_V18:
                  remoteControlRcCh = queuData+15;
                  recevierCh        = queuData+15;
                  room (remoteControlRcCh, recevierCh , Av_Rx );                
            break;                                    

            case Q_EVENT_ROOM_AV_RC_V19:
             Av_Rx=queuData;
             myBlynk.sendAvRxIndex(Av_Rx);
            break;
            
            case Q_EVENT_ROUTER_RESET_TIMER_V23:
                  routerTimer = queuData;
            break;

 
            case Q_EVENT_ROOM_ID_21_25_V25:
                  remoteControlRcCh = queuData+20;
                  recevierCh        = queuData+20;
                  room (remoteControlRcCh, recevierCh , Av_Rx );               
            break;     
            
                    
             case Q_EVENT_RESET_FREQ_V26:
                  recevierFreq = videoCh[recevierCh].frequency =   freqTable[recevierCh];   
                  receiverAvByFreq (recevierFreq);        
            break;         
                 
            case Q_EVENT_DVR_ON_OFF_V27:
                  dvrOnOff (queuData);  
            break;   

            case Q_EVENT_03_SEL_V30:  //ROOM 3
                  videoCh[13].mux=queuData;  
            break;   

            case Q_EVENT_21_SEL_V31:   // ROOM 1
                  videoCh[11].mux=queuData;  
            break;  

            case Q_EVENT_27_SEL_V32: // ROOM 27
                  videoCh[14].mux=queuData;  
            break;   

            case Q_EVENT_50_SEL_V33:  //ROOM 50
                  videoCh[7].mux=queuData;  
            break;  

            case Q_EVENT_20_SEL_V34:  //ROOM 51
                  videoCh[8].mux=queuData;  
            break;   

            case Q_EVENT_52_SEL_V35: // ROOM 31
                  videoCh[15].mux=queuData;  
            break;                  
 
             case Q_EVENT_53_SEL_V36:  // ROOM 48
                  videoCh[5].mux=queuData;  
            break;   

            case Q_EVENT_22_SEL_V37:  //ROOM 22
                  videoCh[16].mux=queuData;  
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
              videoCh[1].zap=queuData;
              
            break;

             case Q_EVENT_ZAP_CHANNEL2_V82 :
              videoCh[2].zap=queuData;
              
            break;

             case Q_EVENT_ZAP_CHANNEL3_V83 :
              videoCh[3].zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL4_V84 :
              videoCh[4].zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL5_V85 :
              videoCh[5].zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL6_V86 :
              videoCh[6].zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL7_V87 :
              videoCh[7].zap=queuData;
              
            break;

             case Q_EVENT_ZAP_CHANNEL8_V88 :
              videoCh[8].zap=queuData;
              
            break;
            
            case Q_EVENT_ZAP_CHANNEL9_V89 :
              videoCh[9].zap=queuData;
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
              //if (recevierFreq >= 920 && recevierFreq <= 1500) 
              receiverAvByFreq (recevierFreq);
            break;

           case Q_EVENT_AV_FR_PLUS_V93:
              recevierFreq += 1;
              //if (recevierFreq >= 920 && recevierFreq <= 1500) 
              receiverAvByFreq (recevierFreq);
            break;


            case Q_EVENT_ZAP_CHANNEL10_V94 :
              videoCh[10].zap=queuData;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL11_V95 :
              videoCh[11].zap=queuData;
            break; 

            case Q_EVENT_ZAP_CHANNEL12_V96 :
              videoCh[12].zap=queuData;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL13_V97 :
              videoCh[13].zap=queuData;
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

             case Q_EVENT_OTA_GITHUB_V105:
               otaWifiGithub = false;         
               wifiIDETimer = millis();
               otaWifi();
             break;

            case Q_EVENT_ZAP_CHANNEL14_V106 :
              videoCh[14].zap=queuData;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL15_V107 :
              videoCh[15].zap=queuData;
            break; 

            
            case Q_EVENT_ZAP_CHANNEL16_V108 :
              videoCh[16].zap=queuData;
            break; 

             case Q_EVENT_ZAP_CHANNEL17_V109 :
              videoCh[17].zap=queuData;
            break; 

            case Q_EVENT_ZAP_CHANNEL18_V110 :
              videoCh[18].zap=queuData;
            break;             

             case Q_EVENT_ZAP_CHANNEL19_V111 :
              videoCh[19].zap=queuData;
            break; 
 
             case Q_EVENT_ZAP_CHANNEL20_V112 :
              videoCh[20].zap=queuData;
            break; 
                       
            
    }  
}


void processSms(void)
{
    boolean isValidNumber =false;
      int smsID=0;
      bool dvr = false;
      
      smsReceived =  sms.smsString;
      
       for(byte i=0;i<smsReceived.length();i++)
          {
            if(isDigit(smsReceived.charAt(i))) isValidNumber =true;
            else isValidNumber=false;
          }
        if(isValidNumber)
        {
          smsValue = stringToInteger(smsReceived);
          if (smsValue >= 900 && smsValue <= 5000) smsID =Q_EVENT_FREQ_V0;
          if (smsValue >= 1 && smsValue <= 30)     smsID =Q_EVENT_RC_CH_NR_V1;
          if (smsValue >= 41 && smsValue <= 60)    smsID =Q_EVENT_VIDEO_CH_V2;
        }
        else
        {
          if      (smsReceived =="Reboot")        smsID = Q_EVENT_REBOOT_V8;
          
          else if (smsReceived == "Netgeer" )     smsID = Q_EVENT_NETGEER_V15;

          else if (smsReceived =="Otagsm")        smsID = Q_EVENT_OTA_GSM_V7;
          
          else if (smsReceived == "Otagithub" )   smsID = Q_EVENT_OTA_GITHUB_V105  ;
          
           else if (smsReceived == "WifiIde" )      smsID = Q_EVENT_WIFI_IDE_V100  ;
           
          else if (smsReceived == "WifiLocalWebOta" )      smsID = Q_EVENT_WIFI_WEB_V104  ;
          
          else if (smsReceived == "Preset" )      smsID = Q_EVENT_RESET_FREQ_V26  ;
                                
          else if (smsReceived == "Dvr on" )      {smsValue = 1; smsID = Q_EVENT_DVR_ON_OFF_V27;}
          else if (smsReceived == "Dvr off" )     {smsValue = 0; smsID = Q_EVENT_DVR_ON_OFF_V27;}
                             
          else if (smsReceived == "Av" )         {smsID = Q_EVENT_ROOM_AV_RC_V19 ;smsValue = 1;}
          else if (smsReceived == "Rc" )         {smsID = Q_EVENT_ROOM_AV_RC_V19 ;smsValue = 2;}
          else if (smsReceived == "Both" )       {smsID = Q_EVENT_ROOM_AV_RC_V19 ;smsValue = 3;}
          
          else if (smsReceived == "Ch1" )        {smsID = Q_EVENT_ROOM_ID_1_TO_5_V3 ;smsValue = 1;}
          else if (smsReceived == "Ch2" )        {smsID = Q_EVENT_ROOM_ID_1_TO_5_V3 ;smsValue = 2;}
          else if (smsReceived == "Ch3" )        {smsID = Q_EVENT_ROOM_ID_1_TO_5_V3 ;smsValue = 3;}
          else if (smsReceived == "Ch4" )        {smsID = Q_EVENT_ROOM_ID_1_TO_5_V3 ;smsValue = 4;}
          else if (smsReceived == "Ch5" )        {smsID = Q_EVENT_ROOM_ID_1_TO_5_V3 ;smsValue = 5;}
          
          else if (smsReceived == "Ch6" )        {smsID = Q_EVENT_ROOM_ID_6_TO_10_V16 ;smsValue = 1;}
          else if (smsReceived == "Ch7" )        {smsID = Q_EVENT_ROOM_ID_6_TO_10_V16 ;smsValue = 2;}
          else if (smsReceived == "Ch8" )        {smsID = Q_EVENT_ROOM_ID_6_TO_10_V16 ;smsValue = 3;}
          else if (smsReceived == "Ch9" )        {smsID = Q_EVENT_ROOM_ID_6_TO_10_V16 ;smsValue = 4;}
          else if (smsReceived == "Ch10" )        {smsID = Q_EVENT_ROOM_ID_6_TO_10_V16 ;smsValue = 5;}

          else if (smsReceived == "Ch11" )        {smsID = Q_EVENT_ROOM_ID_11_TO_15_V17 ;smsValue = 1;}
          else if (smsReceived == "Ch12" )        {smsID = Q_EVENT_ROOM_ID_11_TO_15_V17 ;smsValue = 2;}
          else if (smsReceived == "Ch13" )        {smsID = Q_EVENT_ROOM_ID_11_TO_15_V17 ;smsValue = 3;}
          else if (smsReceived == "Ch14" )        {smsID = Q_EVENT_ROOM_ID_11_TO_15_V17 ;smsValue = 4;}
          else if (smsReceived == "Ch15" )        {smsID = Q_EVENT_ROOM_ID_11_TO_15_V17 ;smsValue = 5;}

          else if (smsReceived == "Ch16" )        {smsID = Q_EVENT_ROOM_ID_16_TO_20_V18 ;smsValue = 1;}
          else if (smsReceived == "Ch17" )        {smsID = Q_EVENT_ROOM_ID_16_TO_20_V18 ;smsValue = 2;}
          else if (smsReceived == "Ch18" )        {smsID = Q_EVENT_ROOM_ID_16_TO_20_V18 ;smsValue = 3;}
          else if (smsReceived == "Ch19" )        {smsID = Q_EVENT_ROOM_ID_16_TO_20_V18 ;smsValue = 4;}
          else if (smsReceived == "Ch20" )        {smsID = Q_EVENT_ROOM_ID_16_TO_20_V18 ;smsValue = 5;}

          else if (smsReceived == "Ch21" )        {smsID = Q_EVENT_ROOM_ID_21_25_V25 ;smsValue = 1;}
          else if (smsReceived == "Ch22" )        {smsID = Q_EVENT_ROOM_ID_21_25_V25 ;smsValue = 2;}
          else if (smsReceived == "Ch23" )        {smsID = Q_EVENT_ROOM_ID_21_25_V25 ;smsValue = 3;}
          else if (smsReceived == "Ch24" )        {smsID = Q_EVENT_ROOM_ID_21_25_V25 ;smsValue = 4;}
          else if (smsReceived == "Ch25" )        {smsID = Q_EVENT_ROOM_ID_21_25_V25 ;smsValue = 5;}
              
          
        }
   
        switch (smsID)
          {
            case Q_EVENT_FREQ_V0:
              recevierFreq=smsValue;
              DEBUG_PRINT("FREQ: ");DEBUG_PRINTLN(smsValue);
              receiverAvByFreq (recevierFreq);
            break;
            
            case Q_EVENT_RC_CH_NR_V1:
              remoteControlRcCh=smsValue;
              DEBUG_PRINT("T433_CH_NR: ");DEBUG_PRINTLN(smsValue);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );}
            break;      
                  
            case Q_EVENT_VIDEO_CH_V2:
                recevierCh=smsValue;
                DEBUG_PRINT("VIDEO_CH_PATH: ");DEBUG_PRINTLN(smsValue);
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh (recevierCh);
                
            break;

           case Q_EVENT_OTA_GSM_V7:
              DEBUG_PRINT("OTA: ");DEBUG_PRINTLN("Ota Gsm");
              otaGsm ();
            break;
 
            case Q_EVENT_REBOOT_V8:
              rebootCmd=smsValue;
              DEBUG_PRINT("RESET: ");DEBUG_PRINTLN("Reboot ESP32");
              rebootSw();
            break;
             

            case Q_EVENT_NETGEER_V15  :
              ResetNetgeer();
            break;

            case Q_EVENT_ROOM_ID_1_TO_5_V3:
                  remoteControlRcCh = smsValue;
                  recevierCh        = smsValue;
                  room (remoteControlRcCh, recevierCh , Av_Rx );
           break;
            
            case Q_EVENT_ROOM_ID_6_TO_10_V16:
                  remoteControlRcCh = smsValue+5;
                  recevierCh        = smsValue+5;
                  room (remoteControlRcCh, recevierCh , Av_Rx );            
            break;
            
            case Q_EVENT_ROOM_ID_11_TO_15_V17:
                  remoteControlRcCh = smsValue+10;
                  recevierCh        = smsValue+10;
                  room (remoteControlRcCh, recevierCh , Av_Rx );             
            break;
            
            case Q_EVENT_ROOM_ID_16_TO_20_V18:
                  remoteControlRcCh = smsValue+15;
                  recevierCh        = smsValue+15;
                  room (remoteControlRcCh, recevierCh , Av_Rx );                
            break;                                    

            case Q_EVENT_ROOM_AV_RC_V19:
             Av_Rx=smsValue;
             myBlynk.sendAvRxIndex(Av_Rx);
            break;
            
            case Q_EVENT_ROUTER_RESET_TIMER_V23:
                  routerTimer = smsValue;
            break;

 
            case Q_EVENT_ROOM_ID_21_25_V25:
                  remoteControlRcCh = smsValue+20;
                  recevierCh        = smsValue+20;
                  room (remoteControlRcCh, recevierCh , Av_Rx );               
            break;     
            
                    
             case Q_EVENT_RESET_FREQ_V26:
                  recevierFreq = videoCh[recevierCh].frequency =   freqTable[recevierCh];   
                  receiverAvByFreq (recevierFreq);        
            break;         
                 
            case Q_EVENT_DVR_ON_OFF_V27:
                  dvrOnOff (smsValue);  
            break;   

            case Q_EVENT_03_SEL_V30:  //ROOM 3
                  videoCh[13].mux=smsValue;  
            break;   

            case Q_EVENT_21_SEL_V31:   // ROOM 1
                  videoCh[11].mux=smsValue;  
            break;  

            case Q_EVENT_27_SEL_V32: // ROOM 27
                  videoCh[14].mux=smsValue;  
            break;   

            case Q_EVENT_50_SEL_V33:  //ROOM 50
                  videoCh[7].mux=smsValue;  
            break;  

            case Q_EVENT_20_SEL_V34:  //ROOM 51
                  videoCh[8].mux=smsValue;  
            break;   

            case Q_EVENT_52_SEL_V35: // ROOM 31
                  videoCh[15].mux=smsValue;  
            break;                  
 
             case Q_EVENT_53_SEL_V36:  // ROOM 48
                  videoCh[5].mux=smsValue;  
            break;   

            case Q_EVENT_22_SEL_V37:  //ROOM 22
                  videoCh[16].mux=smsValue;  
            break;     

                                                                      
            case Q_EVENT_ZAP_V71:
              zapOnOff=smsValue;
              DEBUG_PRINT("ZAP IS : ");
              DEBUG_PRINTLN(zapOnOff ? F("On") : F("Off"));
              myBlynk.zapLed(zapOnOff);
            break;

            case Q_EVENT_ZAP_TIMER_V72:
              zapTimer=smsValue;
            break;

            case Q_EVENT_ZAP_CHANNEL1_V81 :
              videoCh[1].zap=smsValue;
              
            break;

             case Q_EVENT_ZAP_CHANNEL2_V82 :
              videoCh[2].zap=smsValue;
              
            break;

             case Q_EVENT_ZAP_CHANNEL3_V83 :
              videoCh[3].zap=smsValue;

            break;

             case Q_EVENT_ZAP_CHANNEL4_V84 :
              videoCh[4].zap=smsValue;

            break;

             case Q_EVENT_ZAP_CHANNEL5_V85 :
              videoCh[5].zap=smsValue;

            break;

             case Q_EVENT_ZAP_CHANNEL6_V86 :
              videoCh[6].zap=smsValue;

            break;

             case Q_EVENT_ZAP_CHANNEL7_V87 :
              videoCh[7].zap=smsValue;
              
            break;

             case Q_EVENT_ZAP_CHANNEL8_V88 :
              videoCh[8].zap=smsValue;
              
            break;
            
            case Q_EVENT_ZAP_CHANNEL9_V89 :
              videoCh[9].zap=smsValue;
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
              //if (recevierFreq >= 920 && recevierFreq <= 1500) 
              receiverAvByFreq (recevierFreq);
            break;

           case Q_EVENT_AV_FR_PLUS_V93:
              recevierFreq += 1;
              //if (recevierFreq >= 920 && recevierFreq <= 1500) 
              receiverAvByFreq (recevierFreq);
            break;


            case Q_EVENT_ZAP_CHANNEL10_V94 :
              videoCh[10].zap=smsValue;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL11_V95 :
              videoCh[11].zap=smsValue;
            break; 

            case Q_EVENT_ZAP_CHANNEL12_V96 :
              videoCh[12].zap=smsValue;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL13_V97 :
              videoCh[13].zap=smsValue;
            break; 
            
            case Q_EVENT_RC_PULSE_V98:
             pulseRC=smsValue;
             mySwitch.setPulseLength(pulseRC);
            break;

            case Q_EVENT_WIFI_IDE_V100:
               wifiIde = false;         
               wifiIDETimer = millis();
               wifiUploadCtrl();
             break;
             
            case Q_EVENT_RC_REPETION_V101:
             repetionRC=smsValue;
             mySwitch.setRepeatTransmit(repetionRC);
            break;

           
            case Q_EVENT_SLEEP_TIMER_V102:
             deepSleepTimerHours=smsValue;
             goToDeepSleep(deepSleepTimerHours);
            break;
                          
            case Q_EVENT_WIFI_WEB_V104:
               wifiWebUpdater = false;
               wifiIDETimer = millis();
               webUpdateOta ();
             break;

             case Q_EVENT_OTA_GITHUB_V105:
               otaWifiGithub = false;         
               wifiIDETimer = millis();
               otaWifi();
             break;

            case Q_EVENT_ZAP_CHANNEL14_V106 :
              videoCh[14].zap=smsValue;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL15_V107 :
              videoCh[15].zap=smsValue;
            break; 

            
            case Q_EVENT_ZAP_CHANNEL16_V108 :
              videoCh[16].zap=smsValue;
            break; 

             case Q_EVENT_ZAP_CHANNEL17_V109 :
              videoCh[17].zap=smsValue;
            break; 

            case Q_EVENT_ZAP_CHANNEL18_V110 :
              videoCh[18].zap=smsValue;
            break;             

             case Q_EVENT_ZAP_CHANNEL19_V111 :
              videoCh[19].zap=smsValue;
            break; 
 
             case Q_EVENT_ZAP_CHANNEL20_V112 :
              videoCh[20].zap=smsValue;
            break; 
                       
          
    }  

    smsID = NONE;
}


void zappingAvCh (bool zapCmd, int zapTimer)
{
 if( zapCmd )
  { 
         switch (stateMachine)
          {
            case SM_CH1_A:    //44
            case SM_CH1_B:
                if (videoCh[1].zap ) 
                  {
                    if (stateMachine == SM_CH1_A) {zaptime= millis();stateMachine =SM_CH1_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=videoCh[1].id;
                        receiverAvByCh (recevierCh);
                        stateMachine =SM_CH2_A;
                      }
                  }
                else stateMachine =SM_CH2_A;
            break;

            case SM_CH2_A:    //45
            case SM_CH2_B:
                if (videoCh[2].zap ) 
                  {
                    if (stateMachine == SM_CH2_A) {zaptime= millis();stateMachine =SM_CH2_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=videoCh[2].id;
                        receiverAvByCh (recevierCh);
                        stateMachine =SM_CH3_A;
                     }
                  }
                else stateMachine =SM_CH3_A;
            break;
            
            case SM_CH3_A:    //46
            case SM_CH3_B:
                if (videoCh[3].zap ) 
                  {
                    if (stateMachine == SM_CH3_A) {zaptime= millis();stateMachine =SM_CH3_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=videoCh[3].id;
                        receiverAvByCh (recevierCh);
                        stateMachine =SM_CH4_A;
                      }
                  }
                else stateMachine =SM_CH4_A;
            break;

            case SM_CH4_A:    //47
            case SM_CH4_B:
                if (videoCh[4].zap ) 
                  {
                    if (stateMachine == SM_CH4_A) {zaptime= millis();stateMachine =SM_CH4_B;}
                    if (millis() - zaptime > zapTimer) 
                         {
                          recevierCh=videoCh[4].id;
                          receiverAvByCh (recevierCh);
                          stateMachine =SM_CH5_A;
                         }
                   }
                else stateMachine =SM_CH5_A;
            break;
                       
            case SM_CH5_A:    //48
            case SM_CH5_B:
                if (videoCh[5].zap ) 
                  {
                    if (stateMachine == SM_CH5_A) {zaptime= millis();stateMachine =SM_CH5_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[5].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH6_A;
                      if (videoCh[5].mux && videoCh[16].zap)remoteControl(16); //48 and 22 off 22
                     }
                  }
                else stateMachine =SM_CH6_A;
            break;
                       
            case SM_CH6_A:    //49
            case SM_CH6_B:
                if (videoCh[6].zap ) 
                  {
                    if (stateMachine == SM_CH6_A) {zaptime= millis();stateMachine =SM_CH6_B;}
                    if (millis() - zaptime > zapTimer) 
                     {
                      recevierCh=videoCh[6].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH7_A;
                     }
                  }
                else stateMachine =SM_CH7_A;
            break;
                       
            case SM_CH7_A:    //50
            case SM_CH7_B:
                if (videoCh[7].zap ) 
                  {
                    if (stateMachine == SM_CH7_A) {zaptime= millis();stateMachine =SM_CH7_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=videoCh[7].id;
                        receiverAvByCh (recevierCh);
                        stateMachine =SM_CH8_A;
                        if (videoCh[7].mux && videoCh[14].zap)remoteControl(7); //50 and 27 off 50
                       }
                  }
                else stateMachine =SM_CH8_A;
            break;
                       
            case SM_CH8_A:    //51
            case SM_CH8_B:
                if (videoCh[8].zap) 
                  {
                    if (stateMachine == SM_CH8_A) {zaptime= millis();stateMachine =SM_CH8_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=videoCh[8].id;
                        receiverAvByCh (recevierCh);
                        stateMachine =SM_CH9_A;
                        if (videoCh[8].mux && videoCh[15].zap)remoteControl(8); //51 and 31 off 51
                      }
                  }
                else stateMachine =SM_CH9_A;
            break;

            case SM_CH9_A:    //52
            case SM_CH9_B:
                if (videoCh[9].zap) 
                  {
                    if (stateMachine == SM_CH9_A) {zaptime= millis();stateMachine =SM_CH9_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[9].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH10_A;
                    }
                  }
                else stateMachine =SM_CH10_A;
            break;
            
            case SM_CH10_A:   //53
            case SM_CH10_B:
                if (videoCh[10].zap) 
                  {
                    if (stateMachine == SM_CH10_A) {zaptime= millis();stateMachine =SM_CH10_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[10].id;
                      receiverAvByCh (recevierCh);
                    }
                  }
                else stateMachine =SM_CH11_A;
            break; 
             
            case SM_CH11_A:   //01
            case SM_CH11_B:
                if (videoCh[11].zap) 
                  {
                    if (stateMachine == SM_CH11_A) {zaptime= millis();stateMachine =SM_CH11_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[11].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH12_A;
                      if (videoCh[11].mux  &&videoCh[13].zap)remoteControl(13); //01 and 03 OFF 03
                    }
                  }
                else stateMachine =SM_CH12_A;
            break;     

            case SM_CH12_A:   //2
            case SM_CH12_B:
                if (videoCh[12].zap) 
                  {
                    if (stateMachine == SM_CH12_A) {zaptime= millis();stateMachine =SM_CH12_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[12].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH13_A;
                    }
                  }
                else stateMachine =SM_CH13_A;
            break;  
            
            case SM_CH13_A:   // 3
            case SM_CH13_B:
                if (videoCh[13].zap) 
                  {
                    if (stateMachine == SM_CH13_A) {zaptime= millis();stateMachine =SM_CH13_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[13].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH14_A;
                      if (videoCh[13].mux  &&videoCh[11].zap)remoteControl(13); //01 and 03 OFF 03
                    }
                  }
                else stateMachine =SM_CH14_A;
            break;  

            case SM_CH14_A:   //27
            case SM_CH14_B:
                if (videoCh[14].zap) 
                  {
                    if (stateMachine == SM_CH14_A) {zaptime= millis();stateMachine =SM_CH14_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[14].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH15_A;
                      if (videoCh[14].mux && videoCh[7].zap)remoteControl(7); //50 and 27 OFF 50
                    }
                  }
                else stateMachine =SM_CH15_A;
            break;  
            
            case SM_CH15_A:   //31
            case SM_CH15_B:
                if (videoCh[15].zap) 
                  {
                    if (stateMachine == SM_CH15_A) {zaptime= millis();stateMachine =SM_CH15_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[15].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH16_A;
                      if (videoCh[15].mux && videoCh[8].zap)remoteControl(8); //51 and 31 off 51
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH16_A;}
            break;                         

            case SM_CH16_A:   //22
            case SM_CH16_B:
                if (videoCh[16].zap) 
                  {
                    if (stateMachine == SM_CH16_A) {zaptime= millis();stateMachine =SM_CH16_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[16].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH17_A;
                      if (videoCh[16].mux && videoCh[5].zap)remoteControl(16); //48 and 22 off 22
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH17_A;}
            break;                         

            case SM_CH17_A:   //37
            case SM_CH17_B:
                if (videoCh[17].zap) 
                  {
                    if (stateMachine == SM_CH17_A) {zaptime= millis();stateMachine =SM_CH17_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[17].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH18_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH18_A;}
            break;                         


            case SM_CH18_A:   //X
            case SM_CH18_B:
                if (videoCh[18].zap) 
                  {
                    if (stateMachine == SM_CH18_A) {zaptime= millis();stateMachine =SM_CH18_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[18].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH19_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH19_A;}
            break;                         

            case SM_CH19_A: //XX
            case SM_CH19_B:
                if (videoCh[19].zap) 
                  {
                    if (stateMachine == SM_CH19_A) {zaptime= millis();stateMachine =SM_CH19_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[19].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH20_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH20_A;}
            break;                         

            case SM_CH20_A:  //XXX
            case SM_CH20_B:
                if (videoCh[20].zap) 
                  {
                    if (stateMachine == SM_CH20_A) {zaptime= millis();stateMachine =SM_CH20_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[20].id;
                      receiverAvByCh (recevierCh);
                      stateMachine =SM_CH1_A;
                      if (videoCh[20].mux)remoteControl(20);
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH1_A;}
            break;                         

                                                           
          }
  }
  else {zaptime= millis();stateMachine =SM_CH1_A;}
}



void remoteControl(int cmd )
{
     if (blynkConnected)  myBlynk.blynkRCLed(1, cmd); 
     
     mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);
     DEBUG_PRINT("ch433:");DEBUG_PRINTLN(cmd);
     delay(500);
     
     if (blynkConnected) 
      {
        myBlynk.blynkRCLed(0, cmd);
        myBlynk.resetT433Cmd(cmd);
      }
}




        
void receiverAvByCh (int Ch)
{
  bool ack;
  int PLL_value;
       if (blynkConnected) myBlynk.blynkAckLed(true);
       if (FBConnected) fb.SendString (FB_ACK_LED, String(1) );
       ack = avReceiver.Tuner_PLL(av_pll_addr, _pll[Ch]); 
       delay(500);
       
       if (blynkConnected) {myBlynk.blynkAckLed(ack); myBlynk.sevenSegValue(Ch);}
       if (FBConnected) fb.SendString (FB_ACK_LED, String(ack) );
       recevierFreq =videoCh[Ch].frequency;   
       
       if (blynkConnected) {myBlynk.frequencyValue(recevierFreq );myBlynk.visualActiveRoom(Ch,zapOnOff );}
       DEBUG_PRINT("Received freq channel:");DEBUG_PRINTLN(Ch);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}



void receiverAvByFreq (int Freq)
{
  bool ack=0;
       recevierFreq =Freq;
       if (blynkConnected) myBlynk.blynkAckLed(true); 
       if (FBConnected) fb.SendString (FB_ACK_LED, String(1) );
       videoCh[recevierCh].frequency = Freq;
       _pll[recevierCh] =( 512 * (Freq + 479.5) ) / 64 ;
  //     int PLL_value    =( 512 * (Freq + 479.5) ) / 64 ;
       ack = avReceiver.Tuner_PLL(av_pll_addr, _pll[recevierCh]);
       if (blynkConnected)  { myBlynk.blynkAckLed(ack);myBlynk.frequencyValue(Freq );}
       if (FBConnected) fb.SendString (FB_ACK_LED, String(ack) );
       DEBUG_PRINT("Received manual_freq:");DEBUG_PRINTLN(manual_freq);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
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


void IRAM_ATTR resetModule() 
{
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







void otaWifi(void) {
  DEBUG_PRINTLN("Starting Ota Web Update from Github");
  sendToHMI("Ota GitHub Started", "Ota Web : ", "Ota web Started",FB_NOTIFIER, "Ota GitHub Started" );
while (!otaWifiGithub) 
       {
        enableWDG(false);
        if (  millis() - wifiIDETimer > WIFI_IDE_TIMER )
        {
           otaWifiGithub = true;
           resetWdg();
           enableWDG(true);
           wifiIDETimer = millis();
           ESP.restart();
        }
//        ota.otaWebGithub();
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    for(uint8_t t = 3; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        delay(500);
    }

      t_httpUpdate_return ret = ESPhttpUpdate.update(gitHubURL);

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                USE_SERIAL.println("HTTP_UPDATE_OK");
                break;
                }
      }
 }


void otaGsm(void)
{
  DEBUG_PRINTLN("Firmware Upload..."); 
  enableWDG(DIS);
  ota.init(true);
  ota.startOtaUpdate(overTheAirURL);
}

void rebootSw(void)
{
 ESP.restart();
}



void  dvrOnOff (bool onOff)
{

   if (onOff) digitalWrite(AV_RX_DVR_PIN_2, LOW); 
   else digitalWrite(AV_RX_DVR_PIN_2, HIGH); 

}

/**********************************************************************************************************************************************/

void sendToHMI(char *smsmsg, String notifier_subject, String notifier_body,String fb_path,String fb_cmdString)
{
  if(smsSent) smsSent=sms.SendSMS(smsmsg);
  if (blynkConnected) myBlynk.notifierDebug(NOTIFIER_ID, notifier_body);
  DEBUG_PRINTLN(notifier_body);
}


int stringToInteger(String str)
{
char carray[5]; 
      str.toCharArray(carray, sizeof(carray));
      return ( atoi(carray));  
}

void goToDeepSleep(int sleepTimer)
{
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
           ESP.restart();
        }
        else ArduinoOTA.handle();
       }
}
