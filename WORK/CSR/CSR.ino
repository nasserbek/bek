#include "main.h"
blynk myBlynk;

void setup() 
{
     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     pinMode(I2C_1_2_RELAY , OUTPUT);
     pinMode(I2C_3_4_RELAY , OUTPUT);

     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // AV RECEIVER ON POWER UP NC CONTACT
     digitalWrite(I2C_1_2_RELAY, HIGH);   
     digitalWrite(I2C_3_4_RELAY, HIGH);  
       
     Serial.begin(115200);
     Wire.begin();
     delay(500);
     Wire1.begin(SDA_2, SCL_2);

     initWDG(MIN_5,EN);
     resetWdg();    //reset timer (feed watchdog) 
     mySwitch.enableTransmit(RC_TX_PIN);
     myBlynk.init();    
     blynkConnected=myBlynk.blynkStatus();
     wifiAvailable = myBlynk.wifiStatus();

     DEBUG_PRINT("Blynk: ");DEBUG_PRINTLN( blynkConnected ? F("Connected") : F("Not Connected"));
     if (blynkConnected) 
              {
                myBlynk.sendAvRxIndex(Av_Rx);
                     #ifdef CSR    
                        myBlynk.RelaySelect(3);
                     #endif 
                       #ifdef CSR2    
                        myBlynk.RelaySelect(1);
                     #endif 
                      #ifdef CSR3    
                        myBlynk.RelaySelect(3);
                     #endif   
                myBlynk.dvrSwitch(1);
                myBlynk.sendVersion(VERSION_ID);
                myBlynk.TerminalPrint(WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) );
             }
    /********************* AWS MQTT BROKER ****************/
    connectAWS();
    /*****************************************************/   
     
    internetSurvilanceTimer = millis();
    liveTimerOff            = millis();
    liveTimerOn             = millis();
    OtaTimeoutTimer            = millis();
    restartAfterResetNG     = millis();
    NetgeerResetGooglLostTimer = millis();
    blynkNotActiveTimer     = millis();
    routerResetTimer        = millis();
    resetNetgeerAfterInternetLossTimer = millis();
   
    createHandleGroup();
    looadRoomData();
    enableWDG(DIS);
    initWDG(SEC_60,EN);
}


void loop(void) 
{
       resetWdg();    //reset timer (feed watchdog) 
     
       blynkConnected=myBlynk.blynkStatus(); 
       
       netgeerCtrl();
     
       if ( blynkConnected )
          {
            myBlynk.blynkRun();
            queuValidData = (xQueueReceive(g_event_queue_handle, &queuDataID, 5 / portTICK_RATE_MS) == pdPASS);
            if(queuValidData) 
                  {
                    if (hmi == BLYNK)    {myBlynk.getData (); queuData = myBlynk.blynkData; }
                    processBlynkQueu();
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
       
      /********************* AWS MQTT BROKER ****************/
         
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
                    if (hmi == NODE_RED) { getDataNodeRed (); queuData = nodeRedData; }
                    processBlynkQueu();
                  }
         }
      /*****************************************************/   
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

             case Q_EVENT_REPEAT_V3:
                  repeatCh =queuData;
             break;


            case Q_SCAN_ACTIVE_CH_V4:
                  scanZapSetup =queuData; 
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
                   OtaGithubGithub= false;         
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
             // TCA9548A(selected_Rx);
             AvReceiverSel(queuData);
                                 
            break;
 
            case Q_EVENT_ZAP_SCAN_ONLY_V10:
                  zapScanOnly = queuData;
            break;

            case Q_EVENT_SPARE_V11:

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
              scanZapSetup =false;
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


void videoChanel(int ch, bool cmd)
{
    zaptime= millis(); zaptimeOff= millis();  //Stop Zap timers to show another room not in Zap list on the fly
    
    if (lastSelectedCh !=0 && !zapOnOff && !scanZapSetup && lastSelectedCh != ch) myBlynk.TurnOffLastCh(lastSelectedCh,CH_MODE_0);
    if (scanZapSetup)videoCh[ch].zap=cmd;
    if(!scanZapSetup)
    {
      remoteControlRcCh = ch;
      recevierCh        = ch;
      room ( remoteControlRcCh, recevierCh , Av_Rx );
    }
    if (hmi == NODE_RED && cmd==0) myBlynk.TurnOffLastCh(ch,CH_MODE_0);
    if (hmi == NODE_RED && cmd==1) myBlynk.TurnOffLastCh(ch,CH_MODE_2);
    lastSelectedCh = ch;
}

/**************************************************ZAPPING ZONE***************************************************************/

void resetRemoteRC(void)
{
      RC_Remote_CSR1 =false; myBlynk.resetRemoteRC(1);
      RC_Remote_CSR2 =false; myBlynk.resetRemoteRC(2);
      RC_Remote_CSR3 =false;  myBlynk.resetRemoteRC(3);  
}
void resetZapper (void)
{
  zaptime= millis(); 
  zaptimeOff= millis();
  scantime= millis();
  resetRemoteRC();
  previousCh =0;
  repeatCh = false;
  stateMachine =SM_CH1_A;
  
}


void nextState( int nextSm)
    {
      stateMachine =nextSm;
      resetRemoteRC();      
    } 

 void automaticOn(int chanel)
    {
        #ifdef CSR3      //
          if  ( chanel == R_24 || ( chanel >= R_48 &&  chanel <= R_68 ) ) RC_Remote_CSR1 =true;
        #endif       


         #ifdef CSR2      //24 25 26 27 28
          if  ( chanel >= R_48 &&  chanel <= R_68) RC_Remote_CSR1 =true;
        #endif      

        #ifdef CSR      //48 - 68
          if  ( chanel == R_24 ||  chanel == R_25 || chanel == R_26 || chanel == R_27 || chanel == R_28 || chanel == R_29 ) RC_Remote_CSR2 =true;
         #endif  
    }    


void automaticOff(int chanel)
    {
        #ifdef CSR3      //
          if  (  chanel == R_25 || chanel == R_26 || chanel == R_27 || chanel == R_28 || chanel == R_29) RC_Remote_CSR2 =true;
          if  (  chanel == R_24 || (chanel >= R_48 &&   chanel <= R_68)) RC_Remote_CSR1 =true;
        #endif       

         
        #ifdef CSR2      //24 25 26 27 28
          if  ( chanel >= R_48 &&   chanel <= R_68) RC_Remote_CSR1 =true;
        #endif      

        #ifdef CSR      //48 - 68
          if  ( chanel == R_24 ||  chanel == R_25 || chanel == R_26 || chanel == R_27 || chanel == R_28 || chanel == R_29 ) RC_Remote_CSR2 =true;
         #endif  
    } 
        
void turnOn (int ch, int prevCh,  int smb,  int sma)
    {
      myBlynk.TurnOffLastCh(ch,CH_MODE_1);
      myBlynk.TurnOffLastCh(prevCh,CH_MODE_2);
      
      zaptime= millis();
      zaptimeOff= millis(); 
      scantime= millis();
      repeatCh = false; 
      myBlynk.repeatSync(repeatCh);
      
      stateMachine =smb;
      
      if (!autoRemoteLocalRc) automaticOn(ch);
      if (prevCh == 0){recevierCh=videoCh[ch].id;     receiverAvByCh (recevierCh);}
      if (!zapScanOnly) remoteControl(ch);   
     }
     
void turnOff(int ch, int prevCh, int smc )
 {  
          if (autoRemoteLocalRc) automaticOff(prevCh);
          if (prevCh != 0)
            {
              recevierCh=videoCh[ch].id; 
              receiverAvByCh (recevierCh); 
              if (!zapScanOnly) remoteControl(prevCh);
            }
          stateMachine =smc;
 }
 
 void zapping (int ch, int sma, int smb, int smc, int nextSm)
{
  if (videoCh[ch].zap) 
      {
        if (stateMachine == sma || repeatCh == true) turnOn(ch,previousCh, smb, sma); 
  
            if ( (stateMachine == smb) &&  (millis() - zaptimeOff > zapTimerOff )  ) turnOff(ch, previousCh, smc);
            if ( (stateMachine == smc) && (millis() - zaptime > zapTimer ) ) { nextState( nextSm); previousCh = ch;}
      }  
  else nextState(nextSm);                     
}

void zappingAvCh (bool zapCmd, int zapTimer)
{
 if( (Av_Rx !=1) || scanZapSetup )  //Stop Zapping if RC or Both is selected or Zsetup
  {
      zaptime= millis();
      zaptimeOff= millis(); 
  }
  
 if( zapCmd && (Av_Rx==1) && (!scanZapSetup) )
  { 
         switch (stateMachine)
          {
            case SM_CH1_A:    //24
            case SM_CH1_B:
            case SM_CH1_C:
                        zapping (1, SM_CH1_A, SM_CH1_B, SM_CH1_C, SM_CH2_A);
            break;

            case SM_CH2_A:    //25 48 67    2 - 7 -18
            case SM_CH2_B:
            case SM_CH2_C:
                        zapping (2, SM_CH2_A, SM_CH2_B, SM_CH2_C, SM_CH3_A);
            break;
            
            case SM_CH3_A:    //26  65   3-16
            case SM_CH3_B:
            case SM_CH3_C:
                        zapping (3, SM_CH3_A, SM_CH3_B, SM_CH3_C, SM_CH4_A);
            break;

            case SM_CH4_A:    //27  50  66  4-9-17  
            case SM_CH4_B:
            case SM_CH4_C:
                        zapping (4, SM_CH4_A, SM_CH4_B, SM_CH4_C, SM_CH5_A);
            break;
                       
            case SM_CH5_A:    //28  52  5-11
            case SM_CH5_B:
            case SM_CH5_C:
                        zapping (5, SM_CH5_A, SM_CH5_B, SM_CH5_C, SM_CH6_A);
            break;
                       
            case SM_CH6_A:    //29 53   6-12
            case SM_CH6_B:
            case SM_CH6_C:
                        zapping (6, SM_CH6_A, SM_CH6_B, SM_CH6_C, SM_CH7_A);
            break;
                       
            case SM_CH7_A:    //48-25-67  7 - 2 - 18
            case SM_CH7_B:
            case SM_CH7_C:
                        zapping (7, SM_CH7_A, SM_CH7_B, SM_CH7_C, SM_CH8_A);
            break;
                       
            case SM_CH8_A:    //49 64  8-15
            case SM_CH8_B:
            case SM_CH8_C:
                        zapping (8, SM_CH8_A, SM_CH8_B, SM_CH8_C, SM_CH9_A);
            break;

            case SM_CH9_A:    //50 27 66  9-4-17
            case SM_CH9_B:
            case SM_CH9_C:
                        zapping (9, SM_CH9_A, SM_CH9_B, SM_CH9_C, SM_CH10_A);
            break;
            
            case SM_CH10_A:   //51 62  10-13
            case SM_CH10_B:
            case SM_CH10_C:
                        zapping (10, SM_CH10_A, SM_CH10_B, SM_CH10_C, SM_CH11_A);
            break; 
             
            case SM_CH11_A:   //52 28  11-5
            case SM_CH11_B:
            case SM_CH11_C:
                        zapping (11, SM_CH11_A, SM_CH11_B, SM_CH11_C, SM_CH12_A);
            break;     

            case SM_CH12_A:   //53 29   12-6
            case SM_CH12_B:
            case SM_CH12_C:
                        zapping (12, SM_CH12_A, SM_CH12_B, SM_CH12_C, SM_CH13_A);
            break;  
            
            case SM_CH13_A:   // 62 51  13-10
            case SM_CH13_B:
            case SM_CH13_C:
                        zapping (13, SM_CH13_A, SM_CH13_B, SM_CH13_C, SM_CH14_A);
            break;  

            case SM_CH14_A:   //63 68  14-19
            case SM_CH14_B:
            case SM_CH14_C:
                        zapping (14, SM_CH14_A, SM_CH14_B, SM_CH14_C, SM_CH15_A);
            break;  
            
            case SM_CH15_A:   //64 49  15-8
            case SM_CH15_B:
            case SM_CH15_C:
                        zapping (15, SM_CH15_A, SM_CH15_B, SM_CH15_C, SM_CH16_A);
            break;                         

            case SM_CH16_A:   //65 26  16-3
            case SM_CH16_B:
            case SM_CH16_C:
                        zapping (16, SM_CH16_A, SM_CH16_B, SM_CH16_C, SM_CH17_A);
            break;                         

            case SM_CH17_A:   //66 27 50  17-4-9
            case SM_CH17_B:
            case SM_CH17_C:
                        zapping (17, SM_CH17_A, SM_CH17_B, SM_CH17_C, SM_CH18_A);
            break;                         


            case SM_CH18_A:   //67-25-48  18 - 2 - 7
            case SM_CH18_B:
            case SM_CH18_C:
                        zapping (18, SM_CH18_A, SM_CH18_B, SM_CH18_C, SM_CH19_A);
            break;                         

            case SM_CH19_A: //68  63  19-14
            case SM_CH19_B:
            case SM_CH19_C:
                        zapping (19, SM_CH19_A, SM_CH19_B, SM_CH19_C, SM_CH20_A);
            break;                         

            case SM_CH20_A:  //20
            case SM_CH20_B:
            case SM_CH20_C:
                        zapping (20, SM_CH20_A, SM_CH20_B, SM_CH20_C, SM_CH1_A);
            break;                         

                                                           
          }
  }
  else if (!zapCmd) {zaptime= millis();stateMachine =SM_CH1_A;}
}

/**************************************************END OF ZAPPING ZONE***************************************************************/


/**************************************************VIDEO RC CONTROL ZONE***************************************************************/
void remoteControl(int cmd )
{
       if( (!RC_Remote_CSR1) && (!RC_Remote_CSR2)  && (!RC_Remote_CSR3))
        {
         mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);
        }
      else if( RC_Remote_CSR1)
       {
        doc2["RC"] = cmd;
        serializeJson(doc2, Json); // print to client
        client.publish(AWS_IOT_PUBLISH_TOPIC_RC, Json); 
      }

       else if( RC_Remote_CSR2)
      {
        doc2["RC"] = cmd;
        serializeJson(doc2, Json); // print to client
        client.publish(AWS_IOT_PUBLISH_TOPIC_RC_2, Json); 
      } 

       else if( RC_Remote_CSR3)
      {
        doc2["RC"] = cmd;
        serializeJson(doc2, Json); // print to client
        client.publish(AWS_IOT_PUBLISH_TOPIC_RC_3, Json); 
      } 
}

        
void receiverAvByCh (int Ch)
{
  bool ack;
  int PLL_value;
  
       if (blynkConnected) myBlynk.blynkAckLed(true);
       ack = Tuner_PLL(selected_Rx, av_pll_addr, _pll[Ch]); 
       delay(500);
       
       if (blynkConnected) {myBlynk.blynkAckLed( ack); myBlynk.sevenSegValue(Ch);}
       recevierFreq =videoCh[Ch].frequency;   
       
       if (blynkConnected) {myBlynk.frequencyValue(recevierFreq );myBlynk.visualActiveRoom(Ch,zapOnOff );}
       DEBUG_PRINT("Received freq channel:");DEBUG_PRINTLN(Ch);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}


void receiverAvByFreq ( int Freq)
{
  bool ack=0;
       recevierFreq =Freq;
       if (blynkConnected) myBlynk.blynkAckLed( true); 
        videoCh[recevierCh].frequency = Freq;
       _pll[recevierCh] =( 512 * (Freq + 479.5) ) / 64 ;
       ack = Tuner_PLL(selected_Rx, av_pll_addr, _pll[recevierCh]);
       if (blynkConnected)  { myBlynk.blynkAckLed(ack);myBlynk.frequencyValue(Freq );}
       DEBUG_PRINT("Received manual_freq:");DEBUG_PRINTLN(manual_freq);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}

void room ( int RC, int AV, int sel)
{
     switch (sel)
          {
            case 1:
                receiverAvByCh ( AV);
            break;

            case 2:
                remoteControl(RC);
            break;

            
            case 3:
                receiverAvByCh ( AV);
                remoteControl(RC);
            break;
          }
}                            

void AvReceiverSel(int queuData)
 {            
      #ifdef CSR
                switch (queuData)
                    {
                      case 1:
                                 digitalWrite(I2C_1_2_RELAY, HIGH);  //  
                      break;
                      case 2:
                                 digitalWrite(I2C_1_2_RELAY, LOW);  //  
                      break;

                       case 3:
                                 digitalWrite(I2C_3_4_RELAY, HIGH);  //  
                      break;
                      case 4:
                                 digitalWrite(I2C_3_4_RELAY, LOW);  //  
                      break;                     
                    }  
     #endif

     #ifdef CSR2
                switch (queuData)
                    {
                      case 1:
                                 digitalWrite(I2C_1_2_RELAY, HIGH);  //  
                      break;
                      case 2:
                                 digitalWrite(I2C_1_2_RELAY, LOW);  //  
                      break;

                       case 3:
                                 digitalWrite(I2C_3_4_RELAY, HIGH);  //  
                      break;
                      case 4:
                                 digitalWrite(I2C_3_4_RELAY, LOW);  //  
                      break;                     
                    }  
     #endif
                     
     #ifdef CSR3
                switch (queuData)
                    {
                      case 1:
                                 digitalWrite(I2C_1_2_RELAY, LOW);  //  
                      break;
                      case 2:
                                 digitalWrite(I2C_1_2_RELAY, HIGH);  //  
                      break;

                       case 3:
                                 digitalWrite(I2C_3_4_RELAY, LOW);  //  
                      break;
                      case 4:
                                 digitalWrite(I2C_3_4_RELAY, HIGH);  //  
                      break;                     
                    }   
     #endif
}

bool  TCA9548A(uint8_t bus)
{

       #ifdef CSR2
                          Wire1.beginTransmission(0x70);  // TCA9548A address is 0x70
                          Wire1.write(1 << bus);          // send byte to select bus
                          return(Wire1.endTransmission());  
      #else
                          Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
                          Wire.write(1 << bus);          // send byte to select bus
                          return(Wire.endTransmission()); 
      #endif
}

bool Tuner_PLL( int receiver, int _address, uint _pll)
{
  byte MSB = (_pll & 0xFF00) >> 8   ;   //mask LSB, shift 8 bits to the right
  byte LSB = _pll & 0x00FF     ;        //mask MSB, no need to shift

#ifdef CSR   //4CH 4 Relays Active LOW and 2 I2C Controllers
  switch (receiver)
        {
          case 2:
          case 3:
                Wire1.beginTransmission(_address);
                Wire1.write(MSB );
                Wire1.write(LSB );
                Wire1.write(0xC2 );
                return (Wire1.endTransmission() );                       
          break;

          case 0:
          case 1:
                Wire.beginTransmission(_address);
                Wire.write(MSB );
                Wire.write(LSB );
                Wire.write(0xC2 );
                return (Wire.endTransmission() );                      
            break;                     
        }   
#endif

#ifdef CSR2   //4CH 4 Relays Active LOW and 2 I2C Controllers
  switch (receiver)
        {
          case 2:
          case 3:
                Wire1.beginTransmission(_address);
                Wire1.write(MSB );
                Wire1.write(LSB );
                Wire1.write(0xC2 );
                return (Wire1.endTransmission() );                       
          break;

          case 0:
          case 1:
                Wire.beginTransmission(_address);
                Wire.write(MSB );
                Wire.write(LSB );
                Wire.write(0xC2 );
                return (Wire.endTransmission() );                      
            break;                     
        }   
#endif

#ifdef CSR3   //2CH 2 Relays Active HIGH and MAIN I2C Controllers
          Wire.beginTransmission(_address);
          Wire.write(MSB );
          Wire.write(LSB );
          Wire.write(0xC2 );
          return (Wire.endTransmission() );  
#endif
}
/**************************************************END OF VIDEO RC CONTROL ZONE***************************************************************/

/****************************************************DON'T TOUCH********************************************************************/
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


void rebootSw(void)
{
 ESP.restart();
}

void  dvrOnOff (bool onOff)
{
   if (onOff) digitalWrite(AV_RX_DVR_PIN_2, LOW); 
   else digitalWrite(AV_RX_DVR_PIN_2, HIGH); 
}

void netgeerCtrl(void)
{
       if ( (  (millis() - routerResetTimer) >= routerTimer) && routerResetStart)
                {
    
                routerResetStart=false;
                routerResetTimer        = millis();
                restartAfterResetNG     = millis();
                netGeerReset = true;
                blynkInitDone = false;
                pingGoogle =false;
               }

       if ( ( (millis() - resetNetgeerAfterInternetLossTimer) >= INTERNET_LOSS_TO_RESET_NG_TIMER) && InternetLoss && !blynkConnected && !netGeerReset)
        {
              DEBUG_PRINTLN("Blynk Disconnected for 2 min, Reset Netgeer");
                if(!routerResetStart){routerResetTimer        = millis();routerResetStart = true;DEBUG_PRINTLN("Netgeer Reset done: ");}
        }

       if (  ( (millis() - restartAfterResetNG) >=  RESTART_AFTER_NG_RESET_TIMER) && netGeerReset )
          {
            DEBUG_PRINTLN("Resetaring 30 min after Netgeer Rreset");
            ESP.restart(); 
          }
}     

void ResetNetgeer(void)
          {
              if(!routerResetStart)
              {
                routerResetTimer        = millis();
                routerResetStart = true;
                DEBUG_PRINTLN("Netgeer Reset done: ");
              }
          }

/*
bool pingGoogleConnection(void)
{
       bool pingInternet= Ping.ping("www.google.com");
       DEBUG_PRINT("Ping Google: ");DEBUG_PRINTLN(pingInternet ? F("succesiful") : F("failed"));
       return (pingInternet);
}
*/

int stringToInteger(String str)
{
char carray[5]; 
      str.toCharArray(carray, sizeof(carray));
      return ( atoi(carray));  
}

void goToDeepSleep(int sleepTimer)
{
    //  sendToHMI("Going to Deep Sleep", "Going to Deep Sleep", "Going to Deep Sleep",FB_NOTIFIER, "Going to Deep Sleep" );
    //  sms.sim800PowerOn(false)  ;
      DEBUG_PRINT("Sleep for: ");  DEBUG_PRINT(sleepTimer * 60* 1000000);DEBUG_PRINTLN(" uSec");
      esp_sleep_enable_timer_wakeup(sleepTimer * 60 * 1000000); // in microseconds
      Serial.flush(); 
      esp_deep_sleep_start();
}

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
/*************************************************END OF DON'T TOUCH*********************************************************************************************/
    


/*************************************************OTA ZONE********************************************************************************************/
 void firmwareUpdate(void) {
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
    t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

    switch (ret) {
    case HTTP_UPDATE_FAILED:
        myBlynk.TerminalPrint("HTTP_UPDATE_FAILD Error (%d): %s\n"); //, httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        myBlynk.TerminalPrint("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        myBlynk.TerminalPrint("HTTP_UPDATE_OK");
        break;
    }
}

int CSRFirmwareVersionCheck(void) {
    String payload;
    int httpCode;
    String FirmwareURL = "";
    FirmwareURL += URL_fw_Version;
    FirmwareURL += "?";
    FirmwareURL += String(rand());
    myBlynk.TerminalPrint("Local Web connecto to http://esp32.local with admin admin " );
    myBlynk.TerminalPrint(FirmwareURL);
    WiFiClientSecure * client = new WiFiClientSecure;

    if (client) {
        client -> setCACert(rootCACertificate);
        HTTPClient https;

        if (https.begin( * client, FirmwareURL)) {
            myBlynk.TerminalPrint("[HTTPS] GET...\n");
            // start connection and send HTTP header
            delay(100);
            httpCode = https.GET();
            delay(100);
            if (httpCode == HTTP_CODE_OK) // if version received
            {
                payload = https.getString(); // save received version
            } else {
                myBlynk.TerminalPrint("Error Occured During Version Check: ");
                myBlynk.TerminalPrint(String(httpCode));
            }
            https.end();
        }
        delete client;
    }

    if (httpCode == HTTP_CODE_OK) // if version received
    {
        payload.trim();
        if (payload.equals(VERSION_ID)) {
            myBlynk.TerminalPrint("Device  IS Already on Latest Firmware Version: " + String(payload) );
            return 0;
        } else {
            myBlynk.TerminalPrint(payload);
            myBlynk.TerminalPrint("New Firmware Detected");
            return 1;
        }
    }
    return 0;
}

void OtaGithub(void) {
  myBlynk.TerminalPrint("Starting Ota Web Update from Github");
  while (!OtaGithubGithub) 
       {
        enableWDG(false);
        if (  millis() - OtaTimeoutTimer > WIFI_IDE_TIMER )
        {
           OtaGithubGithub = true;
           resetWdg();
           enableWDG(true);
           OtaTimeoutTimer = millis();
           ESP.restart();
        }
        
    // if ( CSRFirmwareVersionCheck() )    
    firmwareUpdate();
  }
}

void wifiUploadCtrl(void)
{}

void localWebWifiOtaSetup(void)
{
  myBlynk.TerminalPrint("Local Web connecto to http://esp32.local with admin admin " );
  //use mdns for host name resolution
  if (!MDNS.begin(host)) { //http://esp32.local
     myBlynk.TerminalPrint("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  
   myBlynk.TerminalPrint("mDNS responder started");
  //return index page which is stored in serverIndex 
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });

    server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

 //handling uploading firmware file 
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
      // flashing firmware to ESP
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

 void localWebWifiOta (void)
 {
   localWebWifiOtaSetup();
   while (!wifiWebUpdater) 
       {
        enableWDG(false);
        if (  millis() - OtaTimeoutTimer > WIFI_IDE_TIMER )
        {
           wifiWebUpdater = true;
           resetWdg();
           enableWDG(true);
           OtaTimeoutTimer = millis();
           ESP.restart();
        }
        server.handleClient();
        delay(1);
       }
 }


void otaIdeSetup (void)
     {}
/*************************************************END OF OTA ZONE********************************************************************************************/


/*************************************************NODE RED AWS IOT ZONE********************************************************************************************/
void connectAWS()
{
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  //client.setCallback(messageHandler); 
  client.setCallback(callback);
  myBlynk.TerminalPrint("Connecting Client to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
    return;
  }
   if (!client.connected())
  {
    myBlynk.TerminalPrint("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAP);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_RX);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_AV_RC);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_DVR);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_REBOOT);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_SCAN);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_REPEAT);  
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_PRESET); 
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO);  
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON); 
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_RC);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA);    
  
  doc2["version"] = VERSION_ID;
  serializeJson(doc2, Json); // print to client
  client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VERSION , Json);
  myBlynk.TerminalPrint("AWS IoT Connected!");
}

void retriveDataFromTopic (char* topic, byte* payload, unsigned int length )
{
        for (int i=0;i<length;i++) //Converts the received message to String
        {      
          resultS= resultS + (char)payload[i];
        }

        DeserializationError error = deserializeJson(doc1, resultS); //Command to derealize the received Json
        if (error) 
        {
          myBlynk.TerminalPrint(F("deserializeJson() failed with code "));
          myBlynk.TerminalPrint(error.f_str());
        } 
        _nodeRedEvent = true; 
        hmi = NODE_RED; 
}

bool getDataNodeRed(void)
{
    if (_nodeRedEvent)
    {
      nodeRedData=_nodeRedData;
      nodeRedEventID =_nodeRedEventID;
      _nodeRedEvent = false;
      nodeRedActive = true; 
      return true;
    }  
    else return false;
}

int getChID(int ch)
{
        switch (ch)
        {
          case 1:
                return (Q_EVENT_RM_ID_10_V112);
          break;

          case 2:
                    return (Q_EVENT_RM_ID_02_V122);
          break;
          case 3:
                    return (Q_EVENT_RM_ID_03_V123);
          break;
          case 4:
                    return (Q_EVENT_RM_ID_04_V124);
          break;
          case 5:
                    return (Q_EVENT_RM_ID_05_V125);
          break;
          case 6:
                    return (Q_EVENT_RM_ID_06_V126);
          break;
          case 7:
                   return (Q_EVENT_RM_ID_07_V127);
          break;
          case 8:
                    return (Q_EVENT_RM_ID_08_V93);
          break;
          case 9:
                    return (Q_EVENT_RM_ID_09_V80);
          break;
          case 10:
                    return (Q_EVENT_RM_ID_10_V21);
          break;
          case 11:
                   return (Q_EVENT_RM_ID_11_V14);
          break;
          case 12:
                    return (Q_EVENT_RM_ID_12_V15);
          break;
          case 13:
                    return (Q_EVENT_RM_ID_13_V23);
          break;
          case 14:
                    return (Q_EVENT_RM_ID_14_V103);
          break;
          case 15:
                    return (Q_EVENT_RM_ID_15_V104);
          break;
          case 16:
                    return (Q_EVENT_RM_ID_16_V105);
          break;
          case 17:
                    return (Q_EVENT_RM_ID_17_V90);
          break;
          case 18:
                   return (Q_EVENT_RM_ID_18_V91);
          break;
          case 19:
                    return (Q_EVENT_RM_ID_19_V92);
          break;                   
        }  
}     

/********************* AWS MQTT BROKER CALLBACK *******************************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  resultS = "";   //Empty variable from serialized Json
 
  if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_VIDEO)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["CMD"]; 
        int ch        = doc1["VIDEO"] ;
        nodeRedeventdata =getChID (ch) ;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
     }

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_ZAP)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["ZAP"];
        nodeRedeventdata = Q_EVENT_ZAP_V71;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapLed(_nodeRedData);
    }

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_RX)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["RX"];
        nodeRedeventdata = Q_EVENT_SELECTED_RECIEVER_V9;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.RelaySelect(_nodeRedData);
        
    }   
    
  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_AV_RC)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["AVRC"];
        nodeRedeventdata = Q_EVENT_ROOM_AV_RC_V19;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.sendAvRxIndex(_nodeRedData);
      }   

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_DVR)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["DVR"];
        nodeRedeventdata = Q_EVENT_DVR_ON_OFF_V27;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.dvrSwitch(_nodeRedData); 

    }   

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_REBOOT)
    {
        myBlynk.TerminalPrint(" Received topic is: " + String(topic) +"Rebooting ESP");
        ESP.restart();
    }  


else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_SCAN)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["SCAN"];
        nodeRedeventdata = Q_SCAN_ACTIVE_CH_V4;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapSetupOrScanOnly(_nodeRedData); 
     }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_REPEAT)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["REPEAT"];
        nodeRedeventdata = Q_EVENT_REPEAT_V3;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapSetupOrScanOnly(_nodeRedData); 
     }
     
else if(String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_PRESET)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["PRESET"];
        nodeRedeventdata = Q_EVENT_RESET_FREQ_V26;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        
    }
    
else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["ZAPAUTO"];
        nodeRedeventdata = Q_EVENT_AUTOMATIC_RC_L_R_V5;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapAutoLocalRC(_nodeRedData); 
    }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_RC)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["RC"];
        nodeRedeventdata = Q_EVENT_RC_CH_NR_V1;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        
    }
    
 myBlynk.TerminalPrint(" Topic " + String(topic) +" Payload: "+ String(_nodeRedData));
}
/*************************************************END OF NODE RED AWS IOT ZONE********************************************************************************************/
