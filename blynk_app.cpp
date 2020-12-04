#include "blynk_app.h"
#include "headers.h"





#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>



WidgetLED led1(V4);   //Alive Led BEK
WidgetLED led8(V16);   //Alive Led BEK
WidgetLED led2(V5);   //Ack
WidgetLED led3(V6);   //T433 St
WidgetLED led6(V13);  //T315 St
WidgetLED led4(V9);   //Send to blynlk
WidgetLED led5(V12);  //sms
WidgetLED led7(V80);  //Zap Status


#ifdef BEK
    char auth[] = "D4AU1HexWcErQ9vtpkP_EgocpnoArZKC"; 
#else
    char auth[] = "ya1T2eipkMhB3NvyLeAyRVRHqPAUXUG-";  //BEK2
#endif

                                      


//char auth[] = "D4AU1HexWcErQ9vtpkP_EgocpnoArZKC"; // bek2 "ya1T2eipkMhB3NvyLeAyRVRHqPAUXUG-"
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;
int _t433ChNumber, _t315ChNumber,_blynkfreqValue,_sevenSeg;
int _otaBlynk=0;
int _bootBlynk=0;
int _fbonBlynk =0;
int _smsBlynk=0;
bool _blynkEvent = true;
int  _blynkData=0;
int  _blynkEventID =0;
int _tempoVar;

BlynkTimer timer;



BLYNK_WRITE(V0)  //freq
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_FREQ_ID;
  DEBUG_PRINT("V0 Freq: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}


BLYNK_WRITE(V1) //rc433
{
  _t433ChNumber = param.asInt(); // assigning incoming value from pin V1 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V1 T433: ");
  DEBUG_PRINTLN(_t433ChNumber);
}

BLYNK_WRITE(V2) // receiver ch
{
  _sevenSeg= param.asInt(); // assigning incoming value from pin V2 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V2 Seven Segments: ");
  DEBUG_PRINTLN(_sevenSeg);
}

BLYNK_WRITE(V3) // ROOM_201_TO_205
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_201_TO_205;
  DEBUG_PRINT("V3 ROOM_201_TO_205: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V3, "color", BLYNK_GREEN);
}

BLYNK_WRITE(V7)   // ota
{
  _otaBlynk = param.asInt(); // assigning incoming value from pin V7 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_OTA_ID;
  DEBUG_PRINT("V7 Ota Gsm: ");
  DEBUG_PRINTLN(_otaBlynk ? F("On") : F("Off"));
}

BLYNK_WRITE(V8)   //boot
{
  _bootBlynk = param.asInt(); // assigning incoming value from pin V7 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_RESET_ID;
  DEBUG_PRINT("V8 reboot: ");
  DEBUG_PRINTLN(_bootBlynk ? F("On") : F("Off"));
}

BLYNK_WRITE(V10)  //Send to blynk
{
  _fbonBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_SEND_TO_BLYNK_ID;
  DEBUG_PRINT("V10 Send To Blynk: ");
  DEBUG_PRINTLN(_fbonBlynk ? F("Turn On") : F("Turn Off"));
}


BLYNK_WRITE(V11)  //sms on off
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_SMS_ON_ID;
  DEBUG_PRINT("V11 Sms: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}

BLYNK_WRITE(V14) //rc315
{
  _t315ChNumber = param.asInt(); // assigning incoming value from pin V14 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_T315_CH_NR_ID;
  DEBUG_PRINT("V14 T315: ");
  DEBUG_PRINTLN(_t433ChNumber);
}



BLYNK_WRITE(V15)   //NETGEER 
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_NETGEER_ID ;
  DEBUG_PRINT("V15 NETGEER: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V16) // ROOM_206_TO_210
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_206_TO_210;
  DEBUG_PRINT("V16 ROOM_206_TO_210: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V16, "color", BLYNK_GREEN);
}


BLYNK_WRITE(V17) // ROOM_211_TO_215
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_211_TO_215;
  DEBUG_PRINT("V17 ROOM_211_TO_215: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V17, "color", BLYNK_GREEN);
}

BLYNK_WRITE(V18) // ROOM_216_TO_220
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_216_TO_220;
  DEBUG_PRINT("V18 ROOM_216_TO_220: ");
  DEBUG_PRINTLN(_sevenSeg);
  Blynk.setProperty(V18, "color", BLYNK_GREEN);
}


BLYNK_WRITE(V19) // ROOM_AV RC
{
  _sevenSeg= param.asInt();  
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =ROOM_AV_RC;
  DEBUG_PRINT("V19 ROOM_AV_RC: ");
  DEBUG_PRINTLN(_sevenSeg);
  if (_blynkData ==1) Blynk.setProperty(V19, "color", BLYNK_GREEN);
  if (_blynkData ==2) Blynk.setProperty(V19, "color", BLYNK_YELLOW);
  if (_blynkData ==3) Blynk.setProperty(V19, "color", BLYNK_RED);
}

BLYNK_WRITE(V21)   //ch1
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V21 to a variable
    _blynkEvent = true;
    _blynkData=1;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V21 ch1: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V22)   //ch2
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V22 to a variable
    _blynkEvent = true;
    _blynkData=2;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V22 ch3: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V23)   //ch3
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V23 to a variable
    _blynkEvent = true;
    _blynkData=3;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V23 ch3: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V24)   //ch4
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V24 to a variable
    _blynkEvent = true;
    _blynkData=4;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V24 ch4: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V25)   //ch5
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V25 to a variable
    _blynkEvent = true;
    _blynkData=5;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V25 ch5: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V26)   //ch6
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V26 to a variable
    _blynkEvent = true;
    _blynkData=6;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V26 ch6: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V27)   //ch7
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V27 to a variable
    _blynkEvent = true;
    _blynkData=7;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V27 ch7: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V28)   //ch8
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V28 to a variable
    _blynkEvent = true;
    _blynkData=8;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V28 ch8: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}



BLYNK_WRITE(V29)   //ch15
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=15;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V29 T433 ch14: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V30)   //ch0
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V28 to a variable
    _blynkEvent = true;
    _blynkData=9;
    _blynkEventID =FB_AV_7SEG_ID;
  DEBUG_PRINT("V30 ch9 995 Mhz: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V31)   //ch1
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V31 to a variable
    _blynkEvent = true;
    _blynkData=1;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V31 T433 ch1: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V32)   //ch2
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V32 to a variable
    _blynkEvent = true;
    _blynkData=2;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V32 T433 ch2: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V34)   //ch4
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V34 to a variable
    _blynkEvent = true;
    _blynkData=4;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V34 T433 ch4: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V35)   //ch5
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V35 to a variable
    _blynkEvent = true;
    _blynkData=5;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V35 T433 ch5: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V37)   //ch7
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=7;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V37 T433 ch7: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V38)   //ch8
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V38 to a variable
    _blynkEvent = true;
    _blynkData=8;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V38 T433 ch8: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V39)   //ch9
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=9;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V39 T433 ch9: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V40)   //ch10
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=10;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V40 T433 ch10: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V44)   //ch14
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=14;
    _blynkEventID =FB_T433_CH_NR_ID;
  DEBUG_PRINT("V44 T433 ch14: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}




BLYNK_WRITE(V52)   //ch2
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=17;
    _blynkEventID =FB_T315_CH_NR_ID;
  DEBUG_PRINT("V52 T315 ch2: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V53)   //ch3
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=18;
    _blynkEventID =FB_T315_CH_NR_ID;
  DEBUG_PRINT("V53 T315 ch3: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V54)   //ch4
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=19;
    _blynkEventID =FB_T315_CH_NR_ID;
  DEBUG_PRINT("V54 T315 ch4: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V63)   //ch13
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=28;
    _blynkEventID =FB_T315_CH_NR_ID;
  DEBUG_PRINT("V63 T315 ch13: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V64)   //ch14
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V37 to a variable
    _blynkEvent = true;
    _blynkData=29;
    _blynkEventID =FB_T315_CH_NR_ID;
  DEBUG_PRINT("V64 T315 ch14: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}




BLYNK_WRITE(V71)   //Zapping On Off Switch
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_ID;
  DEBUG_PRINT("V71 Zap: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V72)   //Zapping Timer
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_TIMER_ID;
  DEBUG_PRINT("V712 Zap Timer: ");
  DEBUG_PRINTLN(_tempoVar );
}

BLYNK_WRITE(V81)   //Zapping ch1
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID1;
  DEBUG_PRINT("V81 Zap ch1: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V81, "color", BLYNK_GREEN);
  else Blynk.setProperty(V81, "color", BLYNK_RED);
}

BLYNK_WRITE(V82)   //Zapping ch2
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID2;
  DEBUG_PRINT("V82 Zap ch2: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V82, "color", BLYNK_GREEN);
  else Blynk.setProperty(V82, "color", BLYNK_RED);
}
BLYNK_WRITE(V83)   //Zapping ch3
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID3;
  DEBUG_PRINT("V83 Zap ch3: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V83, "color", BLYNK_GREEN);
  else Blynk.setProperty(V83, "color", BLYNK_RED);  
}
BLYNK_WRITE(V84)   //Zapping ch4
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID4;
  DEBUG_PRINT("V84 Zap ch4: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V84, "color", BLYNK_GREEN);
  else Blynk.setProperty(V84, "color", BLYNK_RED);  
}
BLYNK_WRITE(V85)   //Zapping ch5
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID5;
  DEBUG_PRINT("V85 Zap ch5: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V85, "color", BLYNK_GREEN);
  else Blynk.setProperty(V85, "color", BLYNK_RED);  
}
BLYNK_WRITE(V86)   //Zapping ch6
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID6;
  DEBUG_PRINT("V86 Zap ch6: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V86, "color", BLYNK_GREEN);
  else Blynk.setProperty(V86, "color", BLYNK_RED);
}
BLYNK_WRITE(V87)   //Zapping ch7
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID7;
  DEBUG_PRINT("V87 Zap ch7: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V87, "color", BLYNK_GREEN);
  else Blynk.setProperty(V87, "color", BLYNK_RED);
}
BLYNK_WRITE(V88)   //Zapping ch8
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V71 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_ZAP_CHANNEL_ID8;
  DEBUG_PRINT("V88 Zap ch8: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
  if(_blynkData) Blynk.setProperty(V88, "color", BLYNK_GREEN);
  else Blynk.setProperty(V88, "color", BLYNK_RED);  
}

BLYNK_WRITE(V90)   //CH +
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_CH_PLUS_ID  ;
  DEBUG_PRINT("CH +: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V91)   //CH -
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_CH_MINUS_ID  ;
  DEBUG_PRINT("CH -: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V92)   //FR -
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_FR_MINUS_ID  ;
  DEBUG_PRINT("FR -: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}

BLYNK_WRITE(V93)   //FR +
{
  _tempoVar = param.asInt(); // assigning incoming value from pin V15 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_AV_FR_PLUS_ID  ;
  DEBUG_PRINT("FR +: ");
  DEBUG_PRINTLN(_tempoVar ? F("On") : F("Off"));
}


BLYNK_WRITE(V98)  //pulse
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_RC_PULSE_ID;
  DEBUG_PRINT("RC PULSE: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}


BLYNK_WRITE(V100)  //wifi ide
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_WIFI_IDE_ID ;
  DEBUG_PRINT("Wifi IDE: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}


BLYNK_WRITE(V101)  //repetion
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_RC_REPETION_ID;
  DEBUG_PRINT("RC Repetion: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}


BLYNK_WRITE(V102)  //sleep timer
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_SLEEP_TIMER_ID;
  DEBUG_PRINT("Sleep Timer: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}

BLYNK_WRITE(V104)  //wifi WEB
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_WIFI_WEB_ID ;
  DEBUG_PRINT("Wifi WEB: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}


BLYNK_WRITE(V105)  //wifi ota github
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_WIFI_OTA_ID ;
  DEBUG_PRINT("Wifi Ota Github: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}


blynk::blynk(void) 
{

}



void blynk::init() 
{
 //Blynk.begin(auth, ssid, pass); 
 Blynk.config(auth);
 Blynk.connect();
// timer.setInterval(10000L, connection_check);
 led1.on(); //Enable colours for Alive Led BEK
 led8.on(); //Enable colours for Alive Led BEK
 led2.on(); //Enable colours for Ack Led
 led3.on(); //Enable colours for T433 St Led
 led6.on(); //Enable colours for T315 St Led
 led4.on(); //Enable colours for firebase
 led5.on(); //Enable colours for firebase
 led7.on(); //Enable colours for Zapping
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
  if (sendToBlynkLeds)
    {
      if (_data==1)  led2.setColor(BLYNK_RED);
      else           led2.setColor(BLYNK_GREEN);
    }  
}


void blynk::blynkRCLed(bool _data)
{
    if (sendToBlynkLeds)
    {
      if (_data==0)  led3.setColor(BLYNK_RED);
      else           led3.setColor(BLYNK_GREEN);
    }
}

void blynk::blynkRCLed315(bool _data)
{
    if (sendToBlynkLeds)
    {
      if (_data==0)  led6.setColor(BLYNK_RED);
      else           led6.setColor(BLYNK_GREEN);
    }  
}

void blynk::blynkSmsLed(bool _data)
{
 if (_data==0)  led5.setColor(BLYNK_RED);
 else           led5.setColor(BLYNK_YELLOW);
}

void blynk::zapLed(bool _data)
{
 if (_data==0)  led7.setColor(BLYNK_RED);
 else           led7.setColor(BLYNK_GREEN);
}

void blynk::sendToBlynkLed(bool _data)
{
 if (_data==0)  led4.setColor(BLYNK_RED);
 else           led4.setColor(BLYNK_GREEN);
}

void blynk::blynkFirebaseLed(bool _data)
{
}

void blynk::sendAlive(int _data)
{
if (sendToBlynk)
{
#ifdef BEK
     if (_data==0)  led1.setColor(BLYNK_RED); 
 else           led1.setColor(BLYNK_GREEN);

#else
     if (_data==0)  led8.setColor(BLYNK_RED); 
 else           led8.setColor(BLYNK_YELLOW);
#endif
}
}

/***************************************************/
void blynk::resetT433Cmd(int cmd)
{
  if (sendToBlynk)
  {
  t433ChNumber = cmd;
 Blynk.virtualWrite(V1, cmd);
  }
}

void blynk::resetT315Cmd(int cmd)
{
  if (sendToBlynk)
  {
  t315ChNumber = cmd;
 Blynk.virtualWrite(V14, cmd);
  }
}

void blynk::sevenSegValue(int freq )
{
  if (sendToBlynk)
  {
  sevenSeg = freq;
 Blynk.virtualWrite(V2, freq);
  }
}


void blynk::frequencyValue(int freq )
{
  if (sendToBlynk)
  {
  frequency = freq;
  Blynk.virtualWrite(V0, freq);
  }
}

void blynk::sendRsss(int _rsss)
{
}

void blynk::sendAvRxIndex(int _index)
{
  Blynk.virtualWrite(V99, _index);
}
