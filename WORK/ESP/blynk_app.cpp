#include "blynk_app.h"
#include "headers.h"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiMulti.h>
#include "routers.h"

const uint32_t DEBOUNCE_MS = 3000;

uint32_t lastPressTimeV6 =0  ;
uint32_t lastPressTimeV7 =0  ;
uint32_t lastPressTimeV8 =0  ;

extern uint32_t crashCount;
extern String VERSION_ID ;
extern const char* BLYNK_AUTH_TOKEN;
extern int inactivityPowerOffTimer  ; //1 Hour;
extern int inactivityRestartTimer  ; //10 Hours;
extern int zapTimerSec;
extern String VERSION_ID  ;
extern String BOARD;
bool internetConnected = false;
extern void loadCrashCount();
extern uint32_t  restartAfterResetNG;
extern void resetInactivityTimer();
extern bool wifiAvailable ;

extern int MapIndex;
extern void dvrOnOff (bool powerOn);
extern void SendLiveLed(void);
extern void rebootSw(void);
extern bool DvrChOn;
extern int hmi;
extern int ActiveBoard ;
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
extern bool dvrSleep ;
extern int stateDVR;
extern int  Av_Rx;
#include <Update.h>
#include <HTTPClient.h>
WiFiMulti wifiMulti;
BlynkTimer timer;

extern int selected_room ;
extern int selected_Rx;
long  blynkAtiveTimer;
bool  blynkActive = false;

int _t433ChNumber, _t315ChNumber, _blynkfreqValue, _sevenSeg;
int _otaBlynk = 0;
int _bootBlynk = 0;
int _fbonBlynk = 0;
int _smsBlynk = 0;
bool _blynkEvent = true;
int  _blynkData = 0;
int  _blynkEventID = 0;
int _tempoVar;
bool _wifiIsConnected = false;
bool firstConnect = false;

extern EventGroupHandle_t g_event_group;
extern QueueHandle_t g_event_queue_handle;

bool ledStatus = false;
extern bool queuValidData;


WidgetLED I2C_LED_V13(V13);  //I2C ACK
WidgetLED LIVE_LED_V121(V121);  //LIVE
WidgetMap myMap(V12);
WidgetTerminal terminal(V102);

unsigned int BlynkServerTimeout  =  5000;  //  5s server connection timeout (SCT)
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
blynk::blynk(void)
{

}


void ledInit(void)
{
  I2C_LED_V13.on();
  LIVE_LED_V121.on();
}

// V121 LED Widget 
void blinkLedWidget()
{
  if (!blynkActive &&  !zapOnOff && !zapScanOnly && dvrSleep && stateDVR == DVR_OFF)
  {
    if(ledStatus)
    {
      LIVE_LED_V121.setColor(BLYNK_RED);
      DEBUG_PRINTLN("LED on V121: Red");
      ledStatus = false;
    }      
  }
  else if(!ledStatus)
  {
    LIVE_LED_V121.setColor(BLYNK_GREEN);
    DEBUG_PRINTLN("LED on V121: Green");
    ledStatus = true;
  }
}

void   AvRxIndex(int _index)
{
  Blynk.virtualWrite(V19, _index);
  if (_index == 1) {
    Blynk.setProperty(V19, "color", BLYNK_GREEN);
  }
  if (_index == 2) {
    Blynk.setProperty(V19, "color", BLYNK_YELLOW);
  }
  if (_index == 3) {
    Blynk.setProperty(V19, "color", BLYNK_RED);
  }

}

void chSelect(String ch)
{
  String SelectedCh = "rtsp://admin:basma28112018@192.168.10.114:554/" + ch + "/0" ;
      if        (ActiveBoard == ESP1 ) SelectedCh = "rtsp://admin:basma28112018@192.168.10.112:554/" + ch + "/0" ;
      else  if  (ActiveBoard == ESP2 ) SelectedCh = "rtsp://admin:basma28112018@192.168.10.114:554/" + ch + "/0" ;
      else if   (ActiveBoard == ESP3 ) SelectedCh = "rtsp://admin:basma28112018@192.168.1.108:554/" + ch + "/0" ;
  Blynk.setProperty(V28, "url", SelectedCh);
}

void  versionBlynk(String ver)
{
  Blynk.virtualWrite(V24, ver);
}

void  terminalSend (String str)
{
  String msg =  str;
  if ( blynkConnected )
  {
    terminal.println(str);
    terminal.flush();
  }
  else DEBUG_PRINTLN(str);
}

void blynk::mapRefresh(int index)
{
  double lat ;
  double lon ; //PLS 4 
  myMap.clear();
  //  int index = MapIndex;
  switch (index)
  {
    case 1:
      lat = 48.79922843700954; 
      lon = 1.9549987192776308; //PLS  
      myMap.location(index, lat, lon, "PLS");
      break;

    case 2:
      lat = 48.79922843700954; 
      lon = 1.9549987192776308; //PLS  
      myMap.location(index, lat, lon, "PLS");
      break;

      break;

  }

}





void blynk::streamSelect(String ch)
{
  String SelectedCh = "rtsp://admin:basma28112018@192.168.10.114:554/" + ch + "/0" ;
      if        (ActiveBoard == ESP1 ) SelectedCh = "rtsp://admin:basma28112018@192.168.10.112:554/" + ch + "/0" ;
      else  if  (ActiveBoard == ESP2 ) SelectedCh = "rtsp://admin:basma28112018@192.168.10.113:554/" + ch + "/0" ;
      else if   (ActiveBoard == ESP3 ) SelectedCh = "rtsp://admin:basma28112018@192.168.1.108:554/" + ch + "/0" ;
  Blynk.setProperty(V28, "url", SelectedCh);
}



/*********************************************************************************************************************/

//IPAddress   BLYNK_BBOX(192,168,1,4);
//const char* SSID_BBOX     ="Bbox-Bek-2.4G" ;
//const char* WIFI_PW_BBOX    =  "Ali09042010_";
//
//IPAddress   BLYNK_SFR(192,168,1,46);
//const char* SSID_SFR      ="SFR_BEK-23C0";
//const char* WIFI_PW_SFR     =  "ali09042010";
//
//IPAddress   BLYNK_PI  (192,168,10,195);
//IPAddress   BLYNK_PC  (192,168,10,196);
//const char* SSID_METEOR   ="BEK_METEOR_2.4G";
//const char* WIFI_PW_METEOR  =  "Ali09042010_";
//
//IPAddress   BLYNK_FLIP7(10,174,107,53);  //
//const char* SSID_FLIP7    ="BEK_FLIP7" ;
//const char* WIFI_PW_FLIP7   =  "ali09042010";

//struct NetworkConfig
//{
//    const char* ssid;
//    const char* wifiPw;
//    IPAddress server1;
//    IPAddress server2;
//    IPAddress server3;
//    uint16_t port;
//    const char* location;
//};
//
//NetworkConfig nets[] =
//{
//    {SSID_METEOR  , WIFI_PW_METEOR   , BLYNK_PI     , BLYNK_PC      , BLYNK_FLIP7   , 8080, "PLS"},
////  {SSID_FLIP7   , WIFI_PW_FLIP7    , BLYNK_FLIP7  , BLYNK_FLIP7   , BLYNK_FLIP7   , 8080, "MOBILE"},
//    {SSID_BBOX    , WIFI_PW_BBOX     , BLYNK_BBOX   , BLYNK_BBOX    , BLYNK_BBOX    , 8080, "CH"},
//    {SSID_SFR     , WIFI_PW_SFR      , BLYNK_SFR    , BLYNK_SFR     , BLYNK_SFR     , 8080, "NICE"} 
//};




NetworkConfig* getCurrentNetwork()
{
    String currentSSID = WiFi.SSID();
//#if defined TESTING
//  NUM_NETWORKS = sizeof(nets) / sizeof(nets[0]);
//#else
//  NUM_NETWORKS = 1;
//#endif
    for (int i = 0; i < NUM_NETWORKS; i++)
    {
        if (currentSSID == nets[i].ssid  )
        {
            return &nets[i];
        }
    }
    return nullptr;
}

bool blynk_connect()
{
    NetworkConfig* net = getCurrentNetwork();

    if (net == nullptr)
    {
        DEBUG_PRINT("Unknown SSID: ");
        DEBUG_PRINTLN(WiFi.SSID());
        return false;
    }

    IPAddress servers[] =
    {
        net->server1,
        net->server2,
        net->server3
    };

    DEBUG_PRINTLN("--------------------------------");
    DEBUG_PRINT("Connected SSID : ");
    DEBUG_PRINTLN(net->ssid);

    Blynk.disconnect();
    delay(100);

    for (uint8_t i = 0; i < 3; i++)
    {
        DEBUG_PRINT("Trying Blynk Server : ");
        DEBUG_PRINTLN(servers[i]);

        Blynk.config(BLYNK_AUTH_TOKEN, servers[i], 8080);

        if (Blynk.connect(BlynkServerTimeout))
        {
            DEBUG_PRINTLN("Blynk Connected");

            terminal.clear();
            terminal.println(
                String(net->ssid) +
                " IP:" + WiFi.localIP().toString() +
                " RSSI:" + String(WiFi.RSSI()) +
                " Server:" + servers[i].toString()
            );
            terminal.flush();

            blynkAtiveTimer = millis();
            blynkActive = true;        // I think this should be true
            ledInit();

            return true;
        }

        DEBUG_PRINTLN("Connection Failed");
    }

    DEBUG_PRINTLN("No Blynk server available");
    return false;
}

/*********************************************************************************************************************/

bool checkInternet()
{
WiFiClient client;
IPAddress ip;
//    if (WiFi.hostByName("raw.githubusercontent.com", ip))
//    {
//        DEBUG_PRINT("GitHub IP: ");
//        DEBUG_PRINTLN(ip);
//    }
//    else
//    {
//        DEBUG_PRINTLN("DNS FAILED");
//    }
//    
  if (client.connect("1.1.1.1", 80))
  {
      DEBUG_PRINTLN("Internet OK");
      client.stop();
      return true;
  }
  else
  {
      DEBUG_PRINTLN("No Internet");
      return false;
  }  
}



bool  wifi_connect()
{
//  #if defined TESTING
//  NUM_NETWORKS = sizeof(nets) / sizeof(nets[0]);
//#else
//  NUM_NETWORKS = 1;
//#endif

  bool wifiConnection = false;
  for (int i = 0; i < NUM_NETWORKS; i++)
    {
      wifiMulti.addAP(nets[i].ssid,nets[i].wifiPw);
    } 

  DEBUG_PRINTLN("Connecting to Wifi...");
  //Connecting to the strongest WiFi connection
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
  {
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("WiFi is connected to: " + String (WiFi.SSID() )) ;
    DEBUG_PRINTLN("ESP Local IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());  //print IP of the connected WiFi network
    wifiConnection = true;
  }
  else  // if not WiFi not connected
  {
    DEBUG_PRINTLN("WIFI Connection Failed");  
    wifiConnection = false;
  }

  return wifiConnection ;
}


void checkBlynk() {
  
  internetConnected = checkInternet();
  
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
  {
    unsigned long startConnecting = millis();
    wifiAvailable = true;

    while (!Blynk.connected()) {
      
      if (millis() > startConnecting + BlynkServerTimeout) {
        DEBUG_PRINTLN("Wifi connected but Blynk is Disconnected, connectig agin to Blynk....");
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("WiFi is connected to: " + String (WiFi.SSID() )) ;
        DEBUG_PRINTLN("ESP Local IP address: ");
        DEBUG_PRINTLN(WiFi.localIP());  //print IP of the connected WiFi network
    
        blynkConnected = blynk_connect();
        if (blynkConnected) 
              {
                 AvRxIndex(Av_Rx);
                 chSelect("ch01");
                 versionBlynk(VERSION_ID);
                 loadCrashCount();
                 terminalSend (VERSION_ID + " " + String(crashCount) + " Craches" );
              }
        else DEBUG_PRINTLN("Unable to connect to Blynk server. ");
        break;
      }
    }
  }
  else
  {
     DEBUG_PRINTLN("WIFI Diconnected!! Trying to reconnect.");//); 
     wifiAvailable  = wifi_connect();
     blynkConnected = false;
  }
    
    DEBUG_PRINTLN("--------------------------------");
    DEBUG_PRINT("WIFI is ");
    DEBUG_PRINTLN(wifiAvailable ? F("Connected") : F("Not Connected"));
    DEBUG_PRINT("BLYNK is ");
    DEBUG_PRINTLN(blynkConnected ? F("Connected") : F("Not Connected"));
    
    DEBUG_PRINTF("Checking the connection again in %is.\n", blynkIntervalInterval / 1000);

    if(InternetLoss)
    {
      unsigned long remaining = RESTART_AFTER_NG_RESET_TIMER - (millis() - restartAfterResetNG);
      
      unsigned long minutes = remaining / 60000;
      unsigned long seconds = (remaining % 60000) / 1000;
      
      DEBUG_PRINT("Restart in ");
      DEBUG_PRINT(minutes);
      DEBUG_PRINT(":");
      if (seconds < 10) DEBUG_PRINT('0');
      DEBUG_PRINTLN(seconds);      
    
    }
}


bool blynk::wifi_init()
{
  return(wifi_connect());
}

bool blynk::init()
{
  timer.setInterval(LiveUpdateInterval, SendLiveLed);// run some function at intervals per LiveUpdateInterval
  timer.setInterval(blynkIntervalInterval, checkBlynk);   // check connection to server per blynkIntervalInterval
  timer.setInterval(5000L, blinkLedWidget);

  if (wifiAvailable) blynkConnected = blynk_connect();
  else blynkConnected = false;
  return blynkConnected;
}


BLYNK_WRITE(V0)  //freq
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_FREQ_V0;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V1) //rc433
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RC_CH_NR_V1;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V2) // receiver ch
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_VIDEO_CH_V2;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V3) // catchCh
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_REPEAT_V3;
  DEBUG_PRINTLN(_blynkData);
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V4)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_SCAN_ACTIVE_CH_V4;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V5)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_AUTOMATIC_RC_L_R_V5;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

bool debounceButton(uint32_t &lastTime, uint32_t debounceMs)
{
    uint32_t now = millis();

    if (now - lastTime < debounceMs)
        return false;

    lastTime = now;
    return true;
}
 
BLYNK_WRITE(V6) //OTA_LOCAL_WEB
{
    if (param.asInt() != 1)
        return;

    if (!debounceButton(lastPressTimeV6, DEBOUNCE_MS))
        return;
  
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_OTA_LOCAL_WEB_WIFI_V6;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V7)  //OTA_GITHUB
{
    if (param.asInt() != 1)
        return;

    if (!debounceButton(lastPressTimeV7, DEBOUNCE_MS))
        return;
    
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_OTA_GITHUB_V7;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V8)   //boot
{
    if (param.asInt() != 1)
        return;

    if (!debounceButton(lastPressTimeV8, DEBOUNCE_MS))
        return;
    
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_REBOOT_V8;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V9) // Room Nr
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SELECTED_RECIEVER_CH_1_4_V9;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V10)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_SCAN_ONLY_V10;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}


BLYNK_WRITE(V11)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_WIFI_IDE_V11;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V12)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SPARE_V12;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}



BLYNK_WRITE(V16) // // RC LOCAL REMOTE TO ESP1
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RC_REMOTE_ESP1_V16;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V17) // RC LOCAL REMOTE TO ESP3
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RC_REMOTE_ESP3_V17;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V18) // ROOM_16_TO_20
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_AWS_RECONNECT_V18;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V19) // ROOM_AV RC
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ROOM_AV_RC_V19;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V20) // RC LOCAL REMOTE TO ESP2
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RC_REMOTE_ESP2_V20;
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
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_TIMER_OFF_V25;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V26)   //RESET FREQ
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RESET_FREQ_V26;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V27)   //DVR ON OFF
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SPARE_V27;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V30)   //03
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_DVR_OFF_TIMER_V30;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V31)   //21
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RESTART_TIMER_V31;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V32)   //27
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SELECTED_RECIEVER_CH_5_6_V32;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V33)   //50
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SPARE_V33;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V34)   //20
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_ALL_ON_OFF_V34;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V35)   //52
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SHARE_VIDEO_WITH_ESP1_V35;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V36)   //53
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SHARE_VIDEO_WITH_ESP2_V36;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V37)   //22
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_SHARE_VIDEO_WITH_ESP3_V37;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V38)   //NOT USED
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_WIFI_RSSI_V38;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V39)   //215
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RESET_ROUTER_V39;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V40)   //217
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_AUTOMATIC_RESET_ROUTER_V40;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V71)   //Zapping On Off Switch
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_V71;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V72)   //Zapping Timer
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_TIMER_V72;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V81)   //DVR NEW
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_VIDEO_ON_OFF_V81;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

//BLYNK_WRITE(V82)   //Zapping ch2
//{
//    _blynkEvent = true;
//    _blynkData=param.asInt();
//    eventdata = Q_EVENT_SPARE_V82;
//    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
//}
//
//BLYNK_WRITE(V83)   //Zapping ch3
//{
//    _blynkEvent = true;
//    _blynkData=param.asInt();
//    eventdata = Q_EVENT_SPARE_V83;
//    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
//
//}

BLYNK_WRITE(V84)   //Zapping ch4
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_VIDEO_STREAMING_V84;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V85)   //Zapping ch5
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL5_V85;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V86)   //Zapping ch6
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL6_V86;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V87)   //Zapping ch7
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL7_V87;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V88)   //Zapping ch8
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL8_V88;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V89)   //Zapping ch9
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL9_V89;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}





BLYNK_WRITE(V94)   //Zapping ch10
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL10_V94;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V95)   //Zapping ch11
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL11_V95;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V96)   //Zapping ch12
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL12_V96;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V97)   //Zapping ch14
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL13_V97;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V98)  //pulse
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RC_PULSE_V98;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V100)  //wifi ide
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_20_V100;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V101)  //repetion
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RC_REPETION_V101;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V102)  //TERMINAL
{
  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
    terminal.println("You said: 'Marco'") ;
    terminal.println("I said: 'Polo'") ;
    terminal.flush();
  }

  else if (String("w") == param.asStr())
  {
    terminal.println( WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) + "\n");
    terminal.flush();
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
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL14_V106;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V107)   //Zapping ch15
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL15_V107;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V108)   //Zapping ch16
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL16_V108;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V109)   //Zapping ch17
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL17_V109;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V110)   //Zapping ch18
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL18_V110;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V111)   //Zapping ch19
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_ZAP_CHANNEL19_V111;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V112)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
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
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_02_V122;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V123)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_03_V123;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V124)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_04_V124;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V125)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_05_V125;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V126)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_06_V126;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V127)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_07_V127;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V80)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_09_V80;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V21)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_10_V21;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V22)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_MAP_INDEX_V22;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V14)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_11_V14;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V15)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_12_V15;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V23)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_13_V23;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V103)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_14_V103;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V104)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_15_V104;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V105)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_16_V105;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V90)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_17_V90;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V91)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_18_V91;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}
BLYNK_WRITE(V92)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_19_V92;
  xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}
BLYNK_WRITE(V93)
{

  _blynkEvent = true;
  _blynkData = param.asInt();
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
    blynkData = _blynkData;
    blynkEventID = _blynkEventID;
    _blynkEvent = false;
    blynkActive = true;
    blynkAtiveTimer     = millis();
    hmi = BLYNK;
    LiveSec = LiveMin = LiveHour = 0;
    resetInactivityTimer();
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
  if (!zapSetup && !zapScanOnly)
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
  if (!zapSetup && !zapScanOnly)
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
  if (!zapSetup && !zapScanOnly) Blynk.virtualWrite(V0, freq);
}

void blynk::dvrSwitch(bool cmd)
{
  Blynk.virtualWrite(V81, cmd);
}

void blynk::sendAvRxIndex(int _index)
{
  Blynk.virtualWrite(V19, _index);
  if (_index == 1) {
    Blynk.setProperty(V19, "color", BLYNK_GREEN);
  }
  if (_index == 2) {
    Blynk.setProperty(V19, "color", BLYNK_YELLOW);
  }
  if (_index == 3) {
    Blynk.setProperty(V19, "color", BLYNK_RED);
  }

}

void blynk::sendBoardIndex(int _index)
{
  // Blynk.virtualWrite(V12, _index);
  //  if (_index ==1) {Blynk.setProperty(V12, "color", BLYNK_GREEN);}
  //  if (_index ==2) {Blynk.setProperty(V12, "color", BLYNK_YELLOW);}
  //  if (_index ==3) {Blynk.setProperty(V12, "color", BLYNK_RED);}
  //  if (_index ==4) {Blynk.setProperty(V12, "color", BLYNK_BLUE);}

}

void blynk::blynkAckLed( bool _data)
{
  if (RC_Api)  I2C_LED_V13.setColor(BLYNK_YELLOW);
  else
  {
    if (_data == 1)  I2C_LED_V13.setColor(BLYNK_RED);
    else           I2C_LED_V13.setColor(BLYNK_GREEN);
  }
}



void blynk::liveLedCall(bool _data)
{

}

void blynk::visualActiveRoom(int id, bool zap)
{

}


 
void blynk::RelaySelect(int receiver)
{
  if (receiver >= 1 && receiver <= 4 )
  {
    Blynk.virtualWrite(V9, receiver);
    Blynk.virtualWrite(V32, 0);
  }
  else if (receiver >= 5 && receiver <= 8 )
  {
    Blynk.virtualWrite(V32, (receiver - 4));
    Blynk.virtualWrite(V9, 0);
  }
}

void blynk::sendVersion(String ver)
{
  Blynk.virtualWrite(V24, ver);
}

void blynk::sendNotify(String msg)
{
  Blynk.notify(msg);
}

void blynk::SyncAll(void)
{
}

void blynk::blynkTimers(void)
{
  Blynk.virtualWrite(V30, inactivityPowerOffTimer);
  Blynk.virtualWrite(V31, inactivityRestartTimer);
  Blynk.virtualWrite(V72, zapTimerSec);
}

void blynk::repeatSync(bool repeat)
{
  Blynk.virtualWrite(V3, repeat);
}

void blynk::TerminalPrint (String str)
{
  String msg =  str;
  if ( blynkConnected )
  {
    terminal.println(str);
    terminal.flush();
  }
  else DEBUG_PRINTLN(str);
}

void blynk::BlynkButtonColours(int lastSelectedCh, int chMode)
{
  if (!zapSetup && !zapScanOnly)
  {
    if (chMode == CH_MODE_0)
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




    if (chMode == CH_MODE_4)
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
  if (!zapSetup && !zapScanOnly)
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
  if (httpCode != HTTP_CODE_OK) {
    Blynk.connect();
    return;
  }
  int contentLength = http.getSize();
  if (contentLength <= 0) {
    Blynk.connect();
    return;
  }
  bool canBegin = Update.begin(contentLength);
  if (!canBegin) {
    Blynk.connect();
    return;
  }
  Client& client = http.getStream();
  int written = Update.writeStream(client);
  if (written != contentLength) {
    Blynk.connect();
    return;
  }
  if (!Update.end()) {
    Blynk.connect();
    return;
  }
  if (!Update.isFinished()) {
    Blynk.connect();
    return;
  }
  reboot();
}
