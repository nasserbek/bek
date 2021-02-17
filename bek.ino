#include "main.h"

 reciever avReceiver;
 sim800L sms; 
 otaUpload ota; 
 blynk myBlynk;

 
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

   Ch_1.frequency = 1080;   
   Ch_1.id =1;

   Ch_2.frequency = 1120;   
   Ch_2.id =2;

   Ch_3.frequency = 1160;   
   Ch_3.id =3;

   Ch_4.frequency = 1200;   
   Ch_4.id =4;

   Ch_5.frequency = 1240;   
   Ch_5.id =5;

   Ch_6.frequency = 1280;   
   Ch_6.id =6;

   Ch_7.frequency = 1320;   
   Ch_7.id =7;

   Ch_8.frequency = 1360;   
   Ch_8.id =8;

   Ch_9.frequency = 1060;   
   Ch_9.id =9;

   Ch_10.frequency = 1100;   
   Ch_10.id =10;

   Ch_11.frequency = 1140;   
   Ch_11.id =11;

   Ch_12.frequency = 1180;
   Ch_12.id =12;

   Ch_13.frequency = 1220;
   Ch_13.id =13;

   Ch_14.frequency = 1260;
   Ch_14.id =14;

   Ch_15.frequency = 1300;
   Ch_15.id =15;

   // VTR 21
   xRoom_201.frequency = freq =Ch_1.frequency;  
   xRoom_201.pll = _pll[Ch_1.id] = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_201.vCh = Ch_1.id;
   xRoom_201.rCh=1;  
   
   // VTR 22
   xRoom_202.frequency = freq =Ch_2.frequency;  
   xRoom_202.pll = _pll[Ch_2.id] = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_202.vCh = Ch_2.id;
   xRoom_202.rCh=2;  

   // VTR 23
   xRoom_203.frequency = freq =Ch_3.frequency;
   xRoom_203.pll =  _pll[Ch_3.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_203.vCh = Ch_3.id;
   xRoom_203.rCh=3;  

   // VTR 24
   xRoom_204.frequency = freq =Ch_4.frequency;
   xRoom_204.pll = _pll[Ch_4.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_204.vCh = Ch_4.id;
   xRoom_204.rCh=4;  


   // VTR 25
   xRoom_205.frequency = freq =Ch_5.frequency;
   xRoom_205.pll =  _pll[Ch_5.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_205.vCh = Ch_5.id;
   xRoom_205.rCh=5;

   // VTR 26
   xRoom_206.frequency = freq =Ch_6.frequency;
   xRoom_206.pll =  _pll[Ch_6.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_206.vCh = Ch_6.id;
   xRoom_206.rCh=6;

   // VTR 35
   xRoom_207.frequency = freq =Ch_7.frequency;
   xRoom_207.pll =  _pll[Ch_7.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_207.vCh = Ch_7.id;
   xRoom_207.rCh=7;

   // VTR 36
   xRoom_208.frequency = freq =Ch_8.frequency;
   xRoom_208.pll =  _pll[Ch_8.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_208.vCh = Ch_8.id;
   xRoom_208.rCh=8;
 
   // VTR 37
   xRoom_209.frequency = freq =Ch_9.frequency;
   xRoom_209.pll =  _pll[Ch_9.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_209.vCh = Ch_9.id;
   xRoom_209.rCh=9;

   // VTR 38
   xRoom_210.frequency = freq =Ch_10.frequency;  //VTR 31
   xRoom_210.pll =  _pll[Ch_10.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_210.vCh = Ch_10.id;
   xRoom_210.rCh=10;  // 10 ON 31 ALREADY

   // VTR 39
   xRoom_211.frequency = freq =Ch_11.frequency; 
   xRoom_211.pll =  _pll[Ch_11.id] = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_211.vCh = Ch_11.id;
   xRoom_211.rCh=11;
   
   // VTR 40 
   xRoom_212.frequency = freq =Ch_12.frequency;
   xRoom_212.pll =  _pll[Ch_12.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_212.vCh = Ch_12.id;
   xRoom_212.rCh=12;  





    // VTR  
   xRoom_213.frequency = freq =Ch_13.frequency;
   xRoom_213.pll =  _pll[Ch_13.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_213.vCh = Ch_13.id;
   xRoom_213.rCh=13;     
   
    // VTR    
   xRoom_214.frequency = freq =Ch_14.frequency;
   xRoom_214.pll =  _pll[Ch_14.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_214.vCh = Ch_14.id;
   xRoom_214.rCh=14; 
   
   // VTR  
   xRoom_214.frequency = freq =Ch_14.frequency;
   xRoom_214.pll =  _pll[Ch_14.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_214.vCh = Ch_14.id;
   xRoom_214.rCh=14;  

   // VTR 15
   xRoom_215.frequency = freq =Ch_15.frequency;
   xRoom_215.pll =  _pll[Ch_15.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_215.vCh = Ch_15.id;
   xRoom_215.rCh=15;
 
   // VTR  
   xRoom_216.frequency = freq =Ch_16.frequency;
   xRoom_216.pll =  _pll[Ch_16.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_216.vCh = Ch_16.id;
   xRoom_216.rCh=16;


   // VTR    
   xRoom_227.frequency = freq =Ch_6.frequency;
   xRoom_227.pll =  _pll[Ch_6.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_227.vCh = Ch_6.id;
   xRoom_227.rCh=17;   

   // VTR    
   xRoom_228.frequency = freq =Ch_7.frequency;
   xRoom_228.pll =  _pll[Ch_7.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_228.vCh = Ch_7.id;
   xRoom_228.rCh=18;   

   // VTR   
   xRoom_229.frequency = freq =Ch_8.frequency;
   xRoom_229.pll =  _pll[Ch_8.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_229.vCh = Ch_8.id;
   xRoom_229.rCh=19;  // BY ERROR T BE CHANGED TO 21

   // VTR    
   xRoom_230.frequency = freq =Ch_9.frequency;
   xRoom_230.pll =  _pll[Ch_9.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_230.vCh = Ch_9.id;
   xRoom_230.rCh=21;  // 20IS ON 210
      
      
   // VTR    
   xRoom_231.frequency = freq =Ch_10.frequency;
   xRoom_231.pll =  _pll[Ch_10.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_231.vCh = Ch_10.id;
   xRoom_231.rCh=10;   

   // VTR  
   xRoom_232.frequency = freq =Ch_11.frequency;
   xRoom_232.pll =  _pll[Ch_11.id] = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_232.vCh = Ch_11.id;
   xRoom_232.rCh=22;

   // VTR 33
   xRoom_233.frequency = freq =Ch_12.frequency;
   xRoom_233.pll =  _pll[Ch_12.id]  = ( 512 * (freq + 479.5) ) / 64 ;
   xRoom_233.vCh = Ch_12.id;
   xRoom_233.rCh=23;

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
   
    createHandleGroup();
    looadRoomData();
    enableWDG(DIS);
    initWDG(SEC_60,EN);
    
    String ver = VERSION_ID;
    if (blynkConnected) myBlynk.sendVersion(ver);
    
    char buf[10]; //make this the size of the String
    ver.toCharArray(buf, 10);    
    if(smsSent) smsSent= sms.SendSMS(buf);
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

            case Q_EVENT_ROOM_VTR_21_TO_25_V3:
                  
                  switch (queuData)
                    {
                      case 1: // ROOM 21
                            remoteControlRcCh = xRoom_201.rCh;
                            recevierCh        =xRoom_201.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 2: // ROOM 22
                            remoteControlRcCh = xRoom_202.rCh;
                            recevierCh        =xRoom_202.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 23
                            remoteControlRcCh = xRoom_203.rCh;
                            recevierCh        =xRoom_203.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;      
                      
                      case 4:// ROOM 24
                            remoteControlRcCh = xRoom_204.rCh;
                            recevierCh        =xRoom_204.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 25
                            remoteControlRcCh = xRoom_205.rCh;
                            recevierCh        =xRoom_205.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }
                    
            break;
            
            case Q_EVENT_ROOM_VTR_26_TO_38_V16:
                   switch (queuData)
                    {
                      case 1:// ROOM 26
                            remoteControlRcCh = xRoom_206.rCh;
                            recevierCh        =xRoom_206.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx ); 
                      break;
                      
                      case 2:// ROOM 35
                            remoteControlRcCh = xRoom_207.rCh;
                            recevierCh        =xRoom_207.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 36
                            remoteControlRcCh = xRoom_208.rCh;
                            recevierCh        =xRoom_208.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;      
                      
                      case 4:// ROOM 37
                            remoteControlRcCh = xRoom_209.rCh;
                            recevierCh        =xRoom_209.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 38
                            remoteControlRcCh = xRoom_210.rCh;
                            recevierCh        =xRoom_210.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;   
                    }

            break;
            
            case Q_EVENT_ROOM_VTR_39_TO_40_V17:
                   switch (queuData)
                    {
                      case 1:// ROOM 39
                            remoteControlRcCh = xRoom_211.rCh;
                            recevierCh        =xRoom_211.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 2:// ROOM 40
                            remoteControlRcCh = xRoom_212.rCh;
                            recevierCh        =xRoom_212.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 213
                            remoteControlRcCh = xRoom_213.rCh;
                            recevierCh        =xRoom_213.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
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
            
            case Q_EVENT_ROOM_216_227_228_229_230_V18:
                   switch (queuData)
                    {
                      case 1:// ROOM 216
                            remoteControlRcCh = xRoom_216.rCh;
                            recevierCh        =xRoom_216.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 2:// ROOM 227  
                            remoteControlRcCh = xRoom_227.rCh;
                            recevierCh        =xRoom_227.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 228
                            remoteControlRcCh = xRoom_228.rCh;
                            recevierCh        =xRoom_228.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;        
                      
                      case 4:// ROOM 229
                            remoteControlRcCh = xRoom_229.rCh;
                            recevierCh        =xRoom_229.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;               
                      
                      case 5:// ROOM 230
                            remoteControlRcCh = xRoom_230.rCh;
                            recevierCh        =xRoom_230.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
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

 
            case Q_EVENT_ROOM_231_232_233_V25:
                   switch (queuData)
                    {
                      case 1:// ROOM 231
                            remoteControlRcCh = xRoom_231.rCh;
                            recevierCh        =xRoom_231.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 2:// ROOM 232  
                            remoteControlRcCh = xRoom_232.rCh;
                            recevierCh        =xRoom_232.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;
                      
                      case 3:// ROOM 233
                            remoteControlRcCh = xRoom_233.rCh;
                            recevierCh        =xRoom_233.vCh;
                            room (remoteControlRcCh, recevierCh , Av_Rx );
                      break;        
                      
                      case 4:
                      break;               
                      
                      case 5:
                      break;   
                    }

            break;     
            
            case Q_EVENT_209_SEL_V30 :
              xRoom_209.selected=queuData;
            break;
            case Q_EVENT_216_SEL_V31 :
              xRoom_216.selected=queuData;
            break;

            case Q_EVENT_202_SEL_V32 :
              xRoom_202.selected=queuData;
            break;

             case Q_EVENT_210_SEL_V33 :
              xRoom_210.selected=queuData;
            break;
            case Q_EVENT_204_SEL_V34 :
              xRoom_204.selected=queuData;
            break;

            case Q_EVENT_219_SEL_V35 :
              xRoom_233.selected=queuData;
            break;

            case Q_EVENT_207_SEL_V36 :
              xRoom_207.selected=queuData;
            break;

            case Q_EVENT_214_SEL_V37 :
              xRoom_214.selected=queuData;
            break;

            case Q_EVENT_208_SEL_V38 :
              xRoom_208.selected=queuData;
            break;

            case Q_EVENT_215_SEL_V39 :
              xRoom_215.selected=queuData;
            break;
            
            case Q_EVENT_217_SEL_V40 :
              xRoom_231.selected=queuData;
            break;
                                               
            case Q_EVENT_ZAP_V71:
              zapOnOff=queuData;
              if(zapOnOff)
              {
                Ch_1.muxTimer = millis();
                Ch_2.muxTimer = millis();
                Ch_3.muxTimer = millis();
                Ch_4.muxTimer = millis();
                Ch_5.muxTimer = millis();
                Ch_6.muxTimer = millis();
                Ch_7.muxTimer = millis();
                Ch_8.muxTimer = millis();
              }
              DEBUG_PRINT("ZAP IS : ");
              DEBUG_PRINTLN(zapOnOff ? F("On") : F("Off"));
              myBlynk.zapLed(zapOnOff);
            break;

            case Q_EVENT_ZAP_TIMER_V72:
              zapTimer=queuData;
            break;

            case Q_EVENT_ZAP_CHANNEL1_V81 :
              Ch_1.zap=queuData;
              
            break;

             case Q_EVENT_ZAP_CHANNEL2_V82 :
              Ch_2.zap=queuData;
              
            break;

             case Q_EVENT_ZAP_CHANNEL3_V83 :
              Ch_3.zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL4_V84 :
              Ch_4.zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL5_V85 :
              Ch_5.zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL6_V86 :
              Ch_6.zap=queuData;

            break;

             case Q_EVENT_ZAP_CHANNEL7_V87 :
              Ch_7.zap=queuData;
              
            break;

             case Q_EVENT_ZAP_CHANNEL8_V88 :
              Ch_8.zap=queuData;
              
            break;
            
            case Q_EVENT_ZAP_CHANNEL9_V89 :
              Ch_9.zap=queuData;
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
              Ch_10.zap=queuData;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL11_V95 :
              Ch_11.zap=queuData;
            break; 

            case Q_EVENT_ZAP_CHANNEL12_V96 :
              Ch_12.zap=queuData;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL14_V97 :
              Ch_14.zap=queuData;
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

             case Q_EVENT_WIFI_OTA_V105:
               otaWifiGithub = false;         
               wifiIDETimer = millis();
               otaWifi();
             break;

            case Q_EVENT_ZAP_CHANNEL15_V106 :
              Ch_15.zap=queuData;
            break; 
 
            case Q_EVENT_ZAP_CHANNEL16_V107 :
              Ch_16.zap=queuData;
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
          else if (smsReceived =="Ota") smsID = FB_OTA_ID;
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
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=Ch_1.id;
                        receiverAvByCh (recevierCh);
                        Ch_1.mux= xRoom_209.selected & xRoom_216.selected;
                   //     if(Ch_1.mux) 
                           if(xRoom_209.selected || xRoom_216.selected)
                            {
                              if (millis() - Ch_1.muxTimer > MUX_ROOM_ZAP) 
                                  {
                                    if(xRoom_209.selected && !xRoom_216.selected)remoteControl(xRoom_209.rCh);
                                    if(xRoom_216.selected && !xRoom_209.selected)remoteControl(xRoom_216.rCh);
                                    Ch_1.muxTimer = millis();}
                            }
                        stateMachine =2;}
                  }
                else stateMachine =2;
            break;

            case 2:
            case 3:
                if (Ch_2.zap ) 
                  {
                    if (stateMachine == 2) {zaptime= millis();stateMachine =3;}
                    if (millis() - zaptime > zapTimer) 
                    {
                        recevierCh=Ch_2.id;
                        receiverAvByCh (recevierCh);
                        Ch_2.mux= xRoom_202.selected & xRoom_210.selected;
                   //     if(Ch_2.mux) 
                           if(xRoom_202.selected || xRoom_210.selected)
                            {
                              if (millis() - Ch_2.muxTimer > MUX_ROOM_ZAP) 
                                  {
                                    if(xRoom_202.selected && !xRoom_210.selected)remoteControl(xRoom_202.rCh);
                                    if(xRoom_210.selected && !xRoom_202.selected)remoteControl(xRoom_210.rCh);
                                    Ch_2.muxTimer = millis();}
                            }
                        stateMachine =4;}
                  }
                else stateMachine =4;
            break;
            
            case 4:
            case 5:
                if (Ch_3.zap ) 
                  {
                    if (stateMachine == 4) {zaptime= millis();stateMachine =5;}
                    if (millis() - zaptime > zapTimer) 
                    {
                        recevierCh=Ch_3.id;receiverAvByCh (recevierCh);
                        stateMachine =6;}
                  }
                else stateMachine =6;
            break;

            case 6:
            case 7:
                if (Ch_4.zap ) 
                  {
                    if (stateMachine == 6) {zaptime= millis();stateMachine =7;}
                    if (millis() - zaptime > zapTimer) 
                    {
                        recevierCh=Ch_4.id;
                        receiverAvByCh (recevierCh);
                        Ch_4.mux= xRoom_204.selected & xRoom_233.selected;
                   //     if(Ch_4.mux) 
                           if(xRoom_204.selected || xRoom_233.selected)
                            {
                              if (millis() - Ch_4.muxTimer > MUX_ROOM_ZAP) 
                                  {
                                    if(xRoom_204.selected && !xRoom_233.selected)remoteControl(xRoom_204.rCh);
                                    if(xRoom_233.selected && !xRoom_204.selected)remoteControl(xRoom_233.rCh);
                                    Ch_4.muxTimer = millis();}
                            }
                        stateMachine =8;}
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
                    if (millis() - zaptime > zapTimer) 
                    {
                        recevierCh=Ch_7.id;
                        receiverAvByCh (recevierCh);
                        Ch_7.mux= xRoom_207.selected & xRoom_214.selected;
                   //     if(Ch_7.mux) 
                           if(xRoom_207.selected || xRoom_214.selected)
                            {
                              if (millis() - Ch_7.muxTimer > MUX_ROOM_ZAP) 
                                  {
                                    if(xRoom_207.selected && !xRoom_214.selected)remoteControl(xRoom_207.rCh);
                                    if(xRoom_214.selected && !xRoom_207.selected)remoteControl(xRoom_214.rCh);
                                    Ch_7.muxTimer = millis();}
                            }
                        stateMachine =14;}
                  }
                else stateMachine =14;
            break;
                       
            case 14:
            case 15:
                if (Ch_8.zap) 
                  {
                    if (stateMachine == 14) {zaptime= millis();stateMachine =15;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=Ch_8.id;
                        receiverAvByCh (recevierCh);
                        Ch_8.mux= xRoom_208.selected & xRoom_215.selected;
                   //     if(Ch_8.mux) 
                           if(xRoom_208.selected || xRoom_215.selected)
                            {
                              if (millis() - Ch_8.muxTimer > MUX_ROOM_ZAP) 
                                  {
                                    if(xRoom_208.selected && !xRoom_215.selected)remoteControl(xRoom_208.rCh);
                                    if(xRoom_215.selected && !xRoom_208.selected)remoteControl(xRoom_215.rCh);
                                    Ch_8.muxTimer = millis();}
                            }
                      stateMachine =16;}
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
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_11.id;receiverAvByCh (recevierCh);stateMachine =22;}
                  }
                else stateMachine =22;
            break;     

            case 22:
            case 23:
                if (Ch_12.zap) 
                  {
                    if (stateMachine == 22) {zaptime= millis();stateMachine =23;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_12.id;receiverAvByCh (recevierCh);stateMachine =24;}
                  }
                else stateMachine =24;
            break;  
            case 24:
            case 25:
                if (Ch_14.zap) 
                  {
                    if (stateMachine == 24) {zaptime= millis();stateMachine =25;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_14.id;receiverAvByCh (recevierCh);stateMachine =26;}
                  }
                else stateMachine =26;
            break;  

            case 26:
            case 27:
                if (Ch_15.zap) 
                  {
                    if (stateMachine == 26) {zaptime= millis();stateMachine =27;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_15.id;receiverAvByCh (recevierCh);stateMachine =28;}
                  }
                else stateMachine =28;
            break;  
            
            case 28:
            case 29:
                if (Ch_16.zap) 
                  {
                    if (stateMachine == 28) {zaptime= millis();stateMachine =29;}
                    if (millis() - zaptime > zapTimer) {recevierCh=Ch_16.id;receiverAvByCh (recevierCh);stateMachine =0;}
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
               recevierFreq =Ch_1.frequency;
            break;

            case 2:
                recevierFreq =Ch_2.frequency;
            break;

            
            case 3:
               recevierFreq =Ch_3.frequency;
            break;


            case 4:
               recevierFreq =Ch_4.frequency;
            break;

                       
            case 5:
              recevierFreq =Ch_5.frequency;
            break;

                       
            case 6:
              recevierFreq =Ch_6.frequency;
            break;

                       
            case 7:
              recevierFreq =Ch_7.frequency;
            break;

                       
            case 8:
              recevierFreq =Ch_8.frequency;
            break;
            
            case 9:
              recevierFreq =Ch_9.frequency;
            break;
            
            case 10:
              recevierFreq =Ch_10.frequency;
            break;
            
            case 11:
              recevierFreq =Ch_11.frequency;
            break;     
                   
            case 12:
              recevierFreq =Ch_12.frequency;
            break;     

            case 13:
              recevierFreq =Ch_13.frequency;
            break;     
                        
            case 14:
              recevierFreq =Ch_14.frequency;
            break;     
            
            case 15:
              recevierFreq =Ch_15.frequency;
            break;     
                  
          }
       if (blynkConnected) myBlynk.frequencyValue(recevierFreq );
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
  sendToHMI("Ota web Started", "Ota Web : ", "Ota web Started",FB_NOTIFIER, "Ota web Started" );
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
        ota.otaWebGithub();

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
