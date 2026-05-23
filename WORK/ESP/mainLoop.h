// mainLoop.h

#ifndef MAINLOOP_H
#define MAINLOOP_H

extern int inactivityPowerOffTimer  ; //1 Hour;
extern int inactivityRestartTimer  ; //10 Hours;
extern bool connectAWS(void);
extern blynk myBlynk;
extern void resetRemoteRCNoBlynk(int esp);

extern bool  blynkActive;
extern unsigned int LiveUpdateInterval ;
bool dvrSleep  ;
extern uint32_t crashCount;
extern void saveCrashCount(void);
extern void blueLedFlash();

void SendLiveLed()
  {
    if (liveLed)  liveLed = false; 
    else liveLed = true;
    liveLedUpdate =false;
    awsConnected = client.connected();


 /****************** DVR ************************   */
  if(inactivityCtrl == BLYNK_TIMERS)
    {
    if(!blynkActive &&  !zapOnOff && !zapScanOnly)
      {
        LiveSec += LiveUpdateInterval/1000;
        if (LiveSec >= 60) { LiveMin +=1;  LiveSec = 0;}
        if (LiveMin >= 60) { LiveHour +=1; LiveMin =0; }
  
        if(LiveHour >= 1  &&  stateDVR == DVR_ON && !dvrSleep ) 
          { 
            myBlynk.TerminalPrint("Turning Off Video for non activity for 1 Hour.."); 
            dvrOnOff (POWER_OFF);
            dvrSleep = true;
          }
  
        else if(LiveHour >= 12  &&  stateDVR == DVR_OFF && dvrSleep ) 
          { 
            myBlynk.TerminalPrint("Restarting for non activity for 12 Hour.."); 
            ESP.restart();
          }
      }
      
      else if (blynkActive && dvrSleep && stateDVR == DVR_OFF )
      {
        myBlynk.TerminalPrint("Turning On Video after sleeping..."); 
        dvrOnOff (POWER_ON);
        dvrSleep = false; 
        LiveSec =LiveMin =LiveHour = 0; 
      }
  }
   /********************************************************************/
  }
  
void resetRouter(void)
{
// remoteControl(ROUTER_CH);
// delay(3000);
// remoteControl(ROUTER_CH);
}


void processBlynkQueu(void)
{
  String ver = VERSION_ID;
     switch (queuDataID)
          {

            case Q_EVENT_FREQ_V0:
              recevierFreq=queuData;
              receiverAvByFreq ( recevierFreq);
            break;
            
            case Q_EVENT_RC_CH_NR_V1:
              resetRemoteRCNoBlynk(ESP0);
              remoteControlRcCh=queuData;
              RC_Api = true;
              myBlynk.blynkAckLed(false);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 20) {remoteControl(remoteControlRcCh );}
              delay (300);
              RC_Api = false;
              myBlynk.blynkAckLed(false);
            break;      
                  
            case Q_EVENT_VIDEO_CH_V2:
                recevierCh=queuData;
//                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
//                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                
                myBlynk.BlynkButtonColours( lastSelectedCh,CH_MODE_0);
                receiverAvByCh ( recevierCh,1);
                delay(100);
                myBlynk.BlynkButtonColours( recevierCh,CH_MODE_4);
                lastSelectedCh = recevierCh;

             break;
             
             case Q_EVENT_REPEAT_V3:
                  catchCh =queuData;
             break;


            case Q_SCAN_ACTIVE_CH_V4:
                  zapSetup =queuData; 
            break;

            case Q_EVENT_AUTOMATIC_RC_L_R_V5:
                  autoRemoteLocalRc  =queuData; 
            break;         

            case Q_EVENT_OTA_LOCAL_WEB_WIFI_V6:
                     wifiWebUpdater = false;
                     crashCount = 0;
                     saveCrashCount();
                     OtaTimeoutTimer = millis();
                     localWebWifiOta ();
            break;

            case Q_EVENT_OTA_GITHUB_V7:
                   otaWifiGithub= false;         
                   crashCount = 0;
                   saveCrashCount();
                   OtaTimeoutTimer = millis();
                   OtaGithub();
            break;
 
            case Q_EVENT_REBOOT_V8:
                  rebootCmd=queuData;
                  crashCount = 0;
                  saveCrashCount();
                  delay(1000);
                  rebootSw();
            break;

           case Q_EVENT_SELECTED_RECIEVER_CH_1_4_V9:
                  selected_Rx = queuData-1;
                  AvReceiverSel(queuData);
                  videoplayerCh = "ch0" + String(queuData);
                  myBlynk.streamSelect(videoplayerCh);
           break;
 
            case Q_EVENT_ZAP_SCAN_ONLY_V10:
                  zapScanOnly = queuData;
            break;

            case Q_EVENT_WIFI_IDE_V11:
                 wifiIde = false;  
                 crashCount = 0;
                 saveCrashCount();       
                 OtaTimeoutTimer = millis();
                 ArduinoIdeWifi();
            break;            

            break;

            case Q_EVENT_SPARE_V12:
                    
            break;                       
            
            case Q_EVENT_RC_REMOTE_ESP1_V16:
                  RC_Remote_ESP1=queuData;    
                  if (RC_Remote_ESP1) {RC_Remote_ESP2 = false; RC_Remote_ESP3=false;}       
            break;
            
            case Q_EVENT_RC_REMOTE_ESP3_V17:
                  RC_Remote_ESP3=queuData;      
                  if (RC_Remote_ESP3) {RC_Remote_ESP2 = false; RC_Remote_ESP1=false;}        
            break;
            
            case Q_EVENT_AWS_RECONNECT_V18:
                  awsConnected = connectAWS();
            break;                                    

            case Q_EVENT_ROOM_AV_RC_V19:
                  Av_Rx=queuData;
                  myBlynk.sendAvRxIndex(Av_Rx);
            break;
            
            case Q_EVENT_RC_REMOTE_ESP2_V20:
                  RC_Remote_ESP2=queuData;
                  if (RC_Remote_ESP2) {RC_Remote_ESP1 = false; RC_Remote_ESP3=false;} 
            break; 
            
            case Q_EVENT_ZAP_TIMER_OFF_V25:
                  zapTimerOff=queuData;               
            break;     
                    
            case Q_EVENT_RESET_FREQ_V26:
                  recevierFreq = videoCh[recevierCh].frequency =   freqTable[recevierCh];   
                  receiverAvByFreq ( recevierFreq);        
            break;         
                 
            case Q_EVENT_SPARE_V27:
//                      DvrChOn = true;
//                      dvrOnOff (POWER_ON);
            break;   

            case Q_EVENT_DVR_OFF_TIMER_V30: 
                        inactivityPowerOffTimer  = queuData ; //1 Hour;
            break;   

            case Q_EVENT_RESTART_TIMER_V31:   
                        inactivityRestartTimer  = queuData ; //10 HourS;
            break;  

            case Q_EVENT_SELECTED_RECIEVER_CH_5_6_V32: 
                      selected_Rx = queuData-1;
                      AvReceiverSel(queuData);
                      videoplayerCh = "ch0" + String(queuData);
                      myBlynk.streamSelect(videoplayerCh);
            break;   

            case Q_EVENT_SPARE_V33: 
            break;  

            case Q_EVENT_ZAP_ALL_ON_OFF_V34:  
                  zapAllOnOff(queuData);
            break;   

            case Q_EVENT_SHARE_VIDEO_WITH_ESP1_V35: 
                    V_Remote_ESP1=queuData;
            break;                  
 
            case Q_EVENT_SHARE_VIDEO_WITH_ESP2_V36: 
                     V_Remote_ESP2=queuData;
            break;   

            case Q_EVENT_SHARE_VIDEO_WITH_ESP3_V37:  
                    V_Remote_ESP3=queuData;
            break;     

            case Q_EVENT_WIFI_RSSI_V38:  

            break;    
            case Q_EVENT_RESET_ROUTER_V39:  
                  resetRouter();
            break;   
             
            case Q_EVENT_AUTOMATIC_RESET_ROUTER_V40:   
                  autoResetRouter = queuData;
                  myBlynk.TerminalPrint("Auto Reboot Router is: ");myBlynk.TerminalPrint( autoResetRouter ? F("Enabled") : F("Disabled"));
            break;                            

           
            case Q_EVENT_ZAP_V71:
                zapOnOff=queuData;
                resetZapper ();
                zapSetup =false;
                zapScanOnly = false;
                myBlynk.resetSetupAndScan(false);
            break;

            case Q_EVENT_ZAP_TIMER_V72:
                  zapTimerSec = queuData;
                  zapTimer  =  (zapTimerSec * 1000UL) ;
            break;

            case Q_EVENT_VIDEO_ON_OFF_V81 :
                 DvrChOn = false;
                 dvrOnOff (queuData);
            break;

//            case Q_EVENT_SPARE_V82 :
//
//            break;
//
//             case Q_EVENT_SPARE_V83 :
//
//            break;

//             case Q_EVENT_VIDEO_STREAMING_V84 :
                
//            break;

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

            case Q_EVENT_RM_ID_20_V100:
                    videoChanel(20, queuData); //12
            break;
             
            case Q_EVENT_RC_REPETION_V101:
               repetionRC=queuData;
               mySwitch.setRepeatTransmit(repetionRC);
            break;
           
            case Q_EVENT_TERMINAL_V102:
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
 
            case Q_EVENT_RM_ID_10_V112 :
                  videoChanel(1, queuData);
            break; 
      

             case Q_EVENT_RM_ID_02_V122:
                  videoChanel(2, queuData);
             break;
               
             case Q_EVENT_RM_ID_03_V123:
                  videoChanel(3, queuData);
             break;  
             
             case Q_EVENT_RM_ID_04_V124:
                  videoChanel(4, queuData);
             break;  
             
             case Q_EVENT_RM_ID_05_V125:
                  videoChanel(5, queuData);
             break;  
             
             case Q_EVENT_RM_ID_06_V126:
                  videoChanel(6, queuData);
             break;  
             
             case Q_EVENT_RM_ID_07_V127:
                  videoChanel(7, queuData);
             break;  
             
             case Q_EVENT_RM_ID_08_V93:
                  videoChanel(8, queuData);
             break;  
             
             case Q_EVENT_RM_ID_09_V80:
                  videoChanel(9, queuData);
             break;  
             
             case Q_EVENT_RM_ID_10_V21:
                  videoChanel(10, queuData);
             break;  

             case Q_EVENT_MAP_INDEX_V22:
                  MapIndex = queuData;
                  myBlynk.mapRefresh(MapIndex);
             break; 
             
             case Q_EVENT_RM_ID_11_V14:
                  videoChanel(11, queuData);
             break;  
             
             case Q_EVENT_RM_ID_12_V15:
                  videoChanel(12, queuData);
             break;  
             
             case Q_EVENT_RM_ID_13_V23:
                  videoChanel(13, queuData);
             break;  
             
             case Q_EVENT_RM_ID_14_V103:
                  videoChanel(14, queuData);
             break;  
             
             case Q_EVENT_RM_ID_15_V104:
                  videoChanel(15, queuData);
             break;  
             
             case Q_EVENT_RM_ID_16_V105:
                  videoChanel(16, queuData);
             break;  
             
             case Q_EVENT_RM_ID_17_V90:
                  videoChanel(17, queuData);
             break;  
             
             case Q_EVENT_RM_ID_18_V91:
                  videoChanel(18, queuData);
             break;  
             
             case Q_EVENT_RM_ID_19_V92:
                  videoChanel(19, queuData);
             break;  

    }  
    selected_room = recevierCh;
}

void resetInternetLoss()
{
      InternetLoss = false;   
      netGeerReset = false; 
      routerResetStart  = false;
      resetNetgeerAfterInternetLossTimer = millis();
      restartAfterResetNG = millis();
      routerResetTimer        = millis();
}
      
void blynkLoop(void)
{
 StaticJsonDocument<54> doc; //Json to send from
 
       blynkConnected=myBlynk.blynkStatus(); 
       
       if ( blynkConnected )
          {
           blueLedFlash();
           if(!liveLedUpdate) 
                {
                  liveLedUpdate = true; 
                } 
       
            myBlynk.blynkRun();
            queuValidData = (xQueueReceive(g_event_queue_handle, &queuDataID, 5 / portTICK_RATE_MS) == pdPASS);
            if(queuValidData) 
                  {
                  myBlynk.getData ();
                  if (hmi == BLYNK)    
                    {
                     queuData = myBlynk.blynkData; 
                     processBlynkQueu(); 
                    }
                  }
            resetInternetLoss();
          }

       else if( !InternetLoss && !blynkConnected)  
          {
            DEBUG_PRINTLN("Blynk Disconnected , Internet Loss!!!");
            InternetLoss = true; 
            netGeerReset = false; 
            routerResetStart  = false;
            resetNetgeerAfterInternetLossTimer = millis();
            
            blynkEvent=false; 
            myBlynk.sendToBlynk = false;
            myBlynk.sendToBlynkLeds = false;
          }
       myBlynk.blynkRunTimer();
}



void awsLoop(void)
{
      if (!awsConnected )
            {
              myBlynk.TerminalPrint("AWS IoT Disonnected, trying to reconnect");
              awsConnected = connectAWS();
           }
           
        if (awsConnected )
        {
            client.loop();   //AWS MQTT
            queuValidData = (xQueueReceive(g_event_queue_handle, &queuDataID, 5 / portTICK_RATE_MS) == pdPASS);
            if(queuValidData) 
                  {
                      getDataNodeRed ();
                      if (hmi == NODE_RED) { 
                      queuData = nodeRedData; 
                       processBlynkQueu();
                      }
                  }
         }
}



#endif
