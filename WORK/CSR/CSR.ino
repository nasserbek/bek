#include "main.h"
 blynk myBlynk;

#define USE_SERIAL Serial


void setup() 
{
  bool ack = true;
//RELE WITH LOW TRIGGER, ON IF LOW AND OFF IF HIGH
     //pinMode(NETGEER_PIN_0, OUTPUT);
     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     
     pinMode(I2C_1_2_RELAY , OUTPUT);
     pinMode(I2C_3_4_RELAY , OUTPUT);

     //digitalWrite(NETGEER_PIN_0, LOW);// NC ACTIVATE ON POWER ON BY DOING NOTHING
     digitalWrite(AV_RX_DVR_PIN_2, LOW);  // NC DISACTIVATE AV RECEIVER ON POWER ON



     Serial.begin(115200);

     //I2C_INIT();

     

     #ifdef CSR2
        digitalWrite(I2C_1_2_RELAY, HIGH);  //  1
        digitalWrite(I2C_3_4_RELAY, HIGH);  //  1
      //  Wire1.begin(SDA_2, SCL_2);
     #else
        digitalWrite(I2C_1_2_RELAY, LOW);  //  1
        digitalWrite(I2C_3_4_RELAY, LOW);  //  1
       // Wire.begin();
     #endif
    

     Wire.begin();
     delay(500);
     Wire1.begin(SDA_2, SCL_2);

     ack = TCA9548A(0);
     DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));

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
                myBlynk.streamSelect(streamWebDdns);
                myBlynk.sendPulseRepetetion(pulseRC, repetionRC);
             }

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
                    myBlynk.getData ();
                    queuData = myBlynk.blynkData;
                    processBlynkQueu();
                  }

            InternetLoss = false;   resetNetgeerAfterInternetLossTimer = millis();
            netGeerReset = false;   restartAfterResetNG = millis();
          }

       if( !InternetLoss && !blynkConnected)  
          {
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

             // digitalWrite(NETGEER_PIN_0, LOW);
     
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
             // digitalWrite(NETGEER_PIN_0, HIGH);
              if(!routerResetStart){routerResetTimer        = millis();routerResetStart = true;DEBUG_PRINTLN("Netgeer Reset done: ");}
          }

/*
bool pingGoogleConnection(void)
{
       bool pingInternet= Ping.ping("www.google.com");
       DEBUG_PRINT("Ping Google: ");DEBUG_PRINTLN(pingInternet ? F("succesiful") : F("failed"));
       return (pingInternet);
}
*/




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
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 15) {remoteControl(remoteControlRcCh );}
            break;      
                  
            case Q_EVENT_VIDEO_CH_V2:
                recevierCh=queuData;
                DEBUG_PRINT("FB_VIDEO_CH_PATH: ");DEBUG_PRINTLN(queuData);
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh ( recevierCh);
                
            break;

           case Q_EVENT_OTA_GSM_V7:

            break;
 
            case Q_EVENT_REBOOT_V8:
              rebootCmd=queuData;
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(queuData);
              rebootSw();
            break;

           case Q_EVENT_SELECTED_RECIEVER_V9:
               selected_Rx = queuData-1;
              TCA9548A(selected_Rx);
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
     #else
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

                                 
            break;
 


            case Q_EVENT_BLYNK1_V14:
              myBlynk.blynk1();
              DEBUG_PRINT("Switching to Blynk 1");
              if (blynkConnected) myBlynk.sendVersion(ver);
            break;

            case Q_EVENT_NETGEER_V15  :
             myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset from Blynk");
              ResetNetgeer();
            break;

            case Q_EVENT_ROOM_ID_1_TO_5_V3:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
           break;

            case Q_EVENT_ALL_CH_V10:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
           break;
            
            case Q_EVENT_ROOM_ID_6_TO_10_V16:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );            
            break;
            
            case Q_EVENT_ROOM_ID_11_TO_15_V17:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );             
            break;
            
            case Q_EVENT_ROOM_ID_16_TO_20_V18:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );                
            break;                                    

            case Q_EVENT_ROOM_AV_RC_V19:
             Av_Rx=queuData;
             myBlynk.sendAvRxIndex(Av_Rx);
            break;
            
            case Q_EVENT_ROUTER_RESET_TIMER_V23:
                  routerTimer = queuData;
            break;

 
            case Q_EVENT_ROOM_ID_21_25_V25:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );               
            break;     
            
                    
             case Q_EVENT_RESET_FREQ_V26:
                  recevierFreq = videoCh[recevierCh].frequency =   freqTable[recevierCh];   
                  receiverAvByFreq ( recevierFreq);        
            break;         
                 
            case Q_EVENT_DVR_ON_OFF_V27:
                  dvrOnOff (queuData);  
            break;   

            case Q_EVENT_03_SEL_V30:  //64
                  videoCh[12].mux=queuData; 
                  if (!videoCh[12].mux) ch12_on == false ;
            break;   

            case Q_EVENT_21_SEL_V31:   // 49
                  videoCh[2].mux=queuData;  
                  if (!videoCh[2].mux) ch2_on == false ;
            break;  

            case Q_EVENT_27_SEL_V32: // 62
                  videoCh[10].mux=queuData;  
                  if (!videoCh[10].mux) ch10_on == false ;
            break;   

            case Q_EVENT_50_SEL_V33:  //51
                  videoCh[1].mux=queuData;  
                  if (!videoCh[1].mux) ch1_on == false ;
            break;  

            case Q_EVENT_20_SEL_V34:  // 66
                  videoCh[14].mux=queuData;  
                  if (!videoCh[14].mux) ch14_on == false ;
            break;   

            case Q_EVENT_52_SEL_V35: // 53
                  videoCh[3].mux=queuData;  
                  if (!videoCh[4].mux) ch4_on == false ;
            break;                  
 
             case Q_EVENT_53_SEL_V36:  //  29
                  videoCh[7].mux=queuData;  
                  if (!videoCh[7].mux) ch7_on == false ;
            break;   

            case Q_EVENT_22_SEL_V37:  //63
                  videoCh[11].mux=queuData;  
            break;     

            case Q_EVENT_208_SEL_V38:  // 68
                  videoCh[15].mux=queuData;  
            break;    
            case Q_EVENT_215_SEL_V39:  //27
                  videoCh[5].mux=queuData;
            break;    
            case Q_EVENT_217_SEL_V40:  //ROOM 

            break;                            
                                                                      
            case Q_EVENT_ZAP_V71:
              zapOnOff=queuData;
              DEBUG_PRINT("ZAP IS : ");
              DEBUG_PRINTLN(zapOnOff ? F("On") : F("Off"));
              if (zapOnOff) {zaptime= millis();stateMachine =SM_CH1_A;}
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
                receiverAvByCh ( recevierCh);
            break;
                     
            
            case Q_EVENT_AV_CH_MINUS_V91:
                recevierCh -= 1;
                if (recevierCh > MAX_NR_CHANNELS) recevierCh = 1;
                else if (recevierCh < 1) recevierCh = MAX_NR_CHANNELS;
                receiverAvByCh ( recevierCh);
            break;
            
             case Q_EVENT_AV_FR_MINUS_V92:
              recevierFreq -= 1;
              receiverAvByFreq ( recevierFreq);
            break;

           case Q_EVENT_AV_FR_PLUS_V93:
              recevierFreq += 1;
              receiverAvByFreq ( recevierFreq);
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
                          
            case Q_EVENT_WIFI_WEB_V104:

             break;

             case Q_EVENT_OTA_GITHUB_V105:

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

//1 13, 2 14

void zappingAvCh (bool zapCmd, int zapTimer)
{
 if( zapCmd )
  { 
         switch (stateMachine)
          {
            case SM_CH1_A:    //1 51
            case SM_CH1_B:
                  if (videoCh[1].zap ) 
                  {
                    if (stateMachine == SM_CH1_A) {zaptime= millis();stateMachine =SM_CH1_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(1, 10, ch1_on, ch10_on );  
                        delay(1000);
                        recevierCh=videoCh[1].id;
                        receiverAvByCh (recevierCh);
                        stateMachine =SM_CH2_A;
                                                   
                      }
                  }
                else stateMachine =SM_CH2_A;
            break;

            case SM_CH2_A:    //2 49
            case SM_CH2_B:
                if (videoCh[2].zap ) 
                  {
                    if (stateMachine == SM_CH2_A) {zaptime= millis();stateMachine =SM_CH2_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(2, 12, ch2_on, ch12_on );  
                        delay(1000);
                        recevierCh=videoCh[2].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH3_A;
                        
                     }
                  }
                else stateMachine =SM_CH3_A;
            break;
            
            case SM_CH3_A:    //3 53
            case SM_CH3_B:
                if (videoCh[3].zap ) 
                  {
                    if (stateMachine == SM_CH3_A) {zaptime= millis();stateMachine =SM_CH3_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(3, 7, ch2_on, ch12_on );  
                        delay(1000);
                        recevierCh=videoCh[3].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH4_A;
                      }
                  }
                else stateMachine =SM_CH4_A;
            break;

            case SM_CH4_A:    //4 24
            case SM_CH4_B:
                if (videoCh[4].zap ) 
                  {
                    if (stateMachine == SM_CH4_A) {zaptime= millis();stateMachine =SM_CH4_B;}
                    if (millis() - zaptime > zapTimer) 
                         {
                        //  Switching(4, 7, ch4_on, ch7_on );  
                         // delay(1000);
                          recevierCh=videoCh[4].id;
                          receiverAvByCh ( recevierCh);
                          stateMachine =SM_CH5_A;
                          
                         }
                   }
                else stateMachine =SM_CH5_A;
            break;
                       
            case SM_CH5_A:    //5 27
            case SM_CH5_B:
                if (videoCh[5].zap ) 
                  {
                    if (stateMachine == SM_CH5_A) {zaptime= millis();stateMachine =SM_CH5_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(5, 14, ch2_on, ch12_on );  
                      delay(1000);
                      recevierCh=videoCh[5].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH6_A;
                     }
                  }
                else stateMachine =SM_CH6_A;
            break;
                       
            case SM_CH6_A:    //6 ***
            case SM_CH6_B:
                if (videoCh[6].zap ) 
                  {
                    if (stateMachine == SM_CH6_A) {zaptime= millis();stateMachine =SM_CH6_B;}
                    if (millis() - zaptime > zapTimer) 
                     {
                      recevierCh=videoCh[6].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH7_A;
                     }
                  }
                else stateMachine =SM_CH7_A;
            break;
                       
            case SM_CH7_A:    //7 29
            case SM_CH7_B:
                if (videoCh[7].zap ) 
                  {
                    if (stateMachine == SM_CH7_A) {zaptime= millis();stateMachine =SM_CH7_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(7, 3, ch7_on, ch4_on ); 
                        delay(1000); 
                        recevierCh=videoCh[7].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH8_A;
                        
                       }
                  }
                else stateMachine =SM_CH8_A;
            break;
                       
            case SM_CH8_A:    //8 48
            case SM_CH8_B:
                if (videoCh[8].zap) 
                  {
                    if (stateMachine == SM_CH8_A) {zaptime= millis();stateMachine =SM_CH8_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        recevierCh=videoCh[8].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH9_A;
                      }
                  }
                else stateMachine =SM_CH9_A;
            break;

            case SM_CH9_A:    //9 52
            case SM_CH9_B:
                if (videoCh[9].zap) 
                  {
                    if (stateMachine == SM_CH9_A) {zaptime= millis();stateMachine =SM_CH9_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[9].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH10_A;
                    }
                  }
                else stateMachine =SM_CH10_A;
            break;
            
            case SM_CH10_A:   //10 62
            case SM_CH10_B:
                if (videoCh[10].zap) 
                  {
                    if (stateMachine == SM_CH10_A) {zaptime= millis();stateMachine =SM_CH10_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(10, 1, ch10_on, ch1_on );
                      delay(1000);
                      recevierCh=videoCh[10].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH11_A;
                      
                      }
                  }
                else stateMachine =SM_CH11_A;
            break; 
             
            case SM_CH11_A:   //11 63
            case SM_CH11_B:
                if (videoCh[11].zap) 
                  {
                    if (stateMachine == SM_CH11_A) {zaptime= millis();stateMachine =SM_CH11_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(11, 15, ch11_on, ch15_on );  //63 68
                      delay(1000);
                      recevierCh=videoCh[11].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH12_A;
                      
                    }
                  }
                else stateMachine =SM_CH12_A;
            break;     

            case SM_CH12_A:   //12 64
            case SM_CH12_B:
                if (videoCh[12].zap) 
                  {
                    if (stateMachine == SM_CH12_A) {zaptime= millis();stateMachine =SM_CH12_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(12, 2, ch12_on, ch2_on );  
                      delay(1000);
                      recevierCh=videoCh[12].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH13_A;
                      
                    }
                  }
                else stateMachine =SM_CH13_A;
            break;  
            
            case SM_CH13_A:   // 13 65
            case SM_CH13_B:
                if (videoCh[13].zap) 
                  {
                    if (stateMachine == SM_CH13_A) {zaptime= millis();stateMachine =SM_CH13_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[13].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH14_A;
                    }
                  }
                else stateMachine =SM_CH14_A;
            break;  

            case SM_CH14_A:   //14 66
            case SM_CH14_B:
                if (videoCh[14].zap) 
                  {
                    if (stateMachine == SM_CH14_A) {zaptime= millis();stateMachine =SM_CH14_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(14, 5, ch2_on, ch12_on );  
                      delay(1000);
                      recevierCh=videoCh[14].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH15_A;
                    }
                  }
                else stateMachine =SM_CH15_A;
            break;  
            
            case SM_CH15_A:   //15  68
            case SM_CH15_B:
                if (videoCh[15].zap) 
                  {
                    if (stateMachine == SM_CH15_A) {zaptime= millis();stateMachine =SM_CH15_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(15, 11, ch15_on, ch11_on );  //63 68
                      delay(1000);
                      recevierCh=videoCh[15].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH16_A;
                      
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH16_A;}
            break;                         

            case SM_CH16_A:   //16
            case SM_CH16_B:
                if (videoCh[16].zap) 
                  {
                    if (stateMachine == SM_CH16_A) {zaptime= millis();stateMachine =SM_CH16_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[16].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH17_A;
                      if (videoCh[16].mux && videoCh[5].zap)remoteControl(16); //48 and 22 off 22
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH17_A;}
            break;                         

            case SM_CH17_A:   //17
            case SM_CH17_B:
                if (videoCh[17].zap) 
                  {
                    if (stateMachine == SM_CH17_A) {zaptime= millis();stateMachine =SM_CH17_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[17].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH18_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH18_A;}
            break;                         


            case SM_CH18_A:   //18
            case SM_CH18_B:
                if (videoCh[18].zap) 
                  {
                    if (stateMachine == SM_CH18_A) {zaptime= millis();stateMachine =SM_CH18_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[18].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH19_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH19_A;}
            break;                         

            case SM_CH19_A: //19
            case SM_CH19_B:
                if (videoCh[19].zap) 
                  {
                    if (stateMachine == SM_CH19_A) {zaptime= millis();stateMachine =SM_CH19_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[19].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH20_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH20_A;}
            break;                         

            case SM_CH20_A:  //20
            case SM_CH20_B:
                if (videoCh[20].zap) 
                  {
                    if (stateMachine == SM_CH20_A) {zaptime= millis();stateMachine =SM_CH20_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      recevierCh=videoCh[20].id;
                      receiverAvByCh ( recevierCh);
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


void Switching(int id1, int id2, bool chOnA, bool chOnB )
{

  if (videoCh[id1].zap)
  {
    if (videoCh[id1].mux && (!(videoCh[id2].mux) ))remoteControl(id1); //switch 1 10
    else if (videoCh[id2].mux && (!(videoCh[id1].mux) ))remoteControl(id2); //switch 10 62
    else if (videoCh[id2].mux && videoCh[id1].mux )
    {

    if ( chOnA == true && chOnB  == false)
      {
        remoteControl(id2); //switch 10 62  10 ON
        delay(1000);
        remoteControl(id1); //switch 10 62   62 OFF
        chOnA = false;
        chOnB  = true;
      }
    else if ( chOnB  == true && chOnA == false)
      {
        remoteControl(id1); //switch 10 62
        delay(1000);
        remoteControl(id2); //switch 10 62
        chOnA = true;
        chOnB  = false;
      }
    else
      {
        remoteControl(id1); //switch 10 62
        delay(500);
        remoteControl(id2); //switch 10 62
        chOnA = true;
        chOnB  = false;
      }
    }
                              
  }
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

/**********************************************************************************************************************************************/

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
     
     /*
     switch (bus)
                    {
                      case 0:
                      case 1:
                          Wire1.beginTransmission(0x70);  // TCA9548A address is 0x70
                          Wire1.write(1 << bus);          // send byte to select bus
                          return(Wire1.endTransmission());  
                      break;

                      case 2:
                      case 3:
                          Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
                          Wire.write(1 << bus);          // send byte to select bus
                          return(Wire.endTransmission()); 
                      break;                     
                    }
*/
  
}

bool Tuner_PLL( int receiver, int _address, uint _pll)
{
  byte MSB = (_pll & 0xFF00) >> 8   ;   //mask LSB, shift 8 bits to the right
  byte LSB = _pll & 0x00FF     ;        //mask MSB, no need to shift

  /*
  switch (receiver)
                    {
                      case 0:
                      case 1:
                            Wire1.beginTransmission(_address);
                            Wire1.write(MSB );
                            Wire1.write(LSB );
                            Wire1.write(0xC2 );
                            return (Wire1.endTransmission() );                        break;
                      break;

                      case 2:
                      case 3:
                            Wire.beginTransmission(_address);
                            Wire.write(MSB );
                            Wire.write(LSB );
                            Wire.write(0xC2 );
                            return (Wire.endTransmission() );                      
                       break;                     
                    }   
*/

     #ifdef CSR2
        Wire1.beginTransmission(_address);
        Wire1.write(MSB );
        Wire1.write(LSB );
        Wire1.write(0xC2 );
        return (Wire1.endTransmission() );  
     #else
          Wire.beginTransmission(_address);
          Wire.write(MSB );
          Wire.write(LSB );
          Wire.write(0xC2 );
          return (Wire.endTransmission() );  
     #endif
 
}