#include "blynk_app.h"
#include "headers.h"





#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

long  blynkAtiveTimer;
bool  blynkActive =false;

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


#ifdef MAIN_BOARD
    char auth[] = "gu_ZdcjSU6csKvO7gpJWgh8IQJrevB-f"; //MAIN BOX
#else
    char auth[] = "3SSt-ZshvBk_-gy3xw3tpx7oX0RGISH8";  //SPARE BOX
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
extern bool sendTime_7500ms;
extern int _days  ;
extern int _hours;
extern int _minutes ;
extern int _seconds ;

void printDigits(byte digits){
 // utility function for digital clock display: prints colon and leading 0
 Serial.print(":");
 if(digits < 10)
   Serial.print('0');
 Serial.print(digits,DEC);  
}

void counterTime(){
long timeNow = millis();
 
_days =days = timeNow / day ;                                //number of days
_hours = hours = (timeNow % day) / hour;                       //the remainder from days division (in milliseconds) divided by hours, this gives the full hours
_minutes = minutes = ((timeNow % day) % hour) / minute ;         //and so on...
_seconds = seconds = (((timeNow % day) % hour) % minute) / second;

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

void sendTimeToBlynk_7500ms(){
  Serial.println("\tLook, no Blynk  block.");
  if(wifiMulti.run()== 3){
    Serial.println("\tWiFi still  connected.");
    _wifiIsConnected = true;
  }
  if(Blynk.connected()){
    counterTime();
    if(!blynkActive)
    {
      Blynk.virtualWrite(V4, days);
      Blynk.virtualWrite(V20, hours);
      Blynk.virtualWrite(V21, minutes);
      Blynk.virtualWrite(V11, seconds);
      Serial.println("\tTick update to blynk.");
    }
    
    sendTime_7500ms = !sendTime_7500ms;
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

    wifiMulti.addAP("GIGACUBE_BEK", WIFI_PASSWORD);
    wifiMulti.addAP("Freebox-bek", WIFI_PASSWORD);
    wifiMulti.addAP("OPPO Reno2 Bek" , WIFI_PASSWORD);

 // if(WiFi.status() == 6){
    if(wifiMulti.run() == 6){
    Serial.println("\tWiFi not connected yet.");
  }
  
  timer.setInterval(functionInterval, sendTimeToBlynk_7500ms);// run some function at intervals per functionInterval
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
  
  Blynk.config(auth, blynk_server,8080);
  Blynk.connect(); 
  checkBlynk();
  ledInit();
  blynkAtiveTimer     = millis();
}

BLYNK_WRITE(V0)  //freq
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_FREQ_V0;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V1) //rc433
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RC_CH_NR_V1;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V2) // receiver ch
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_VIDEO_CH_V2;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V3) // ROOM_1_TO_5
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROOM_ID_1_TO_5_V3;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V7)   // ota
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_OTA_GSM_V7;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V8)   //boot
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_REBOOT_V8;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V9) // Room Nr
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_VIDEO_CH_V2;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V10)  //Send to blynk
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_STREAMING_WEB_DDNS_V10;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V14) //rc315
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_T315_CH_NR_V14;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V15)   //NETGEER 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_NETGEER_V15;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V16) // ROOM_6_TO_10
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROOM_ID_6_TO_10_V16;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V17) // ROOM_11_TO_15
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROOM_ID_11_TO_15_V17;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V18) // ROOM_16_TO_20
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROOM_ID_16_TO_20_V18;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V19) // ROOM_AV RC
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROOM_AV_RC_V19;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V23)   //router reset Timer
{
     _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROUTER_RESET_TIMER_V23;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V25) // ROOM_21_25
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROOM_ID_21_25_V25;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V26)   //RESET FREQ
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RESET_FREQ_V26;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V27)   //DVR ON OFF
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_DVR_ON_OFF_V27;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V30)   //209 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID1;
    eventdata = Q_EVENT_209_SEL_V30;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V31)   //216 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_216_SEL_V31;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V32)   //202 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_202_SEL_V32;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V33)   //210 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_210_SEL_V33;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V34)   //204 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_204_SEL_V34;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V35)   //219 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_219_SEL_V35;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V36)   //207 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_207_SEL_V36;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V37)   //214 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_214_SEL_V37;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V38)   //208 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_208_SEL_V38;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V39)   //215
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_215_SEL_V39;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V40)   //217 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_217_SEL_V40;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V71)   //Zapping On Off Switch
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_V71;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V72)   //Zapping Timer
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_TIMER_V72;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V81)   //Zapping ch1
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL1_V81;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V82)   //Zapping ch2
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL2_V82;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V83)   //Zapping ch3
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL3_V83;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V84)   //Zapping ch4
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL4_V84;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V85)   //Zapping ch5
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL5_V85;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V86)   //Zapping ch6
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL6_V86;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V87)   //Zapping ch7
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL7_V87;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V88)   //Zapping ch8
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL8_V88;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V89)   //Zapping ch9
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL9_V89;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V90)   //CH +
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_AV_CH_PLUS_V90;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V91)   //CH -
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_AV_CH_MINUS_V91;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V92)   //FR -
{
     _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_AV_FR_MINUS_V92;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V93)   //FR +
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_AV_FR_PLUS_V93;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V94)   //Zapping ch10
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL10_V94;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V95)   //Zapping ch11
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL11_V95;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V96)   //Zapping ch12
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL12_V96;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V97)   //Zapping ch14
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL13_V97;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V98)  //pulse
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RC_PULSE_V98;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V100)  //wifi ide
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_WIFI_IDE_V100;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V101)  //repetion
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RC_REPETION_V101;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V102)  //sleep timer
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SLEEP_TIMER_V102;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V104)  //wifi WEB
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_WIFI_WEB_V104;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V105)  //wifi ota github
{
    _blynkEvent = true;
    _blynkData=param.asInt();
     eventdata = Q_EVENT_OTA_GITHUB_V105;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V106)   //Zapping ch14
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL14_V106;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V107)   //Zapping ch15
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID8;
    eventdata = Q_EVENT_ZAP_CHANNEL15_V107;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
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
      blynkActive = true; 
      blynkAtiveTimer     = millis();
      return true;
    }  
    else return false;
 
}

void blynk::blynkRunTimer()
{
  timer.run();
  if ( (  (millis() - blynkAtiveTimer) >=  BLYNK_ACTIVE_TIMEOUT ) && blynkActive )
    {
      blynkActive = false; blynkAtiveTimer     = millis();
   }
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


void blynk::blynkRCLed(bool _data, int cmd)
{
      if (_data==0)  
          {
            T433_LED_V6.setColor(BLYNK_RED);
            if ( (cmd >= 1) && (cmd <= 5))  Blynk.setProperty(V3, "color", BLYNK_GREEN);
            if ( (cmd >= 6) && (cmd <= 10))  Blynk.setProperty(V16, "color", BLYNK_GREEN);
            if ( (cmd >= 11) && (cmd <= 15))  Blynk.setProperty(V17, "color", BLYNK_GREEN);
            if ( (cmd >= 16) && (cmd <= 20))  Blynk.setProperty(V18, "color", BLYNK_GREEN);
            if ( (cmd >= 20) && (cmd <= 25))  Blynk.setProperty(V25, "color", BLYNK_GREEN);
          }
      else           
      {
            T433_LED_V6.setColor(BLYNK_GREEN);
            if ( (cmd >= 1) && (cmd <= 5))  Blynk.setProperty(V3, "color", BLYNK_RED);
            if ( (cmd >= 6) && (cmd <= 10))  Blynk.setProperty(V16, "color", BLYNK_RED);
            if ( (cmd >= 11) && (cmd <= 15))  Blynk.setProperty(V17, "color", BLYNK_RED);
            if ( (cmd >= 16) && (cmd <= 20))  Blynk.setProperty(V18, "color", BLYNK_RED);
            if ( (cmd >= 20) && (cmd <= 25))  Blynk.setProperty(V25, "color", BLYNK_RED);      
      }
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
 else           ZAP_LED_V80.setColor(BLYNK_GREEN);
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

void blynk::sevenSegValue(int vch )
{
 sevenSeg = vch;
 Blynk.virtualWrite(V2, vch);
 Blynk.virtualWrite(V9, vch);
}


void blynk::sendPulseRepetetion(int pulse, int repetetion)
{
  Blynk.virtualWrite(V98, pulse);
  Blynk.virtualWrite(V101, repetetion);
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
 Blynk.virtualWrite(V19, _index);
 Blynk.virtualWrite(V99, _index);
  if (_index ==1) {Blynk.setProperty(V19, "color", BLYNK_GREEN);Blynk.setProperty(V99, "color", BLYNK_GREEN);}
  if (_index ==2) {Blynk.setProperty(V19, "color", BLYNK_YELLOW);Blynk.setProperty(V99, "color", BLYNK_YELLOW);}
  if (_index ==3) {Blynk.setProperty(V19, "color", BLYNK_RED);Blynk.setProperty(V99, "color", BLYNK_RED);}
   
}

void blynk::visualActiveRoom(int id, bool zap)
{
  if ( (id >= 1) && (id <= 5)) 
    { 
      if (zap) Blynk.virtualWrite(V3, id);
      Blynk.setProperty(V3, "color", BLYNK_GRAY);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
    }
  if ( (id >= 6) && (id <= 10)) 
    { 
      if (zap)Blynk.virtualWrite(V16, id-5);
      Blynk.setProperty(V16, "color", BLYNK_GRAY);
      Blynk.setProperty(V3, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
   }
  if ( (id >= 11) && (id <= 15)) 
    { 
      if (zap)Blynk.virtualWrite(V17, id-10);
      Blynk.setProperty(V17, "color", BLYNK_GRAY);
      Blynk.setProperty(V3, "color", BLYNK_BLACK);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
   }
  if ( (id >= 16) && (id <= 20)) 
    { 
      if (zap)Blynk.virtualWrite(V18, id-15);
      Blynk.setProperty(V18, "color", BLYNK_GRAY);
      Blynk.setProperty(V3, "color", BLYNK_BLACK);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
   }
  if ( (id >= 21) && (id <= 25)) 
    { 
     if (zap) Blynk.virtualWrite(V25, id-20);
      Blynk.setProperty(V25, "color", BLYNK_GRAY);
      Blynk.setProperty(V3, "color",  BLYNK_BLACK);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
   }
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
  if (stream == DDNS) Blynk.setProperty(V22,"url", "rtsp://sdp:basma28112018@bouy.ddns.net:5001/ch01/0");
  else if (stream == WEB) Blynk.setProperty(V22, "url","rtsp://sdp:basma28112018@192.168.1.94:554/ch01/0");
}

void blynk::sendVersion(String ver)
{
  Blynk.virtualWrite(V24, ver);
}
