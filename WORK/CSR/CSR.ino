
#include "main.h"

blynk myBlynk;

#define USE_SERIAL Serial

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");myBlynk.TerminalPrint("incoming: ");
  Serial.println(topic);myBlynk.TerminalPrint(topic);
  
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);myBlynk.TerminalPrint(message);
}


void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived ["); myBlynk.TerminalPrint("Message arrived [");
  Serial.print(topic); myBlynk.TerminalPrint(topic);
  Serial.print("] "); myBlynk.TerminalPrint("] ");
  
  if (strstr(topic, AWS_IOT_SUBSCRIBE_TOPIC))
  {
  
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]); 
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1')
    {
    } 
    else 
    {
    }
  }

  else
  {
    Serial.println("unsubscribed topic");myBlynk.TerminalPrint("unsubscribed topic");
  }
}



void publishMessage()
{

  StaticJsonDocument<200> doc;
//  doc["humidity"] = 54;
//  doc["temperature"] = 35;
  doc["version"] = VERSION_ID;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}



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
  Serial.println("Connecting to AWS IOT");
  myBlynk.TerminalPrint("Connecting Client to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(3000);
    return;
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    myBlynk.TerminalPrint("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
//  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC2);
//  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC3);
//  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC4);
 
  Serial.println("AWS IoT Connected!");
  myBlynk.TerminalPrint("AWS IoT Connected!");
  publishMessage();
}




void setup() 
{

     pinMode(AV_RX_DVR_PIN_2, OUTPUT);
     pinMode(I2C_1_2_RELAY , OUTPUT);
     pinMode(I2C_3_4_RELAY , OUTPUT);

     digitalWrite(AV_RX_DVR_PIN_2, HIGH);  // AV RECEIVER OFF ON POWER ON
    

     #ifdef CSR  //4 rele board active low
        digitalWrite(I2C_1_2_RELAY, HIGH);   
        digitalWrite(I2C_3_4_RELAY, HIGH);  
     #endif   

     #ifdef CSR2  //4 rele board active low
        digitalWrite(I2C_1_2_RELAY, HIGH);   
        digitalWrite(I2C_3_4_RELAY, HIGH);  
     #endif 
          
     #ifdef CSR3      //Single rele active High
        digitalWrite(I2C_1_2_RELAY, LOW);   
        digitalWrite(I2C_3_4_RELAY, LOW);   
     #endif
    
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
                myBlynk.RelaySelect();
                myBlynk.sendPulseRepetetion(pulseRC, repetionRC);
                myBlynk.TerminalPrint(WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) );
                connectAWS();
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
       
       client.loop();   //AWS MQTT  
       
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

             case Q_EVENT_ROOM_ID_1_TO_5_V3:
                  remoteControlRcCh = queuData;
                  recevierCh        = queuData;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;


            case Q_EVENT_SWITCHING_25_V4:
                  videoCh[1].mux=queuData; 
                  if (!videoCh[1].mux) ch1_on == false ;
            break;

            case Q_EVENT_SWITCHING_48_V5:
                  videoCh[7].mux=queuData; 
                  if (!videoCh[7].mux) ch7_on == false ;
            break;         

            case Q_EVENT_SWITCHING_26_V6:
                  videoCh[2].mux=queuData; 
                  if (!videoCh[2].mux) ch2_on == false ;
            break;

            case Q_EVENT_SWITCHING_65_V7:
                  videoCh[16].mux=queuData; 
                  if (!videoCh[16].mux) ch16_on == false ;
            break;
 
            case Q_EVENT_REBOOT_V8:
              rebootCmd=queuData;
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(queuData);
              rebootSw();
            break;

           case Q_EVENT_SELECTED_RECIEVER_V9:
               selected_Rx = queuData-1;
             // TCA9548A(selected_Rx);
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

                                 
            break;
 
            case Q_EVENT_SWITCHING_50_V10:
                  videoCh[9].mux=queuData; 
                  if (!videoCh[9].mux) ch9_on == false ;
            break;

            case Q_EVENT_SWITCHING_28_V11:
                  videoCh[5].mux=queuData; 
                  if (!videoCh[5].mux) ch5_on == false ;
            break;

            case Q_EVENT_SWITCHING_52_V12:
                  videoCh[11].mux=queuData; 
                  if (!videoCh[11].mux) ch11_on == false ;
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

            case Q_EVENT_V30_SWITCHING_64:  //64
                  videoCh[15].mux=queuData; 
                  if (!videoCh[15].mux) ch15_on == false ;
            break;   

            case Q_EVENT_V31_SWITCHING_49:   // 49
                  videoCh[8].mux=queuData;  
                  if (!videoCh[8].mux) ch8_on == false ;
            break;  

            case Q_EVENT_V32_SWITCHING_62: // 62
                  videoCh[13].mux=queuData;  
                  if (!videoCh[13].mux) ch13_on == false ;
            break;   

            case Q_EVENT_V33_SWITCHING_51:  //51
                  videoCh[10].mux=queuData;  
                  if (!videoCh[10].mux) ch10_on == false ;
            break;  

            case Q_EVENT_V34_SWITCHING_66:  // 66
                  videoCh[17].mux=queuData;  
                  if (!videoCh[17].mux) ch17_on == false ;
            break;   

            case Q_EVENT_V35_SWITCHING_53: // 53
                  videoCh[12].mux=queuData;  
                  if (!videoCh[12].mux) ch12_on == false ;
            break;                  
 
             case Q_EVENT_V36_SWITCHING_29:  //  29
                  videoCh[6].mux=queuData;  
                  if (!videoCh[6].mux) ch6_on == false ;
            break;   

            case Q_EVENT_V37_SWITCHING_63:  //63
                  videoCh[14].mux=queuData;  
                  if (!videoCh[14].mux) ch14_on == false ;
            break;     

            case Q_EVENT_V38_SWITCHING_68:  // 68
                  videoCh[19].mux=queuData; 
                  if (!videoCh[19].mux) ch19_on == false ; 
            break;    
            case Q_EVENT_V39_SWITCHING_27:  //27
                  videoCh[4].mux=queuData;
                  if (!videoCh[4].mux) ch4_on == false ;
            break;    
            case Q_EVENT_V40_SWITCHING_67:  //67 
                  videoCh[18].mux=queuData;
                  if (!videoCh[18].mux) ch18_on == false ;            
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
 
             case Q_EVENT_ZAP_CHANNEL20_V112 :
              videoCh[20].zap=queuData;
            break; 

             case Q_EVENT_RM_ID_01_V121:
                  remoteControlRcCh = 1;
                  recevierCh        = 1;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;                       

             case Q_EVENT_RM_ID_02_V122:
                  remoteControlRcCh = 2;
                  recevierCh        = 2;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_03_V123:
                  remoteControlRcCh = 3;
                  recevierCh        = 3;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_04_V124:
                  remoteControlRcCh = 4;
                  recevierCh        = 4;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_05_V125:
                  remoteControlRcCh = 5;
                  recevierCh        = 5;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_06_V126:
                  remoteControlRcCh = 6;
                  recevierCh        = 6;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_07_V127:
                  remoteControlRcCh = 7;
                  recevierCh        = 7;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_08_V93:
                  remoteControlRcCh = 8;
                  recevierCh        = 8;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_09_V80:
                  remoteControlRcCh = 9;
                  recevierCh        = 9;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_10_V21:
                  remoteControlRcCh = 10;
                  recevierCh        = 10;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_11_V14:
                  remoteControlRcCh = 11;
                  recevierCh        = 11;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_12_V15:
                  remoteControlRcCh = 12;
                  recevierCh        = 12;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_13_V23:
                  remoteControlRcCh = 13;
                  recevierCh        = 13;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_14_V103:
                  remoteControlRcCh = 14;
                  recevierCh        = 14;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_15_V104:
                  remoteControlRcCh = 15;
                  recevierCh        = 15;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_16_V105:
                  remoteControlRcCh = 16;
                  recevierCh        = 16;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_17_V90:
                  remoteControlRcCh = 17;
                  recevierCh        = 17;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_18_V91:
                  remoteControlRcCh = 18;
                  recevierCh        = 18;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
             case Q_EVENT_RM_ID_19_V92:
                  remoteControlRcCh = 19;
                  recevierCh        = 19;
                  room ( remoteControlRcCh, recevierCh , Av_Rx );
                   Serial.println(queuData);
             break;  
    }  
    selected_room = recevierCh;
}

/*
SWitching
2 - 7
3 - 16
4 -17 -9
5 -11
6 -12
8 -15
10 - 13
14-18

 */

void zappingAvCh (bool zapCmd, int zapTimer)
{
 if( zapCmd )
  { 
         switch (stateMachine)
          {
            case SM_CH1_A:    //24
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

            case SM_CH2_A:    //25 48 67    2 - 7 -18
            case SM_CH2_B:
                if (videoCh[2].zap ) 
                  {
                    if (stateMachine == SM_CH2_A) {zaptime= millis();stateMachine =SM_CH2_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(2, 7, 18, ch2_on, ch7_on, ch18_on  );  
                        delay(1000);
                        recevierCh=videoCh[2].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH3_A;
                        
                     }
                  }
                else stateMachine =SM_CH3_A;
            break;
            
            case SM_CH3_A:    //26  65   3-16
            case SM_CH3_B:
                if (videoCh[3].zap ) 
                  {
                    if (stateMachine == SM_CH3_A) {zaptime= millis();stateMachine =SM_CH3_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(3, 16, 20, ch3_on, ch16_on, chx_on  );  
                        delay(1000);
                        recevierCh=videoCh[3].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH4_A;
                      }
                  }
                else stateMachine =SM_CH4_A;
            break;

            case SM_CH4_A:    //27  50  66  4-9-17  
            case SM_CH4_B:
                if (videoCh[4].zap ) 
                  {
                    if (stateMachine == SM_CH4_A) {zaptime= millis();stateMachine =SM_CH4_B;}
                    if (millis() - zaptime > zapTimer) 
                         {
                          Switching(4, 9, 17, ch4_on, ch9_on, ch17_on  );  
                          delay(1000);
                          recevierCh=videoCh[4].id;
                          receiverAvByCh ( recevierCh);
                          stateMachine =SM_CH5_A;
                          
                         }
                   }
                else stateMachine =SM_CH5_A;
            break;
                       
            case SM_CH5_A:    //28  52  5-11
            case SM_CH5_B:
                if (videoCh[5].zap ) 
                  {
                    if (stateMachine == SM_CH5_A) {zaptime= millis();stateMachine =SM_CH5_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(5, 11, 20, ch5_on, ch11_on, chx_on  );  
                      delay(1000);
                      recevierCh=videoCh[5].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH6_A;
                     }
                  }
                else stateMachine =SM_CH6_A;
            break;
                       
            case SM_CH6_A:    //29 53   6-12
            case SM_CH6_B:
                if (videoCh[6].zap ) 
                  {
                    if (stateMachine == SM_CH6_A) {zaptime= millis();stateMachine =SM_CH6_B;}
                    if (millis() - zaptime > zapTimer) 
                     {
                      Switching(6, 12, 20, ch6_on, ch12_on, chx_on  );  
                      delay(1000);                      
                      recevierCh=videoCh[6].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH7_A;
                     }
                  }
                else stateMachine =SM_CH7_A;
            break;
                       
            case SM_CH7_A:    //48-25-67  7 - 2 - 18
            case SM_CH7_B:
                if (videoCh[7].zap ) 
                  {
                    if (stateMachine == SM_CH7_A) {zaptime= millis();stateMachine =SM_CH7_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(7, 2, 18, ch7_on, ch2_on, ch18_on  ); 
                        delay(1000); 
                        recevierCh=videoCh[7].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH8_A;
                        
                       }
                  }
                else stateMachine =SM_CH8_A;
            break;
                       
            case SM_CH8_A:    //49 64  8-15
            case SM_CH8_B:
                if (videoCh[8].zap) 
                  {
                    if (stateMachine == SM_CH8_A) {zaptime= millis();stateMachine =SM_CH8_B;}
                    if (millis() - zaptime > zapTimer) 
                      {
                        Switching(8, 15,20, ch8_on, ch15_on, chx_on  );  
                        delay(1000);                        
                        recevierCh=videoCh[8].id;
                        receiverAvByCh ( recevierCh);
                        stateMachine =SM_CH9_A;
                      }
                  }
                else stateMachine =SM_CH9_A;
            break;

            case SM_CH9_A:    //50 27 66  9-4-17
            case SM_CH9_B:
                if (videoCh[9].zap) 
                  {
                    if (stateMachine == SM_CH9_A) {zaptime= millis();stateMachine =SM_CH9_B;}
                    if (millis() - zaptime > zapTimer) 
                      Switching(9, 4, 17, ch9_on, ch4_on, ch17_on  );  
                      delay(1000);                    {
                      recevierCh=videoCh[9].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH10_A;
                    }
                  }
                else stateMachine =SM_CH10_A;
            break;
            
            case SM_CH10_A:   //51 62  10-13
            case SM_CH10_B:
                if (videoCh[10].zap) 
                  {
                    if (stateMachine == SM_CH10_A) {zaptime= millis();stateMachine =SM_CH10_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(10, 13, 20, ch10_on, ch13_on, chx_on  );
                      delay(1000);
                      recevierCh=videoCh[10].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH11_A;
                      
                      }
                  }
                else stateMachine =SM_CH11_A;
            break; 
             
            case SM_CH11_A:   //52 28  11-5
            case SM_CH11_B:
                if (videoCh[11].zap) 
                  {
                    if (stateMachine == SM_CH11_A) {zaptime= millis();stateMachine =SM_CH11_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(11, 5, 20, ch11_on, ch5_on , chx_on );  
                      delay(1000);
                      recevierCh=videoCh[11].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH12_A;
                      
                    }
                  }
                else stateMachine =SM_CH12_A;
            break;     

            case SM_CH12_A:   //53 29   12-6
            case SM_CH12_B:
                if (videoCh[12].zap) 
                  {
                    if (stateMachine == SM_CH12_A) {zaptime= millis();stateMachine =SM_CH12_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(12, 6, 20, ch12_on, ch6_on , chx_on );  
                      delay(1000);
                      recevierCh=videoCh[12].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH13_A;
                      
                    }
                  }
                else stateMachine =SM_CH13_A;
            break;  
            
            case SM_CH13_A:   // 62 51  13-10
            case SM_CH13_B:
                if (videoCh[13].zap) 
                  {
                    if (stateMachine == SM_CH13_A) {zaptime= millis();stateMachine =SM_CH13_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(13, 10, 20, ch13_on, ch10_on, chx_on  );  
                      delay(1000);                      
                      recevierCh=videoCh[13].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH14_A;
                    }
                  }
                else stateMachine =SM_CH14_A;
            break;  

            case SM_CH14_A:   //63 68  14-19
            case SM_CH14_B:
                if (videoCh[14].zap) 
                  {
                    if (stateMachine == SM_CH14_A) {zaptime= millis();stateMachine =SM_CH14_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(14, 19, 20, ch14_on, ch19_on, chx_on  );  
                      delay(1000);
                      recevierCh=videoCh[14].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH15_A;
                    }
                  }
                else stateMachine =SM_CH15_A;
            break;  
            
            case SM_CH15_A:   //64 49  15-8
            case SM_CH15_B:
                if (videoCh[15].zap) 
                  {
                    if (stateMachine == SM_CH15_A) {zaptime= millis();stateMachine =SM_CH15_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(15, 8, 20, ch15_on, ch8_on , chx_on );  
                      delay(1000);
                      recevierCh=videoCh[15].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH16_A;
                      
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH16_A;}
            break;                         

            case SM_CH16_A:   //65 26  16-3
            case SM_CH16_B:
                if (videoCh[16].zap) 
                  {
                    if (stateMachine == SM_CH16_A) {zaptime= millis();stateMachine =SM_CH16_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(16, 3, 20, ch16_on, ch3_on , chx_on );  
                      delay(1000);                      
                      recevierCh=videoCh[16].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH17_A;
                      if (videoCh[16].mux && videoCh[5].zap)remoteControl(16); //48 and 22 off 22
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH17_A;}
            break;                         

            case SM_CH17_A:   //66 27 50  17-4-9
            case SM_CH17_B:
                if (videoCh[17].zap) 
                  {
                    if (stateMachine == SM_CH17_A) {zaptime= millis();stateMachine =SM_CH17_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(17, 9, 4 , ch17_on, ch9_on, ch4_on  );  
                      delay(1000);                     
                      recevierCh=videoCh[17].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH18_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH18_A;}
            break;                         


            case SM_CH18_A:   //67-25-48  18 - 2 - 7
            case SM_CH18_B:
                if (videoCh[18].zap) 
                  {
                    if (stateMachine == SM_CH18_A) {zaptime= millis();stateMachine =SM_CH18_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(18, 7, 2, ch18_on, ch7_on , ch2_on  );  
                      delay(1000);                      
                      recevierCh=videoCh[18].id;
                      receiverAvByCh ( recevierCh);
                      stateMachine =SM_CH19_A;
                     }
                  }
                else {zaptime= millis();stateMachine =SM_CH19_A;}
            break;                         

            case SM_CH19_A: //68  63  19-14
            case SM_CH19_B:
                if (videoCh[19].zap) 
                  {
                    if (stateMachine == SM_CH19_A) {zaptime= millis();stateMachine =SM_CH19_B;}
                    if (millis() - zaptime > zapTimer) 
                    {
                      Switching(19, 14, 20 , ch19_on, ch14_on, chx_on  );  
                      delay(1000);                       
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


void Switching(int id1, int id2, int id3, bool chOnA, bool chOnB , bool chOnC)
{

  if (videoCh[id2].zap && (!videoCh[id3].zap ))
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

  if (videoCh[id3].zap && (!videoCh[id2].zap ))
  {
    if (videoCh[id1].mux && (!(videoCh[id3].mux) ))remoteControl(id1); //switch 1 10
    else if (videoCh[id3].mux && (!(videoCh[id1].mux) ))remoteControl(id3); //switch 10 62
    else if (videoCh[id3].mux && videoCh[id1].mux )
    {

    if ( chOnA == true && chOnC  == false)
      {
        remoteControl(id3); //switch 10 62  10 ON
        delay(1000);
        remoteControl(id1); //switch 10 62   62 OFF
        chOnA = false;
        chOnC  = true;
      }
    else if ( chOnC  == true && chOnA == false)
      {
        remoteControl(id1); //switch 10 62
        delay(1000);
        remoteControl(id3); //switch 10 62
        chOnA = true;
        chOnC  = false;
      }
    else
      {
        remoteControl(id1); //switch 10 62
        delay(500);
        remoteControl(id3); //switch 10 62
        chOnA = true;
        chOnC  = false;
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


#ifdef CSR3   //2CH 2 Relays Active HIGH and MAIN I2C Controllers
          Wire.beginTransmission(_address);
          Wire.write(MSB );
          Wire.write(LSB );
          Wire.write(0xC2 );
          return (Wire.endTransmission() );  
#endif

}
