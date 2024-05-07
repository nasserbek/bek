// mainLoop.h

#ifndef MAINLOOP_H
#define MAINLOOP_H

extern void connectAWS(void);
extern blynk myBlynk;

void resetRouter(void)
{
 remoteControl(ROUTER_CH);
 delay(3000);
 remoteControl(ROUTER_CH);
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
              remoteControlRcCh=queuData;
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 20) {remoteControl(remoteControlRcCh );}
            break;      
                  
            case Q_EVENT_VIDEO_CH_V2:
                recevierCh=queuData;
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh ( recevierCh,1);
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
                     OtaTimeoutTimer = millis();
                     localWebWifiOta ();
            break;

            case Q_EVENT_OTA_GITHUB_V7:
                   otaWifiGithub= false;         
                   OtaTimeoutTimer = millis();
                   OtaGithub();
            break;
 
            case Q_EVENT_REBOOT_V8:
                  rebootCmd=queuData;
                  rebootSw();
            break;

           case Q_EVENT_SELECTED_RECIEVER_V9:
                  selected_Rx = queuData-1;
                  AvReceiverSel(queuData);
                                 
           break;
 
            case Q_EVENT_ZAP_SCAN_ONLY_V10:
                  zapScanOnly = queuData;
            break;

            case Q_EVENT_WIFI_IDE_V11:
                 wifiIde = false;         
                 OtaTimeoutTimer = millis();
                 ArduinoIdeWifi();
            break;            

            break;

            case Q_EVENT_BOARD_V12:
                  selectedBoard=queuData;
                  myBlynk.sendBoardIndex(selectedBoard);                      
            break;                       
            
            case Q_EVENT_RC_REMOTE_CSR1_V16:
                  RC_Remote_CSR1=queuData;    
                  if (RC_Remote_CSR1) {RC_Remote_CSR2 = false; RC_Remote_CSR3=false;}       
            break;
            
            case Q_EVENT_RC_REMOTE_CSR3_V17:
                  RC_Remote_CSR3=queuData;      
                  if (RC_Remote_CSR3) {RC_Remote_CSR2 = false; RC_Remote_CSR1=false;}        
            break;
            
            case Q_EVENT_AWS_RECONNECT_V18:
                  connectAWS();
            break;                                    

            case Q_EVENT_ROOM_AV_RC_V19:
                  Av_Rx=queuData;
                  myBlynk.sendAvRxIndex(Av_Rx);
            break;
            
            case Q_EVENT_RC_REMOTE_CSR2_V20:
                  RC_Remote_CSR2=queuData;
                  if (RC_Remote_CSR2) {RC_Remote_CSR1 = false; RC_Remote_CSR3=false;} 
            break; 
            
            case Q_EVENT_ZAP_TIMER_OFF_V25:
                  zapTimerOff=queuData;               
            break;     
                    
            case Q_EVENT_RESET_FREQ_V26:
                  recevierFreq = videoCh[recevierCh].frequency =   freqTable[recevierCh];   
                  receiverAvByFreq ( recevierFreq);        
            break;         
                 
            case Q_EVENT_DVR_ON_OFF_V27:
     //             dvrOnOff (queuData);  
            break;   

            case Q_EVENT_REL1_CH_V30: 
            break;   

            case Q_EVENT_REL2_CH_V31:   
            break;  

            case Q_EVENT_REL3_CH_V32: 
            break;   

            case Q_EVENT_REL4_CH_V33: 
            break;  

            case Q_EVENT_ZAP_ALL_ON_OFF_V34:  
                  zapAllOnOff(queuData);
            break;   

            case Q_EVENT_SHARE_VIDEO_WITH_CSR1_V35: 
                    V_Remote_CSR1=queuData;
            break;                  
 
            case Q_EVENT_SHARE_VIDEO_WITH_CSR2_V36: 
                     V_Remote_CSR2=queuData;
            break;   

            case Q_EVENT_SHARE_VIDEO_WITH_CSR3_V37:  
                    V_Remote_CSR3=queuData;
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
                 zapTimer=queuData;
            break;

            case Q_EVENT_VIDEO_ON_OFF_V81 :
                 dvrOnOff (queuData);
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


      
void blynkLoop(void)
{
 StaticJsonDocument<54> doc; //Json to send from
 
       blynkConnected=myBlynk.blynkStatus(); 

            if(!liveLedUpdate) 
                {
                  liveLedUpdate = true; 
                  if ( blynkConnected ) myBlynk.liveLedCall(liveLed);
                  if(awsConnected)
                    {
                      doc["LIVE"] = liveLed;
                      serializeJson(doc, Json); // print to client
                      client.publish(AWS_IOT_SUBSCRIBE_TOPIC_LIVE, Json);
                   }
                }  
       
       if ( blynkConnected )
          {
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
                      queuData = nodeRedData; 
                      processBlynkQueu();
                      }
                  }
         }
}




#endif
