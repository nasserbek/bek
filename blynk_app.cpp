#include "blynk_app.h"
#include "headers.h"





#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

int _t433ChNumber, _t315ChNumber,_blynkfreqValue,_sevenSeg;
int _otaBlynk=0;
int _bootBlynk=0;
int _fbonBlynk =0;
int _smsBlynk=0;
bool _blynkEvent = true;
int  _blynkData=0;
int  _blynkEventID =0;
int _tempoVar;
bool _wifiIsConnected = false;
bool _blynkIsConnected = false;
bool firstConnect= false;

extern EventGroupHandle_t g_event_group;
extern QueueHandle_t g_event_queue_handle;

WidgetLED ACK_LED_V5(V5);   //Ack
WidgetLED T433_LED_V6(V6);   //T433 St
WidgetLED T315_LED_V13(V13);  //T315 St
WidgetLED SMS_LED_V12(V12);  //sms
WidgetLED ZAP_LED_V80(V80);  //Zap Status


#ifdef BEK
    char auth[] = "D4AU1HexWcErQ9vtpkP_EgocpnoArZKC"; 
#else
    char auth[] = "ya1T2eipkMhB3NvyLeAyRVRHqPAUXUG-";  //BEK2
#endif

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;
char blynk_server[] = BLYNK_SERVER;

BlynkTimer timer;
unsigned int myServerTimeout  =  3500;  //  3.5s server connection timeout (SCT)
unsigned int myWiFiTimeout    =  3200;  //  3.2s WiFi connection timeout   (WCT)
unsigned int functionInterval =  7500;  //  7.5s function call frequency   (FCF)
unsigned int blynkInterval    = 25000;  // 25.0s check server frequency    (CSF)



long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;
int eventdata;

void printDigits(byte digits){
 // utility function for digital clock display: prints colon and leading 0
 Serial.print(":");
 if(digits < 10)
   Serial.print('0');
 Serial.print(digits,DEC);  
}

void counterTime(){
long timeNow = millis();
 
days = timeNow / day ;                                //number of days
hours = (timeNow % day) / hour;                       //the remainder from days division (in milliseconds) divided by hours, this gives the full hours
minutes = ((timeNow % day) % hour) / minute ;         //and so on...
seconds = (((timeNow % day) % hour) % minute) / second;

 // digital clock display of current time
 Serial.print(days,DEC);  
 printDigits(hours);  
 printDigits(minutes);
 printDigits(seconds);
 Serial.println();  
 
}





void ledInit(void)
{
  ACK_LED_V5.on(); //Enable colours for Ack Led
  T433_LED_V6.on(); //Enable colours for T433 St Led
  T315_LED_V13.on(); //Enable colours for T315 St Led
  SMS_LED_V12.on(); //Enable colours for firebase
  ZAP_LED_V80.on(); //Enable colours for Zapping
}

void myfunction(){
  Serial.println("\tLook, no Blynk  block.");
  if(wifiMulti.run()== 3){
    Serial.println("\tWiFi still  connected.");
    _wifiIsConnected = true;
  }
  if(Blynk.connected()){
    counterTime();
    Blynk.virtualWrite(V4, days);
    Blynk.virtualWrite(V20, hours);
    Blynk.virtualWrite(V21, minutes);
    Blynk.virtualWrite(V11, seconds);
    Serial.println("\tTick update to blynk.");
 // if(firstConnect== false) {firstConnect= true;Blynk.syncAll();}
    
    _blynkIsConnected = true;
  }
}

void checkBlynk() {
  if (wifiMulti.run() == WL_CONNECTED)  
  {
    unsigned long startConnecting = millis(); 
    _blynkIsConnected = true;   
    _wifiIsConnected = true;
    while(!Blynk.connected()){
      _blynkIsConnected = false;
      Blynk.connect();  
      if(millis() > startConnecting + myServerTimeout){
        Serial.print("Unable to connect to server. ");
        break;
      }
    }
  }
  if (wifiMulti.run() != 3) {
    Serial.print("\tNo WiFi. ");
    _wifiIsConnected = false;
    _blynkIsConnected = false;
  } 
  Serial.printf("\tChecking again in %is.\n", blynkInterval / 1000);
  Serial.println(); 
}


void blynk::init() 
{
  Serial.println();

    wifiMulti.addAP("HUAWEI-BOUY", WIFI_PASSWORD);
    wifiMulti.addAP("Freebox_bek" , WIFI_PASSWORD);
    wifiMulti.addAP("SFR_A870", WIFI_PASSWORD);
    wifiMulti.addAP("OPPO Reno2 Bek" , WIFI_PASSWORD);

 // if(WiFi.status() == 6){
    if(wifiMulti.run() == 6){
    Serial.println("\tWiFi not connected yet.");
  }
  
  timer.setInterval(functionInterval, myfunction);// run some function at intervals per functionInterval
  timer.setInterval(blynkInterval, checkBlynk);   // check connection to server per blynkInterval
  unsigned long startWiFi = millis();
//  WiFi.begin(ssid, pass);
  while (wifiMulti.run() != WL_CONNECTED){
    delay(500);
    if(millis() > startWiFi + myWiFiTimeout){
      Serial.println("\tCheck the WiFi router. ");
      break;
    }       
  }
  
  if(wifiMulti.run()== 3){
    Serial.println("");
    Serial.println("\tWiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    _wifiIsConnected = true;
  }
  
  Blynk.config(auth, blynk_server);
  Blynk.connect(); 
  checkBlynk();
  ledInit();
}

BLYNK_WRITE(V0)  //freq
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID = FB_FREQ_ID;
    eventdata = Q_EVENT_FREQ_V0;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
  
  DEBUG_PRINT("V0 Freq: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}


BLYNK_WRITE(V1) //rc433
{
  _t433ChNumber = param.asInt(); // assigning incoming value from pin V1 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_T433_CH_NR_ID;
    eventdata = Q_EVENT_T433_CH_NR_V1;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V1 T433: ");
  DEBUG_PRINTLN(_t433ChNumber);
}

BLYNK_WRITE(V2) // receiver ch
{
  _sevenSeg= param.asInt(); // assigning incoming value from pin V2 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_7SEG_ID;
    eventdata = Q_EVENT_AV_7SEG_V2;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V2 Seven Segments: ");
  DEBUG_PRINTLN(_sevenSeg);
}

BLYNK_WRITE(V3) // ROOM_201_TO_205
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_201_TO_205;
    eventdata = Q_EVENT_ROOM_201_TO_205_V3;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V3 ROOM_201_TO_205: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V3, "color", BLYNK_BLUE);
}

BLYNK_WRITE(V7)   // ota
{
  _otaBlynk = param.asInt(); // assigning incoming value from pin V7 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_OTA_ID;
    eventdata = Q_EVENT_OTA_V7;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V7 Ota Gsm: ");
  DEBUG_PRINTLN(_otaBlynk ? F("On") : F("Off"));
}

BLYNK_WRITE(V8)   //boot
{
  _bootBlynk = param.asInt(); // assigning incoming value from pin V7 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_RESET_ID;
    eventdata = Q_EVENT_RESET_V8;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V8 reboot: ");
  DEBUG_PRINTLN(_bootBlynk ? F("On") : F("Off"));
}

BLYNK_WRITE(V10)  //Send to blynk
{
  _bootBlynk = param.asInt(); // assigning incoming value from pin V7 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_STREAMING_WEB_DDNS_V10;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V10 STREAMING: ");
  DEBUG_PRINTLN(_bootBlynk ? F("On") : F("Off"));
}



BLYNK_WRITE(V14) //rc315
{
  _t315ChNumber = param.asInt(); // assigning incoming value from pin V14 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_T315_CH_NR_ID;
    eventdata = Q_EVENT_T315_CH_NR_V14;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V14 T315: ");
  DEBUG_PRINTLN(_t433ChNumber);
}



BLYNK_WRITE(V15)   //NETGEER 
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_NETGEER_ID ;
    eventdata = Q_EVENT_NETGEER_V15;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V15 NETGEER: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V16) // ROOM_206_TO_210
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_206_TO_210;
    eventdata = Q_EVENT_ROOM_206_TO_210_V16;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V16 ROOM_206_TO_210: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V16, "color", BLYNK_BLUE);
}


BLYNK_WRITE(V17) // ROOM_211_TO_215
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_211_TO_215;
    eventdata = Q_EVENT_ROOM_211_TO_215_V17;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V17 ROOM_211_TO_215: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V17, "color", BLYNK_BLUE);
}

BLYNK_WRITE(V18) // ROOM_216_TO_220
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_216_TO_220;
    eventdata = Q_EVENT_ROOM_216_TO_220_V18;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V18 ROOM_216_TO_220: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V18, "color", BLYNK_BLUE);
}


BLYNK_WRITE(V19) // ROOM_AV RC
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_AV_RC;
    eventdata = Q_EVENT_ROOM_AV_RC_V19;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V19 ROOM_AV_RC: ");
  DEBUG_PRINTLN(_sevenSeg);
  if (_blynkData ==1) Blynk.setProperty(V19, "color", BLYNK_GREEN);
  if (_blynkData ==2) Blynk.setProperty(V19, "color", BLYNK_YELLOW);
  if (_blynkData ==3) Blynk.setProperty(V19, "color", BLYNK_RED);
}

BLYNK_WRITE(V23)   //router reset Timer
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROUTER_RESET_TIMER_V23;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V23 router reset Timer: ");
  DEBUG_PRINTLN(_tempoVar );
}


BLYNK_WRITE(V71)   //Zapping On Off Switch
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_ID;
    eventdata = Q_EVENT_ZAP_V71;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V71 Zap: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V71, "color", BLYNK_BLUE);
  else Blynk.setProperty(V71, "color", BLYNK_RED); 
}


BLYNK_WRITE(V72)   //Zapping Timer
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_TIMER_ID;
    eventdata = Q_EVENT_ZAP_TIMER_V72;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V712 Zap Timer: ");
  DEBUG_PRINTLN(_tempoVar );
}

BLYNK_WRITE(V81)   //Zapping ch1
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID1;
    eventdata = Q_EVENT_ZAP_CHANNEL1_V81;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V81 Zap ch1: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V81, "color", BLYNK_BLUE);
  else Blynk.setProperty(V81, "color", BLYNK_RED);
}

BLYNK_WRITE(V82)   //Zapping ch2
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID2;
    eventdata = Q_EVENT_ZAP_CHANNEL2_V82;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V82 Zap ch2: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V82, "color", BLYNK_BLUE);
  else Blynk.setProperty(V82, "color", BLYNK_RED);
}
BLYNK_WRITE(V83)   //Zapping ch3
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID3;
    eventdata = Q_EVENT_ZAP_CHANNEL3_V83;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V83 Zap ch3: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V83, "color", BLYNK_BLUE);
  else Blynk.setProperty(V83, "color", BLYNK_RED);  
}
BLYNK_WRITE(V84)   //Zapping ch4
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID4;
    eventdata = Q_EVENT_ZAP_CHANNEL4_V84;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V84 Zap ch4: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V84, "color", BLYNK_BLUE);
  else Blynk.setProperty(V84, "color", BLYNK_RED);  
}
BLYNK_WRITE(V85)   //Zapping ch5
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID5;
    eventdata = Q_EVENT_ZAP_CHANNEL5_V85;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V85 Zap ch5: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V85, "color", BLYNK_BLUE);
  else Blynk.setProperty(V85, "color", BLYNK_RED);  
}
BLYNK_WRITE(V86)   //Zapping ch6
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID6;
    eventdata = Q_EVENT_ZAP_CHANNEL6_V86;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V86 Zap ch6: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V86, "color", BLYNK_BLUE);
  else Blynk.setProperty(V86, "color", BLYNK_RED);
}
BLYNK_WRITE(V87)   //Zapping ch7
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID7;
    eventdata = Q_EVENT_ZAP_CHANNEL7_V87;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V87 Zap ch7: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V87, "color", BLYNK_BLUE);
  else Blynk.setProperty(V87, "color", BLYNK_RED);
}
BLYNK_WRITE(V88)   //Zapping ch8
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID8;
    eventdata = Q_EVENT_ZAP_CHANNEL8_V88;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V88 Zap ch8: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V88, "color", BLYNK_BLUE);
  else Blynk.setProperty(V88, "color", BLYNK_RED);  
}


BLYNK_WRITE(V89)   //Zapping ch9
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID8;
    eventdata = Q_EVENT_ZAP_CHANNEL9_V89;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V89 Zap ch9: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V89, "color", BLYNK_BLUE);
  else Blynk.setProperty(V89, "color", BLYNK_RED);  
}
BLYNK_WRITE(V90)   //CH +
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_CH_PLUS_ID  ;
    eventdata = Q_EVENT_AV_CH_PLUS_V90;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("CH +: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V91)   //CH -
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_CH_MINUS_ID  ;
    eventdata = Q_EVENT_AV_CH_MINUS_V91;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("CH -: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V92)   //FR -
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_FR_MINUS_ID  ;
    eventdata = Q_EVENT_AV_FR_MINUS_V92;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("FR -: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V93)   //FR +
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_FR_PLUS_ID  ;
    eventdata = Q_EVENT_AV_FR_PLUS_V93;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("FR +: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V94)   //Zapping ch10
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID8;
    eventdata = Q_EVENT_ZAP_CHANNEL10_V94;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V94 Zap ch10: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V94, "color", BLYNK_BLUE);
  else Blynk.setProperty(V94, "color", BLYNK_RED);  
}

BLYNK_WRITE(V95)   //Zapping ch11
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID8;
    eventdata = Q_EVENT_ZAP_CHANNEL11_V95;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("V95 Zap ch11: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V95, "color", BLYNK_BLUE);
  else Blynk.setProperty(V95, "color", BLYNK_RED);  
}

BLYNK_WRITE(V98)  //pulse
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_RC_PULSE_ID;
    eventdata = Q_EVENT_RC_PULSE_V98;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("RC PULSE: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}


BLYNK_WRITE(V100)  //wifi ide
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_WIFI_IDE_ID ;
    eventdata = Q_EVENT_WIFI_IDE_V100;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("Wifi IDE: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}


BLYNK_WRITE(V101)  //repetion
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_RC_REPETION_ID;
    eventdata = Q_EVENT_RC_REPETION_V101;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("RC Repetion: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}


BLYNK_WRITE(V102)  //sleep timer
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_SLEEP_TIMER_ID;
    eventdata = Q_EVENT_SLEEP_TIMER_V102;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("Sleep Timer: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}

BLYNK_WRITE(V104)  //wifi WEB
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_WIFI_WEB_ID ;
    eventdata = Q_EVENT_WIFI_WEB_V104;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("Wifi WEB: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}


BLYNK_WRITE(V105)  //wifi ota github
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_WIFI_OTA_ID ;
    eventdata = Q_EVENT_WIFI_OTA_V105;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    
  DEBUG_PRINT("Wifi Ota Github: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}


blynk::blynk(void) 
{

}





void blynk::notifierDebug(String subject, String body)
{
  Blynk.notify(String(subject +"**"+ body) );
}


bool blynk::wifiConnect()
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if (WiFi.status()  == WL_CONNECTED ){DEBUG_PRINTLN("Wifi connected");return true; }
  
    long timeout = millis();
    long wifiReconnect = millis();
    
    while ( WiFi.status()  != WL_CONNECTED ) 
      {
        if (millis() - wifiReconnect > WIFI_RECONNECT_TIMER ) 
          {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            wifiReconnect = millis();
            DEBUG_PRINTLN("Wifi Reconnect");
          }
        if (millis() - timeout > WIFI_SURVILANCE_TIMER){DEBUG_PRINTLN("Wifi failed"); return false; }
      }
    return true; 
}


bool blynk::getData()
{
    if (_blynkEvent)
    {
      blynkData=_blynkData;
      blynkEventID =_blynkEventID;
      _blynkEvent = false;
      return true;
    }  
    else return false;
 
}

void blynk::blynkRunTimer()
{
  timer.run();
}

void blynk::blynkRun()
{
  Blynk.run(); 
}

bool blynk::blynkConnected()
{
  return( Blynk.connected() ); 
}

void blynk::blynkConnect()
{
  Blynk.connect();
}


void blynk::blynkAckLed(bool _data)
{
      if (_data==1)  ACK_LED_V5.setColor(BLYNK_RED);
      else           ACK_LED_V5.setColor(BLYNK_GREEN);
}


void blynk::blynkRCLed(bool _data)
{
      if (_data==0)  T433_LED_V6.setColor(BLYNK_RED);
      else           T433_LED_V6.setColor(BLYNK_GREEN);
}

void blynk::blynkRCLed315(bool _data)
{
      if (_data==0)  T315_LED_V13.setColor(BLYNK_RED);
      else           T315_LED_V13.setColor(BLYNK_GREEN);
}

void blynk::blynkSmsLed(bool _data)
{
 if (_data==0)  SMS_LED_V12.setColor(BLYNK_RED);
 else           SMS_LED_V12.setColor(BLYNK_GREEN);
}

void blynk::zapLed(bool _data)
{
 if (_data==0)  ZAP_LED_V80.setColor(BLYNK_RED);
 else           ZAP_LED_V80.setColor(BLYNK_BLUE);
}

void blynk::sendToBlynkLed(bool _data)
{
}

void blynk::blynkFirebaseLed(bool _data)
{
}

void blynk::sendAlive(int _data)
{
}

/***************************************************/
void blynk::resetT433Cmd(int cmd)
{
 t433ChNumber = cmd;
 Blynk.virtualWrite(V1, cmd);
}

void blynk::resetT315Cmd(int cmd)
{
 t315ChNumber = cmd;
 Blynk.virtualWrite(V14, cmd);
}

void blynk::sevenSegValue(int freq )
{
 sevenSeg = freq;
 Blynk.virtualWrite(V2, freq);
}


void blynk::frequencyValue(int freq )
{
  frequency = freq;
  Blynk.virtualWrite(V0, freq);
}

void blynk::sendRsss(int _rsss)
{
}

void blynk::sendAvRxIndex(int _index)
{
 Blynk.virtualWrite(V99, _index);
}

bool blynk::blynkStatus(void)
{
 return  _blynkIsConnected;
}

bool blynk::wifiStatus(void)
{
 return  _wifiIsConnected;
}

void blynk::streamSelect(bool stream)
{
  if (stream == DDNS) Blynk.setProperty(V22,"url", "rtsp://creil:basma28112018@bouy.ddns.net:5001/ch01/0");
  else if (stream == WEB) Blynk.setProperty(V22, "url","rtsp://creil:basma28112018@192.168.1.94:554/ch01/0");
}
