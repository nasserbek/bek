#include "blynk_app.h"
#include "headers.h"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiMulti.h>
extern void dvrOnOff (bool cmd);
extern void SendLiveLed(void);
extern void rebootSw(void);
extern bool DvrChOn;
extern int hmi;
extern int activeBoard ;
extern int selectedBoard;
extern void apiSend(int board, String virtualPin, int value);
unsigned long startConnecting = millis();
extern bool liveLed ;
extern bool liveLedUpdate;
extern bool internetLossUpdate;
extern bool InternetLoss;
extern bool sendVerWifi;
extern bool blynkConnected;
extern bool awsConnected;
extern void awsTerminal(bool aws, String str) ;
extern int LiveSec;
extern int LiveMin;
extern int LiveHour;
extern bool RC_Api;
extern bool zapScanOnly;
extern bool zapSetup;
extern bool zapOnOff;

WiFiMulti wifiMulti;
BlynkTimer timer;


blynk::blynk(void) 
{

}

#include <Update.h>
#include <HTTPClient.h>
void reboot()
{
#if defined(ARDUINO_ARCH_MEGAAVR)
  wdt_enable(WDT_PERIOD_8CLK_gc);
#elif defined(__AVR__)
  wdt_enable(WDTO_15MS);
#elif defined(__arm__)
  NVIC_SystemReset();
#elif defined(ESP8266) || defined(ESP32)
  ESP.restart();
#else
  #error "MCU reset procedure not implemented"
#endif
  for (;;) {}
}

BLYNK_WRITE(InternalPinOTA) {
  Blynk.disconnect();
  String overTheAirURL = param.asString();
  HTTPClient http;
  http.begin(overTheAirURL);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {Blynk.connect(); return;}
  int contentLength = http.getSize();
  if (contentLength <= 0) {Blynk.connect();return; }
  bool canBegin = Update.begin(contentLength);
  if (!canBegin) { Blynk.connect();return;}
  Client& client = http.getStream();
  int written = Update.writeStream(client);
  if (written != contentLength) {Blynk.connect();return;}
  if (!Update.end()) {Blynk.connect();return;}
  if (!Update.isFinished()) {Blynk.connect();return;}
reboot();
}

extern int selected_room ;
extern int selected_Rx;
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

bool ledStatus = false;
extern bool queuValidData;
//#define BLYNK_GREEN     "#23C48E"
//#define BLYNK_BLUE      "#04C0F8"
//#define BLYNK_YELLOW    "#ED9D00"
//#define BLYNK_RED       "#D3435C"
//#define BLYNK_DARK_BLUE "#5F7CD8"



WidgetLED I2C_LED_V13(V13);  //I2C ACK
WidgetLED LIVE_LED_V121(V121);  //LIVE
WidgetMap myMap(V12);
WidgetTerminal terminal(V102);

unsigned int BlynkServerTimeout  =  2000;  //  2s server connection timeout (SCT)
unsigned int LiveUpdateInterval =  15000;  //  15s function call frequency   (FCF)
unsigned int blynkIntervalInterval    = 10000;  // 10 s check Blyk and Wifi


bool blynk2 = true;
long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;
int eventdata;


void ledInit(void)
{  
  I2C_LED_V13.on(); 
  LIVE_LED_V121.on();
}

// V121 LED Widget is blinking
void blinkLedWidget()
{
  if(!queuValidData)
  {
  if (ledStatus) {
    LIVE_LED_V121.setColor(BLYNK_RED);
    Serial.println("LED on V121: red");
    ledStatus = false;
  } else {
    LIVE_LED_V121.setColor(BLYNK_GREEN);
    Serial.println("LED on V121: green");
    ledStatus = true;
  }
  }
}
//void SendLiveLed()
//  {
//    if (liveLed)  liveLed = false; 
//    else liveLed = true;
//    liveLedUpdate =false;
//  }

void checkBlynk() {
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)  
  {
    unsigned long startConnecting = millis(); 
    _blynkIsConnected = true;   
    _wifiIsConnected = true;

     while(!Blynk.connected()){
      Serial.println("Blynk Disconnected");
       Blynk.connect(BlynkServerTimeout);  
       if(millis() > startConnecting + BlynkServerTimeout){
        _blynkIsConnected = false;
        Serial.println("Unable to connect to server. ");
        break;
      }
    } 
  }
  else 
    {
      _wifiIsConnected = false;
      _blynkIsConnected = false; 
    } 
  DEBUG_PRINT("WIFI: ");DEBUG_PRINTLN( _wifiIsConnected ? F("Connected") : F("Not Connected"));
  DEBUG_PRINT("BLYNK: ");DEBUG_PRINTLN( _blynkIsConnected ? F("Connected") : F("Not Connected"));  
  Serial.printf("\tChecking again Blynk connected in %is.\n", blynkIntervalInterval / 1000);
  Serial.println(".");
  blynkConnected = _blynkIsConnected;
}

bool blynk::wifi_init() 
{
    _wifiIsConnected = false;

       wifiMulti.addAP(WIFI_SSID_METEOR_FREE, WIFI_PASSWORD_METEOR);
       wifiMulti.addAP(WIFI_SSID_FREE , WIFI_PASSWORD);
       wifiMulti.addAP(WIFI_SSID_ZFLIP , WIFI_PASSWORD);
       wifiMulti.addAP(WIFI_SSID_XIAOMI , WIFI_PASSWORD);
       wifiMulti.addAP(WIFI_SSID_METEOR_BU, WIFI_PASSWORD_METEOR);
     
    Serial.println("Connecting Wifi...");
    //Connecting to the strongest WiFi connection
    if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());  //print IP of the connected WiFi network
      _wifiIsConnected = true;
    }
    else  // if not WiFi not connected
    {
      _wifiIsConnected = false;
    }
    DEBUG_PRINT("WIFI: ");DEBUG_PRINTLN( _wifiIsConnected ? F("Connected") : F("Not Connected"));
    return _wifiIsConnected ;
}


bool blynk::init() 
{
   _blynkIsConnected = false;   
  timer.setInterval(LiveUpdateInterval, SendLiveLed);// run some function at intervals per LiveUpdateInterval
  timer.setInterval(blynkIntervalInterval, checkBlynk);   // check connection to server per blynkIntervalInterval
  timer.setInterval(1000L, blinkLedWidget);
  if(_wifiIsConnected)
    {
        if(WiFi.SSID() == WIFI_SSID_FREE) BLYNK_SERVER = BLYNK_SERVER_FREE_BOX;
        else if(WiFi.SSID() == WIFI_SSID_METEOR_FREE) BLYNK_SERVER = BLYNK_SERVER_FREE_METEOR;
        Blynk.config(BLYNK_AUTH_TOKEN, BLYNK_SERVER,8080); 
        Blynk.connect(BlynkServerTimeout);
        delay(1000);
        _blynkIsConnected = Blynk.connected();
      
      int index = 0;
      double lat = 43.97616119635222;
      double lon = 4.871992547377278; //43.97616119635222, 4.871992547377278
      myMap.location(index, lat, lon, "La Pontet");
     
     DEBUG_PRINT("BLYNK: ");DEBUG_PRINTLN( _blynkIsConnected ? F("Connected") : F("Not Connected"));
     blynkAtiveTimer     = millis();
     blynkActive = false;
     ledInit();     
     terminal.clear();
  }
  return _blynkIsConnected;
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

BLYNK_WRITE(V3) // catchCh 
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_REPEAT_V3;
    Serial.println(_blynkData);
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V4)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_SCAN_ACTIVE_CH_V4;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V5)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_AUTOMATIC_RC_L_R_V5;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V6)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_OTA_LOCAL_WEB_WIFI_V6;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V7)   
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_OTA_GITHUB_V7;
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
    eventdata = Q_EVENT_SELECTED_RECIEVER_V9;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V10)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_SCAN_ONLY_V10;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}


BLYNK_WRITE(V11)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_WIFI_IDE_V11;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V12)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SPARE_V12;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}



BLYNK_WRITE(V16) // // RC LOCAL REMOTE TO CSR1
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RC_REMOTE_CSR1_V16;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V17) // RC LOCAL REMOTE TO CSR3
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RC_REMOTE_CSR3_V17;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V18) // ROOM_16_TO_20
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_AWS_RECONNECT_V18;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V19) // ROOM_AV RC
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ROOM_AV_RC_V19;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V20) // RC LOCAL REMOTE TO CSR2
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RC_REMOTE_CSR2_V20;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

/*
BLYNK_WRITE(V22)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAPP_ALL_ON_V22;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
*/

BLYNK_WRITE(V25) // ROOM_21_25
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_TIMER_OFF_V25;
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
    eventdata = Q_EVENT_SPARE_V27;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V30)   //03 
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_REL1_CH_V30;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V31)   //21
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_REL2_CH_V31;    
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V32)   //27
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_REL3_CH_V32;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V33)   //50
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_REL4_CH_V33;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V34)   //20
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_ALL_ON_OFF_V34;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V35)   //52
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SHARE_VIDEO_WITH_CSR1_V35;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V36)   //53 
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SHARE_VIDEO_WITH_CSR2_V36;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V37)   //22 
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SHARE_VIDEO_WITH_CSR3_V37;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V38)   //NOT USED
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_WIFI_RSSI_V38;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V39)   //215
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RESET_ROUTER_V39;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V40)   //217 
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_AUTOMATIC_RESET_ROUTER_V40;
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

BLYNK_WRITE(V81)   //DVR NEW
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_VIDEO_ON_OFF_V81;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

//BLYNK_WRITE(V82)   //Zapping ch2
//{
//    _blynkEvent = true; 
//    _blynkData=param.asInt();
//    eventdata = Q_EVENT_LIVE_MIN_V82;
//    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
//}
//
//BLYNK_WRITE(V83)   //Zapping ch3
//{
//    _blynkEvent = true; 
//    _blynkData=param.asInt();
//    eventdata = Q_EVENT_LIVE_HOUR_V83;
//    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
//
//}

BLYNK_WRITE(V84)   //Zapping ch4
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_VIDEO_STREAMING_V84;
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
    eventdata = Q_EVENT_RM_ID_20_V100;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V101)  //repetion
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RC_REPETION_V101;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V102)  //TERMINAL
{
  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
    terminal.println("You said: 'Marco'") ;
    terminal.println("I said: 'Polo'") ;
  } 
  
  else if (String("w") == param.asStr()) 
  {
    terminal.println( WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) + "\n");
  } 

  else if (String("c") == param.asStr()) 
  {
    terminal.clear();
  }

  else {
    // Send it back
    terminal.print("You said:");
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();
  }

  // Ensure everything is sent
  terminal.flush();

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
    eventdata = Q_EVENT_ZAP_CHANNEL15_V107;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V108)   //Zapping ch16
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL16_V108;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V109)   //Zapping ch17
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL17_V109;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V110)   //Zapping ch18
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL18_V110;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V111)   //Zapping ch19
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL19_V111;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V112)   
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_10_V112;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}





/*
BLYNK_WRITE(V121)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SPARE_V121;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

*/
BLYNK_WRITE(V122)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_02_V122;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V123)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_03_V123;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V124)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_04_V124;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V125)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_05_V125;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V126)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_06_V126;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V127)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_07_V127;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V80)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_09_V80;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V21)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_10_V21;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V14)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_11_V14;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V15)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_12_V15;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V23)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_13_V23;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V103)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_14_V103;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V104)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_15_V104;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V105)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_16_V105;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V90)  
{
    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_17_V90;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V91)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_18_V91;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V92)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_19_V92;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}
BLYNK_WRITE(V93)  
{

    _blynkEvent = true; 
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_08_V93;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}


void blynk::blynkRun()
{
  Blynk.run(); 
}

//bool blynk::blynkConnected()
//{
//  return( Blynk.connected() ); 
//}

void blynk::blynkConnect()
{
  Blynk.connect();
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
      hmi =BLYNK;
      LiveSec =LiveMin =LiveHour = 0;
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







void blynk::resetSetupAndScan(bool _data)
{
Blynk.virtualWrite(V4, _data);
Blynk.virtualWrite(V10, _data);
}

void blynk::zapAutoLocalRC(bool _data)
{
  Blynk.virtualWrite(V5, _data);
}

void blynk::zapStatus(bool _data ) 
{
Blynk.virtualWrite(V71, _data);
}

void blynk::sendToBlynkLed(bool _data)
{
}

void blynk::wifiRSSI(int _data)
{
   Blynk.virtualWrite(V38, _data); 
}

void blynk::resetRemoteRC(int _data)
{
if(!zapSetup && !zapScanOnly)
{
  switch (_data)
        {
          case ESP1:
                   Blynk.virtualWrite(V16, 0); 
          break;

          case ESP2:
                   Blynk.virtualWrite(V20, 0); 
          break;
          case ESP3:
                   Blynk.virtualWrite(V17, 0); 
          break;
        }   
 }
}

void blynk::resetRemoteVideo(int _data)
{
if(!zapSetup && !zapScanOnly)
{
  switch (_data)
        {
          case ESP1:
                   Blynk.virtualWrite(V35, 0); 
          break;

          case ESP2:
                   Blynk.virtualWrite(V36, 0); 
          break;
          case ESP3:
                   Blynk.virtualWrite(V37, 0); 
          break;
        }   
 }
}

/***************************************************/
void blynk::resetT433Cmd(int cmd)
{
 t433ChNumber = cmd;
 Blynk.virtualWrite(V1, cmd);
}

void blynk::resetT315Cmd(int cmd)
{

}

void blynk::VideoActiveCh(int vch )
{
 sevenSeg = vch;
 Blynk.virtualWrite(V2, vch);
}


void blynk::sendPulseRepetetion(int pulse, int repetetion)
{
  Blynk.virtualWrite(V98, pulse);
  Blynk.virtualWrite(V101, repetetion);
}


void blynk::frequencyValue(int freq )
{
  frequency = freq;
  if(!zapSetup && !zapScanOnly) Blynk.virtualWrite(V0, freq);
}

void blynk::dvrSwitch(bool cmd)
{
  Blynk.virtualWrite(V81, cmd);
}

void blynk::sendAvRxIndex(int _index)
{
 Blynk.virtualWrite(V19, _index);
  if (_index ==1) {Blynk.setProperty(V19, "color", BLYNK_GREEN);}
  if (_index ==2) {Blynk.setProperty(V19, "color", BLYNK_YELLOW);}
  if (_index ==3) {Blynk.setProperty(V19, "color", BLYNK_RED);}
   
}

void blynk::sendBoardIndex(int _index)
{
 Blynk.virtualWrite(V12, _index);
  if (_index ==1) {Blynk.setProperty(V12, "color", BLYNK_GREEN);}
  if (_index ==2) {Blynk.setProperty(V12, "color", BLYNK_YELLOW);}
  if (_index ==3) {Blynk.setProperty(V12, "color", BLYNK_RED);}
  if (_index ==4) {Blynk.setProperty(V12, "color", BLYNK_BLUE);}
   
}

void blynk::blynkAckLed( bool _data)
{
  if (RC_Api)  I2C_LED_V13.setColor(BLYNK_YELLOW);
    else
        {
        if (_data==1)  I2C_LED_V13.setColor(BLYNK_RED);
        else           I2C_LED_V13.setColor(BLYNK_GREEN);    
        }
}



void blynk::liveLedCall(bool _data)
{
  if(!blynkActive && !zapOnOff && !zapScanOnly)
    {
      LiveSec += LiveUpdateInterval/1000;
      if (LiveSec >= 60) { LiveMin +=1;  LiveSec = 0;}
      if (LiveMin >= 60) { LiveHour +=1; LiveMin =0; }
      Blynk.virtualWrite(V83, LiveHour);
      Blynk.virtualWrite(V82, LiveMin);
      Blynk.virtualWrite(V121, LiveSec);
      if(_wifiIsConnected && LiveHour >= 2 && DvrChOn ) { terminal.println("Turninh Off Video for non activity in 2 Hours..."); dvrOnOff (false);}
    }
}

void blynk::visualActiveRoom(int id, bool zap)
{

}

bool blynk::blynkStatus(void)
{
 return  _blynkIsConnected;
}

bool blynk::wifiStatus(void)
{
 return  _wifiIsConnected;
}

void blynk::RelaySelect(int rele)
{
 Blynk.virtualWrite(V9, rele);
}

void blynk::sendVersion(String ver)
{
  Blynk.virtualWrite(V24, ver);
}

void blynk::SyncAll(void)
{
}

void blynk::blynk1(void)
{

}    

void blynk::repeatSync(bool repeat)
{
  Blynk.virtualWrite(V3, repeat);
}

void blynk::TerminalPrint (String str)
{
    if ( blynkConnected )terminal.println(str);
    else Serial.println(str); 
   
   #ifdef TEST   
       Serial.println(str);
    #endif 
    terminal.flush();  
}

void blynk::BlynkButtonColours(int lastSelectedCh, int chMode)
{
if(!zapSetup && !zapScanOnly)
{
  if(chMode == CH_MODE_0)
  {
    switch (lastSelectedCh)
        {
          case 1:
                   Blynk.virtualWrite(V112, 0); 
         break;

          case 2:
                   Blynk.virtualWrite(V122, 0); 
                 

          break;
          case 3:
                   Blynk.virtualWrite(V123, 0); 
                   

          break;
          case 4:
                   Blynk.virtualWrite(V124, 0); 
                 

          break;
          case 5:
                   Blynk.virtualWrite(V125, 0); 
                   

          break;
          case 6:
                   Blynk.virtualWrite(V126, 0); 
                   

          break;
          case 7:
                   Blynk.virtualWrite(V127, 0); 
                   

          break;
          case 8:
                   Blynk.virtualWrite(V93, 0); 
                   

          break;
          case 9:
                   Blynk.virtualWrite(V80, 0); 
                  

          break;
          case 10:
                   Blynk.virtualWrite(V21, 0); 
                   

          break;
          case 11:
                   Blynk.virtualWrite(V14, 0); 
                   

          break;
          case 12:
                   Blynk.virtualWrite(V15, 0); 
                   

          break;
          case 13:
                   Blynk.virtualWrite(V23, 0); 
                   

          break;
          case 14:
                   Blynk.virtualWrite(V103, 0); 
                 

          break;
          case 15:
                   Blynk.virtualWrite(V104, 0); 
                   

          break;
          case 16:
                   Blynk.virtualWrite(V105, 0); 
                  

          break;
          case 17:
                   Blynk.virtualWrite(V90, 0); 
                   

          break;
          case 18:
                   Blynk.virtualWrite(V91, 0); 
                   

          break;
          case 19:
                   Blynk.virtualWrite(V92, 0); 
                  

          break;                   
        }  
  }    

  if (chMode == CH_MODE_1)
  {
        switch (lastSelectedCh)
        {
          case 1:
                 Blynk.setProperty(V112, "onBackColor", BLYNK_YELLOW );
          break;

          case 2:
                    Blynk.setProperty(V122, "onBackColor", BLYNK_YELLOW ); 
          break;
          case 3:
                    Blynk.setProperty(V123, "onBackColor", BLYNK_YELLOW ); 
          break;
          case 4:
                    Blynk.setProperty(V124, "onBackColor", BLYNK_YELLOW ); 
           
          break;
          case 5:
                    Blynk.setProperty(V125, "onBackColor", BLYNK_YELLOW ); 
           
          break;
          case 6:
                    Blynk.setProperty(V126, "onBackColor", BLYNK_YELLOW ); 
           
          break;
          case 7:
                    Blynk.setProperty(V127, "onBackColor", BLYNK_YELLOW ); 
    
          break;
          case 8:
                    Blynk.setProperty(V93, "onBackColor", BLYNK_YELLOW ); 
         
          break;
          case 9:
                    Blynk.setProperty(V80, "onBackColor", BLYNK_YELLOW ); 
          
          break;
          case 10:
                    Blynk.setProperty(V21, "onBackColor", BLYNK_YELLOW ); 
           
          break;
          case 11:
                    Blynk.setProperty(V14, "onBackColor", BLYNK_YELLOW ); 
           
          break;
          case 12:
                    Blynk.setProperty(V15, "onBackColor", BLYNK_YELLOW ); 
          
          break;
          case 13:
                    Blynk.setProperty(V23, "onBackColor", BLYNK_YELLOW ); 
       
          break;
          case 14:
                    Blynk.setProperty(V103, "onBackColor", BLYNK_YELLOW ); 
       
          break;
          case 15:
                    Blynk.setProperty(V104, "onBackColor", BLYNK_YELLOW ); 
        
          break;
          case 16:
                    Blynk.setProperty(V105, "onBackColor", BLYNK_YELLOW ); 
       
          break;
          case 17:
                    Blynk.setProperty(V90, "onBackColor", BLYNK_YELLOW ); 
          
          break;
          case 18:
                    Blynk.setProperty(V91, "onBackColor", BLYNK_YELLOW ); 
          
          break;
          case 19:
                    Blynk.setProperty(V92, "onBackColor", BLYNK_YELLOW ); 
          
          break;                   
        }  
  }

   if (chMode == CH_MODE_2)
  {
        switch (lastSelectedCh)
        {
          case 1:

                 Blynk.setProperty(V112, "onBackColor", BLYNK_GREEN );
          break;

          case 2:
                    Blynk.setProperty(V122, "onBackColor", BLYNK_GREEN ); 
           
          break;
          case 3:
                    Blynk.setProperty(V123, "onBackColor", BLYNK_GREEN ); 
          
          break;
          case 4:
                    Blynk.setProperty(V124, "onBackColor", BLYNK_GREEN ); 
          
          break;
          case 5:
                    Blynk.setProperty(V125, "onBackColor", BLYNK_GREEN ); 
          
          break;
          case 6:
                    Blynk.setProperty(V126, "onBackColor", BLYNK_GREEN ); 
         
          break;
          case 7:
                    Blynk.setProperty(V127, "onBackColor", BLYNK_GREEN ); 
       
          break;
          case 8:
                    Blynk.setProperty(V93, "onBackColor", BLYNK_GREEN ); 
       
          break;
          case 9:
                    Blynk.setProperty(V80, "onBackColor", BLYNK_GREEN ); 
           
          break;
          case 10:
                    Blynk.setProperty(V21, "onBackColor", BLYNK_GREEN ); 
        
          break;
          case 11:
                    Blynk.setProperty(V14, "onBackColor", BLYNK_GREEN ); 
      
          break;
          case 12:
                    Blynk.setProperty(V15, "onBackColor", BLYNK_GREEN ); 
      
          break;
          case 13:
                    Blynk.setProperty(V23, "onBackColor", BLYNK_GREEN ); 
          
          break;
          case 14:
                    Blynk.setProperty(V103, "onBackColor", BLYNK_GREEN ); 
        
          break;
          case 15:
                    Blynk.setProperty(V104, "onBackColor", BLYNK_GREEN ); 
          
          break;
          case 16:
                    Blynk.setProperty(V105, "onBackColor", BLYNK_GREEN ); 
          
          break;
          case 17:
                    Blynk.setProperty(V90, "onBackColor", BLYNK_GREEN ); 
           
          break;
          case 18:
                    Blynk.setProperty(V91, "onBackColor", BLYNK_GREEN ); 
          
          break;
          case 19:
                    Blynk.setProperty(V92, "onBackColor", BLYNK_GREEN ); 
           
          break;                   
        }  
  }

   if (chMode == CH_MODE_3)
  {
        switch (lastSelectedCh)
        {
          case 1:

                 Blynk.setProperty(V112, "onBackColor", BLYNK_RED );
          break;

          case 2:
                    Blynk.setProperty(V122, "onBackColor", BLYNK_RED ); 
          
          break;
          case 3:
                    Blynk.setProperty(V123, "onBackColor", BLYNK_RED ); 
           
          break;
          case 4:
                    Blynk.setProperty(V124, "onBackColor", BLYNK_RED ); 
          
          break;
          case 5:
                    Blynk.setProperty(V125, "onBackColor", BLYNK_RED ); 
           
          break;
          case 6:
                    Blynk.setProperty(V126, "onBackColor", BLYNK_RED ); 
           
          break;
          case 7:
                    Blynk.setProperty(V127, "onBackColor", BLYNK_RED ); 
          
          break;
          case 8:
                    Blynk.setProperty(V93, "onBackColor", BLYNK_RED ); 
          
          break;
          case 9:
                    Blynk.setProperty(V80, "onBackColor", BLYNK_RED ); 
          
          break;
          case 10:
                    Blynk.setProperty(V21, "onBackColor", BLYNK_RED ); 
           
          break;
          case 11:
                    Blynk.setProperty(V14, "onBackColor", BLYNK_RED ); 
           
          break;
          case 12:
                    Blynk.setProperty(V15, "onBackColor", BLYNK_RED ); 
           
          break;
          case 13:
                    Blynk.setProperty(V23, "onBackColor", BLYNK_RED ); 
           
          break;
          case 14:
                    Blynk.setProperty(V103, "onBackColor", BLYNK_RED ); 
           
          break;
          case 15:
                    Blynk.setProperty(V104, "onBackColor", BLYNK_RED ); 
           
          break;
          case 16:
                    Blynk.setProperty(V105, "onBackColor", BLYNK_RED ); 
           
          break;
          case 17:
                    Blynk.setProperty(V90, "onBackColor", BLYNK_RED ); 
          
          break;
          case 18:
                    Blynk.setProperty(V91, "onBackColor", BLYNK_RED ); 
          
          break;
          case 19:
                    Blynk.setProperty(V92, "onBackColor", BLYNK_RED ); 
          
          break;                   
        }  
  }  

  
 

if(chMode == CH_MODE_4)
  {
    switch (lastSelectedCh)
        {
          case 1:
                   Blynk.virtualWrite(V112, 1); 
                 
          break;

          case 2:
                   Blynk.virtualWrite(V122, 1); 
                 
          break;
          case 3:
                   Blynk.virtualWrite(V123, 1); 
      

          break;
          case 4:
                   Blynk.virtualWrite(V124, 1); 
               
          break;
          case 5:
                   Blynk.virtualWrite(V125, 1); 
              

          break;
          case 6:
                   Blynk.virtualWrite(V126, 1); 
                 
          break;
          case 7:
                   Blynk.virtualWrite(V127, 1); 
                 

          break;
          case 8:
                   Blynk.virtualWrite(V93, 1); 
                 

          break;
          case 9:
                   Blynk.virtualWrite(V80, 1); 
                  

          break;
          case 10:
                   Blynk.virtualWrite(V21, 1); 
                 

          break;
          case 11:
                   Blynk.virtualWrite(V14, 1); 
                  

          break;
          case 12:
                   Blynk.virtualWrite(V15, 1); 
                 

          break;
          case 13:
                   Blynk.virtualWrite(V23, 1); 
                  

          break;
          case 14:
                   Blynk.virtualWrite(V103, 1); 
                 

          break;
          case 15:
                   Blynk.virtualWrite(V104, 1); 
                  

          break;
          case 16:
                   Blynk.virtualWrite(V105, 1); 
                  

          break;
          case 17:
                   Blynk.virtualWrite(V90, 1); 
                 

          break;
          case 18:
                   Blynk.virtualWrite(V91, 1); 
                  

          break;
          case 19:
                   Blynk.virtualWrite(V92, 1); 
                  

          break;                   
        }  
   }    
 }
}


void blynk::releActiveCh(int rele, int ch)
{
if(!zapSetup && !zapScanOnly)
 {
    switch (rele)
        {
          case 0:
                   Blynk.virtualWrite(V30, ch); 
          break;

          case 1:
                   Blynk.virtualWrite(V31, ch); 
          break;
          
          case 2:
                   Blynk.virtualWrite(V32, ch); 
          break;
          
          case 3:
                   Blynk.virtualWrite(V33, ch); 
          break;
        }
 }
}       

 void blynk::Event24(void)
 {
  Blynk.logEvent("meteor_restart");
 }
