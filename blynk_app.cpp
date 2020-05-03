#include "blynk_app.h"
#include "headers.h"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>

#include <BlynkSimpleEsp32.h>



WidgetLED led1(V4);   //Alive Led
WidgetLED led2(V5);   //Ack
WidgetLED led3(V6);   //T433 St
WidgetLED led4(V9);   //fb
WidgetLED led5(V12);   //sms

char auth[] = "D4AU1HexWcErQ9vtpkP_EgocpnoArZKC";
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;
int _t433ChNumber, _blynkfreqValue,_sevenSeg;
int _otaBlynk=0;
int _bootBlynk=0;
int _fbonBlynk =0;
int _smsBlynk=0;
bool _blynkEvent = true;
int  _blynkData=0;
int  _blynkEventID =0;

BLYNK_WRITE(V11)  //sms on off
{
    _smsBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_SMS_ON_ID;
  DEBUG_PRINT("V11 Sms: ");
  DEBUG_PRINTLN(_smsBlynk ? F("Turn On") : F("Turn Off"));
}

BLYNK_WRITE(V10)  //firebase on off
{
  _fbonBlynk = param.asInt(); // assigning incoming value from pin V10 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_FB_OFF_ID;
  DEBUG_PRINT("V10 Firebase: ");
  DEBUG_PRINTLN(_fbonBlynk ? F("Turn On") : F("Turn Off"));
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

BLYNK_WRITE(V0)  //freq
{
  _blynkfreqValue = param.asInt(); // assigning incoming value from pin V0 to a variable
    _blynkEvent = true;
    _blynkData=param.asInt();
    _blynkEventID =FB_FREQ_ID;
  DEBUG_PRINT("V0 Freq: ");
  DEBUG_PRINTLN(_blynkfreqValue);
}

BLYNK_WRITE(V1) //rc
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

BLYNK::BLYNK(void) 
{

}



void BLYNK::init() 
{
 Blynk.begin(auth, ssid, pass); 
 led1.on(); //Enable colours for Alive Led
 led2.on(); //Enable colours for Ack Led
 led3.on(); //Enable colours for T433 St Led
 led4.on(); //Enable colours for firebase
 led5.on(); //Enable colours for firebase
}

void BLYNK::notifierDebug(String subject, String body)
{
  // *** WARNING: You are limited to send ONLY ONE E-MAIL PER 5 SECONDS! ***
 //   body = String("You pushed the button ") + count + " times.";
//    DEBUG_PRINT(subject);DEBUG_PRINTLN(body); 
 //   Blynk.notify(String(subject +"**"+ body) );
}

bool BLYNK::getData()
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

void BLYNK::blynkRun()
{
  Blynk.run(); 
}

void BLYNK::sendAlive(int _data)
{
 if (_data==0)  led1.setColor(BLYNK_RED); 
 else           led1.setColor(BLYNK_GREEN);
}


void BLYNK::blynkAckLed(bool _data)
{
 if (_data==1)  led2.setColor(BLYNK_RED);
 else           led2.setColor(BLYNK_GREEN);
}


void BLYNK::blynkRCLed(bool _data)
{
 if (_data==0)  led3.setColor(BLYNK_RED);
 else           led3.setColor(BLYNK_GREEN);
}


void BLYNK::blynkFirebaseLed(bool _data)
{
 if (_data==0)  led4.setColor(BLYNK_RED);
 else           led4.setColor(BLYNK_GREEN);
}

void BLYNK::blynkSmsLed(bool _data)
{
 if (_data==0)  led5.setColor(BLYNK_RED);
 else           led5.setColor(BLYNK_GREEN);
}
void BLYNK::resetT433Cmd(int cmd)
{
  t433ChNumber = cmd;
 Blynk.virtualWrite(V1, cmd);
}

void BLYNK::sevenSegValue(int freq )
{
  sevenSeg = freq;
 Blynk.virtualWrite(V2, freq);
}

void BLYNK::sendRsss(int _rsss)
{
  Blynk.virtualWrite(V3, _rsss); 
}
