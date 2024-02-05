#include "MyDrivers.h"

blynk myBlynk;

void setup() 
{

     relaySetup();  
     Serial.begin(115200);
     i2cSetup();
     initWDG(MIN_5,EN);
     resetWdg();    //reset timer (feed watchdog) 
     mySwitch.enableTransmit(RC_TX_PIN);
     blynkInit();
     connectAWS();
     timersMillis();
     createHandleGroup();
     looadRoomData();
     enableWDG(DIS);
     initWDG(SEC_60,EN);
}


void loop(void) 
{
       resetWdg();    //reset timer (feed watchdog) 
       internetCheck();
       blynkLoop();
       awsLoop();
       bool zapScan = zapOnOff || zapScanOnly;
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}

/********************************************Blynk *******************************/
void processBlynkQueu(void)
{
  String ver = VERSION_ID;
     switch (queuDataID)
          {

            case Q_EVENT_FREQ_V0:
              recevierFreq=queuData;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(queuData);
              //if (recevierFreq >= 920 && recevierFreq <= 1500) 
              receiverAvByFreq ( recevierFreq);
            break;
            
            case Q_EVENT_RC_CH_NR_V1:
              remoteControlRcCh=queuData;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(queuData);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 20) {remoteControl(remoteControlRcCh );}
            break;      
                  
            case Q_EVENT_VIDEO_CH_V2:
                recevierCh=queuData;
                DEBUG_PRINT("FB_VIDEO_CH_PATH: ");DEBUG_PRINTLN(queuData);
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh ( recevierCh);
             break;
             
             case Q_EVENT_REPEAT_V3:
                  repeatCh =queuData;
             break;


            case Q_SCAN_ACTIVE_CH_V4:
                  zapSetup =queuData; 
               //   resetZapper ();
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
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(queuData);
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

            case Q_EVENT_SPARE_V12:

            break;                       


            
            case Q_EVENT_RC_L_R_1_V16:
                  RC_Remote_CSR1=queuData;           
            break;
            
            case Q_EVENT_RC_L_R_3_V17:
                  RC_Remote_CSR3=queuData;             
            break;
            
            case Q_EVENT_AWS_RECONNECT_V18:
               connectAWS();
            break;                                    

            case Q_EVENT_ROOM_AV_RC_V19:
                  Av_Rx=queuData;
                  myBlynk.sendAvRxIndex(Av_Rx);
            break;
            
            case Q_EVENT_RC_L_R_2_V20:
                  RC_Remote_CSR2=queuData;
            break; 
            
            case Q_EVENT_ZAP_TIMER_OFF_V25:
                  zapTimerOff=queuData;               
            break;     
            
                    
             case Q_EVENT_RESET_FREQ_V26:
                  recevierFreq = videoCh[recevierCh].frequency =   freqTable[recevierCh];   
                  receiverAvByFreq ( recevierFreq);        
            break;         
                 
            case Q_EVENT_DVR_ON_OFF_V27:
                  dvrOnOff (queuData);  
            break;   

            case Q_EVENT_SAPRE_V30:  //64

            break;   

            case Q_EVENT_SAPRE_V31:   // 49

            break;  

            case Q_EVENT_SAPRE_V32: // 62

            break;   

            case Q_EVENT_SAPRE_V33:  //51

            break;  

            case Q_EVENT_SAPRE_V34:  // 66

            break;   

            case Q_EVENT_SAPRE_V35: // 53

            break;                  
 
             case Q_EVENT_SAPRE_V36:  //  29

            break;   

            case Q_EVENT_SAPRE_V37:  //63

            break;     

            case Q_EVENT_SAPRE_V38:  // 68

            break;    
            case Q_EVENT_SAPRE_V39:  //27

            break;    
            case Q_EVENT_SAPRE_V40:  //67 
           
            break;                            
                                                                      
            case Q_EVENT_ZAP_V71:
              zapOnOff=queuData;
              resetZapper ();
              //if(!zapOnOff)
              zapSetup =false;
              zapScanOnly = false;
              myBlynk.zapSetupOrScanOnly(false);
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

             case Q_EVENT_SPARE_V121:

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
