#include "blynk_app.h"
#include "headers.h"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiMulti.h>








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


//WidgetLED T433_LED_V6(V6);   //T433 St
WidgetLED I2C_LED_V13(V13);  //I2C ACK
//WidgetLED I2C_SEC_LED_V12(V12);  //sms
//WidgetLED ZAP_LED_V80(V80);  //Zap Status
WidgetTerminal terminal(V102);

unsigned int myServerTimeout  =  3500;  //  3.5s server connection timeout (SCT)
unsigned int myWiFiTimeout    =  3200;  //  3.2s WiFi connection timeout   (WCT)
unsigned int functionInterval =  7500;  //  7.5s function call frequency   (FCF)
unsigned int blynkInterval    = 25000;  // 25.0s check server frequency    (CSF)


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
}

void sendTimeToBlynk_7500ms(){
  Serial.println("\tLook, no Blynk  block.");
  if(wifiMulti.run()== 3){
    Serial.println("\tWiFi still  connected.");
    _wifiIsConnected = true;
  }

  if(Blynk.connected())
  {
    if(!blynkActive)
    {
      Serial.println("\tTick update to blynk.");
    }
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

    wifiMulti.addAP(WIFI_SSID_METEOR, WIFI_PASSWORD_METEOR);
    wifiMulti.addAP(WIFI_SSID_GIGA, WIFI_PASSWORD);
    wifiMulti.addAP(WIFI_SSID_FREE, WIFI_PASSWORD);
    wifiMulti.addAP(WIFI_SSID_XIAOMI , WIFI_PASSWORD);
    wifiMulti.addAP(WIFI_SSID_HUAWEI , WIFI_PASSWORD);
    
    
    if(wifiMulti.run() == 6){
    Serial.println("\tWiFi not connected yet.");
  }
  
  timer.setInterval(functionInterval, sendTimeToBlynk_7500ms);// run some function at intervals per functionInterval
  timer.setInterval(blynkInterval, checkBlynk);   // check connection to server per blynkInterval
  unsigned long startWiFi = millis();

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
  
  Blynk.config(BLYNK_AUTH_TOKEN, BLYNK_SERVER);
  Blynk.connect(); 
  checkBlynk();
  ledInit();
  blynkAtiveTimer     = millis();
  



  



  if(_blynkIsConnected)
  {
  terminal.clear();  
  Blynk.virtualWrite(V102,"Blynk v ", VERSION_ID, ": Device started\n");
  Blynk.virtualWrite(V102,"-------------\n");
  Blynk.virtualWrite(V102,"Type 'Marco' and get a reply, or type\n");
  Blynk.virtualWrite(V102,"anything else and get it printed back.\n");
  }
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
    Serial.println(_blynkData);
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}

BLYNK_WRITE(V4)  
{

    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SWITCHING_25_V4;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V5)  
{

    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SWITCHING_48_V5;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V6)  
{

    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SWITCHING_26_V6;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V7)   
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SWITCHING_65_V7;
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
    eventdata = Q_EVENT_SWITCHING_50_V10;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}


BLYNK_WRITE(V11)  
{

    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SWITCHING_28_V11;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);

}

BLYNK_WRITE(V12)  
{

    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_SWITCHING_52_V12;
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


BLYNK_WRITE(V30)   //03 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V30_SWITCHING_64;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}



BLYNK_WRITE(V31)   //21
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V31_SWITCHING_49;    
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V32)   //27
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V32_SWITCHING_62;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V33)   //50
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V33_SWITCHING_51;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V34)   //20
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V34_SWITCHING_66;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V35)   //52
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V35_SWITCHING_53;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V36)   //53 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V36_SWITCHING_29;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V37)   //22 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V37_SWITCHING_63;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V38)   //208 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V38_SWITCHING_68;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V39)   //215
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V39_SWITCHING_27;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}


BLYNK_WRITE(V40)   //217 
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_V40_SWITCHING_67;
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



BLYNK_WRITE(V102)  //TERMINAL
{

    // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) 
  {
    Blynk.virtualWrite(V102, "You said: 'Marco'\nI said: 'Paolo'\n");
  } 
  else 
  {
    // Send it back
    Blynk.virtualWrite(V102, "\nYou said:", param.asStr());

/*  OLDER EXAMPLE VERSION HAD THESE LINES     
*   terminal.print("You said:");
*   terminal.write(param.getBuffer(), param.getLength());
*   terminal.println(); 
*/
  }   
   
//    _blynkEvent = true;
//    _blynkData=param.asInt();
//    eventdata = Q_EVENT_TERMINAL_V102;
//    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
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

BLYNK_WRITE(V112)   //Zapping ch20
{
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_ZAP_CHANNEL20_V112;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
}






BLYNK_WRITE(V121)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_01_V121;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}

BLYNK_WRITE(V122)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_02_V122;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V123)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_03_V123;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V124)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_04_V124;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V125)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_05_V125;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V126)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_06_V126;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V127)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_07_V127;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}

BLYNK_WRITE(V80)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_09_V80;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V21)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_10_V21;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V14)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_11_V14;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V15)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_12_V15;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V23)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_13_V23;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V103)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_14_V103;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V104)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_15_V104;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V105)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_16_V105;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V90)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_17_V90;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V91)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_18_V91;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V92)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_19_V92;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}
BLYNK_WRITE(V93)  
{
   if( param.asInt() != 0)
    {
    _blynkEvent = true;
    _blynkData=param.asInt();
    eventdata = Q_EVENT_RM_ID_08_V93;
    xQueueSend(g_event_queue_handle, &eventdata, portMAX_DELAY);
    }
}



void blynk::notifierDebug(String subject, String body)
{
      Blynk.logEvent(String(subject +"**"+ body) );
}


bool blynk::wifiConnect()
  {
    WiFi.begin(WIFI_SSID_FB, WIFI_PASSWORD);
    if (WiFi.status()  == WL_CONNECTED ){DEBUG_PRINTLN("Wifi connected");return true; }
  
    long timeout = millis();
    long wifiReconnect = millis();
    
    while ( WiFi.status()  != WL_CONNECTED ) 
      {
        if (millis() - wifiReconnect > WIFI_RECONNECT_TIMER ) 
          {
            WiFi.begin(WIFI_SSID_FB, WIFI_PASSWORD);
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





void blynk::blynkRCLed315(bool _data)
{

}

void blynk::blynkSmsLed(bool _data)
{
}

void blynk::zapLed(bool _data)
{

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

}

void blynk::sevenSegValue(int vch )
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



void blynk::blynkAckLed( bool _data)
{

                  if (_data==1)  I2C_LED_V13.setColor(BLYNK_RED);
                  else           I2C_LED_V13.setColor(BLYNK_GREEN);
 
}


void blynk::blynkRCLed(bool _data, int cmd)
{
/*
if (_data==0)  
    {
    switch (cmd)
        {
          case 1:
                     Blynk.setProperty(V121, "color", BLYNK_GREEN);
          break;
          case 2:
                      Blynk.setProperty(V122, "color", BLYNK_GREEN);
          break;

            case 3:
                      Blynk.setProperty(V123, "color", BLYNK_GREEN); 
          break;
          case 4:
                      Blynk.setProperty(V124, "color", BLYNK_GREEN); 
          break;                     

          case 5:
                     Blynk.setProperty(V125, "color", BLYNK_GREEN);
          break;
          case 6:
                      Blynk.setProperty(V126, "color", BLYNK_GREEN);
          break;

          case 7:
                      Blynk.setProperty(V127, "color", BLYNK_GREEN); 
          break;
          case 8:
                      Blynk.setProperty(V93, "color", BLYNK_GREEN); 
          break; 
          case 9:
 //                    Blynk.setProperty(V129, "color", BLYNK_GREEN);
          break;
          case 10:
 //                     Blynk.setProperty(V130, "color", BLYNK_GREEN);
          break;

          case 11:
                      Blynk.setProperty(V14, "color", BLYNK_GREEN); 
          break;
          case 12:
                      Blynk.setProperty(V15, "color", BLYNK_GREEN); 
          break; 
          case 13:
                     Blynk.setProperty(V23, "color", BLYNK_GREEN);
          break;
          case 14:
                      Blynk.setProperty(V103, "color", BLYNK_GREEN);
          break;

          case 15:
                      Blynk.setProperty(V104, "color", BLYNK_GREEN); 
          break;
          case 16:
                      Blynk.setProperty(V105, "color", BLYNK_GREEN); 
          break; 
          case 17:
                      Blynk.setProperty(V91, "color", BLYNK_GREEN);
          break;

          case 19:
                      Blynk.setProperty(V92, "color", BLYNK_GREEN); 
          break;
        }   


           // T433_LED_V6.setColor(BLYNK_RED);
            if ( (cmd >= 1) && (cmd <= 5))  Blynk.setProperty(V3, "color", BLYNK_GREEN);
            if ( (cmd >= 6) && (cmd <= 10))  Blynk.setProperty(V16, "color", BLYNK_GREEN);
            if ( (cmd >= 11) && (cmd <= 15))  Blynk.setProperty(V17, "color", BLYNK_GREEN);
            if ( (cmd >= 16) && (cmd <= 20))  Blynk.setProperty(V18, "color", BLYNK_GREEN);
            if ( (cmd >= 20) && (cmd <= 25))  Blynk.setProperty(V25, "color", BLYNK_GREEN);

          }
      else           
      {
    switch (cmd)
        {
          case 1:
                     Blynk.setProperty(V121, "color", BLYNK_RED);
          break;
          case 2:
                      Blynk.setProperty(V122, "color", BLYNK_RED);
          break;

            case 3:
                      Blynk.setProperty(V123, "color", BLYNK_RED); 
          break;
          case 4:
                      Blynk.setProperty(V124, "color", BLYNK_RED); 
          break;                     

          case 5:
                     Blynk.setProperty(V125, "color", BLYNK_RED);
          break;
          case 6:
                      Blynk.setProperty(V126, "color", BLYNK_RED);
          break;

          case 7:
                      Blynk.setProperty(V127, "color", BLYNK_RED); 
          break;
          case 8:
                      Blynk.setProperty(V93, "color", BLYNK_RED); 
          break; 
          case 9:
 //                    Blynk.setProperty(V129, "color", BLYNK_RED);
          break;
          case 10:
 //                     Blynk.setProperty(V130, "color", BLYNK_RED);
          break;

          case 11:
                      Blynk.setProperty(V14, "color", BLYNK_RED); 
          break;
          case 12:
                      Blynk.setProperty(V15, "color", BLYNK_RED); 
          break; 
          case 13:
                     Blynk.setProperty(V23, "color", BLYNK_RED);
          break;
          case 14:
                      Blynk.setProperty(V103, "color", BLYNK_RED);
          break;

          case 15:
                      Blynk.setProperty(V104, "color", BLYNK_RED); 
          break;
          case 16:
                      Blynk.setProperty(V105, "color", BLYNK_RED); 
          break; 
          case 17:
                      Blynk.setProperty(V91, "color", BLYNK_RED);
          break;

          case 19:
                      Blynk.setProperty(V92, "color", BLYNK_RED); 
          break;
        } 


           // T433_LED_V6.setColor(BLYNK_GREEN);
            if ( (cmd >= 1) && (cmd <= 5))  Blynk.setProperty(V3, "color", BLYNK_RED);
            if ( (cmd >= 6) && (cmd <= 10))  Blynk.setProperty(V16, "color", BLYNK_RED);
            if ( (cmd >= 11) && (cmd <= 15))  Blynk.setProperty(V17, "color", BLYNK_RED);
            if ( (cmd >= 16) && (cmd <= 20))  Blynk.setProperty(V18, "color", BLYNK_RED);
            if ( (cmd >= 20) && (cmd <= 25))  Blynk.setProperty(V25, "color", BLYNK_RED);     
            
      }  
  */ 
}

void blynk::visualActiveRoom(int id, bool zap)
{
/*
  if ( (id >= 1) && (id <= 5)) 
    { 
      if (zap) Blynk.virtualWrite(V3, id);
      Blynk.setProperty(V3, "color", BLYNK_GREEN);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
      Blynk.virtualWrite(V3, 6);
      Blynk.virtualWrite(V16, 11);
      Blynk.virtualWrite(V17, 16);
      Blynk.virtualWrite(V18, 21);
      Blynk.virtualWrite(V25, 26);
    }
  if ( (id >= 6) && (id <= 10)) 
    { 
      if (zap)Blynk.virtualWrite(V16, id);
      Blynk.setProperty(V16, "color", BLYNK_GREEN);
      Blynk.setProperty(V3, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
      Blynk.virtualWrite(V3, 6);
      Blynk.virtualWrite(V16, 11);
      Blynk.virtualWrite(V17, 16);
      Blynk.virtualWrite(V18, 21);
      Blynk.virtualWrite(V25, 26);
   }
  if ( (id >= 11) && (id <= 15)) 
    { 
      if (zap)Blynk.virtualWrite(V17, id);
      Blynk.setProperty(V17, "color", BLYNK_GREEN);
      Blynk.setProperty(V3, "color", BLYNK_BLACK);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
      Blynk.virtualWrite(V16, 11);
      Blynk.virtualWrite(V3, 6);
      Blynk.virtualWrite(V17, 16);
      Blynk.virtualWrite(V18, 21);
      Blynk.virtualWrite(V25, 26);
   }
  if ( (id >= 16) && (id <= 20)) 
    { 
      if (zap)Blynk.virtualWrite(V18, id);
      Blynk.setProperty(V18, "color", BLYNK_GREEN);
      Blynk.setProperty(V3, "color", BLYNK_BLACK);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V25, "color", BLYNK_BLACK);
      Blynk.virtualWrite(V16, 11);
      Blynk.virtualWrite(V17, 16);
      Blynk.virtualWrite(V3, 6);
      Blynk.virtualWrite(V18, 21);
      Blynk.virtualWrite(V25, 26);
   }
  if ( (id >= 21) && (id <= 25)) 
    { 
     if (zap) Blynk.virtualWrite(V25, id);
      Blynk.setProperty(V25, "color", BLYNK_GREEN);
      Blynk.setProperty(V3, "color",  BLYNK_BLACK);
      Blynk.setProperty(V16, "color", BLYNK_BLACK);
      Blynk.setProperty(V17, "color", BLYNK_BLACK);
      Blynk.setProperty(V18, "color", BLYNK_BLACK);
      Blynk.virtualWrite(V16, 11);
      Blynk.virtualWrite(V17, 16);
      Blynk.virtualWrite(V18, 21);
      Blynk.virtualWrite(V3, 26);
      Blynk.virtualWrite(V25, 26);
   }
*/

}

bool blynk::blynkStatus(void)
{
 return  _blynkIsConnected;
}

bool blynk::wifiStatus(void)
{
 return  _wifiIsConnected;
}

void blynk::RelaySelect(void)
{
 Blynk.virtualWrite(V9, 1);
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

void blynk::TerminalPrint (String str)
{
  Blynk.virtualWrite(V102,str);
  Blynk.virtualWrite(V102,".\n");
}