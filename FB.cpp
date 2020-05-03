#include "FB.h"
#include "headers.h"

#define FIREBASE_HOST "bekiot.firebaseio.com"                     //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "Zu8TzSH0rAq9NREWBbsW7tgLh6mQ9AWJIUQAoSJp"       //Your Firebase Database Secret goes here


#include <FirebaseESP32.h>
#include <WiFi.h>
FirebaseData firebaseData;
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
int _eventValue;
int _eventID=0;
String _eventPath, _eventTypeFb ;
bool _firebaseEvent = false;  
int stringToInt(String str)
{
char carray[5]; 
      str.toCharArray(carray, sizeof(carray));
      return ( atoi(carray));  
}


fireBase::fireBase( void) 
  {
  }

bool fireBase::init() 
{
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
      Firebase.reconnectWiFi(true);
      if (initStreams())return (true);
      else return false;
}


bool fireBase::initStreams()
{
  if (!Firebase.beginStream(firebaseData, CMD_PATH)){
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN("Can't begin stream connection...");
    DEBUG_PRINTLN("REASON: " + firebaseData.errorReason());
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN();
    return false;
  }
  else Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
  return true; 
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
    {
     DEBUG_PRINTLN("Stream timeout, resume streaming...");
     DEBUG_PRINTLN();
    }
}

void streamCallback(StreamData firebaseData1)
{
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN("Stream Data available...");
    DEBUG_PRINTLN("STREAM PATH: " + firebaseData1.streamPath());
    DEBUG_PRINTLN("EVENT PATH: " + firebaseData1.dataPath());
    DEBUG_PRINTLN("DATA TYPE: " + firebaseData1.dataType());
    DEBUG_PRINTLN("EVENT TYPE: " + firebaseData1.eventType());
    DEBUG_PRINT("VALUE: ");
    if (firebaseData1.dataType() == "int"){_firebaseEvent = true;DEBUG_PRINTLN(firebaseData1.intData());_eventValue = firebaseData1.intData();}
    else if (firebaseData1.dataType() == "string") {_firebaseEvent = true;DEBUG_PRINTLN(firebaseData1.stringData());_eventValue = stringToInt(firebaseData1.stringData() );}
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN();

    _eventPath = firebaseData1.dataPath();
  
    if (_eventPath == FB_AV_7SEG )     _eventID = FB_AV_7SEG_ID;
    if (_eventPath == FB_FREQ )        _eventID = FB_FREQ_ID;
    if (_eventPath == FB_T433_CH_NR)   _eventID = FB_T433_CH_NR_ID ;
    if (_eventPath == FB_RESET)        _eventID = FB_RESET_ID;
    if (_eventPath == FB_OTA)          _eventID = FB_OTA_ID;
    if (_eventPath == FB_SMS_ON)       _eventID = FB_SMS_ON_ID;
    if (_eventPath == FB_VERSION)      _eventID = FB_VERSION_ID;
    if (_eventPath == FB_FB_OFF)       _eventID = FB_FB_OFF_ID;
    if (_eventPath == FB_BLYNK_ON_OFF) _eventID = FB_BLYNK_ON_OFF_ID;
    if (_eventPath == FB_WIFI_OFF)     _eventID = FB_WIFI_OFF_ID;
    if (_eventPath == FB_SETTINGS)     _eventID = FB_SETTINGS_ID;
    
 }


bool fireBase::firebaseRun(void)
{
    if (_firebaseEvent)
    {
      eventID =_eventID ;
      eventValue=_eventValue;
      _firebaseEvent = false;
      return true;
    }  
    else return false;
}

bool fireBase::wifiConnect()
  {
    if (WiFi.status()  == WL_CONNECTED )return true; 
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    long timeout = millis();
    while ( WiFi.status()  != WL_CONNECTED ) {
     if (millis() - timeout > 10000L) return false;
      }
return true; 
}

int fireBase::getInteger( String _path)
{
 String sValue;
   if ( Firebase.getString(firebaseData,_path ,  sValue) ) return (stringToInt(sValue) );
   else 
   {
    DEBUG_PRINT("Firebase Get error: "); DEBUG_PRINTLN(_path);
    if (WiFi.status() != WL_CONNECTED) wifiConnect();
    return -1;
    }

}

bool fireBase::SendString(String _path,String _cmdString)
{
   if (Firebase.setString(firebaseData,_path, _cmdString))return true;
    else 
    {
        DEBUG_PRINT("Firebase Send error: ");DEBUG_PRINTLN(_path);    
        if (WiFi.status() != WL_CONNECTED) wifiConnect();
        return false;
    }
}


bool fireBase::SendInt(String _path,int _cmdInt)
{
   if (Firebase.setInt(firebaseData,_path, _cmdInt))return true;
    else 
    {
        DEBUG_PRINT("Firebase Send error: ");DEBUG_PRINTLN(_path);    
        if (WiFi.status() != WL_CONNECTED) wifiConnect();
        return false;
    }
}

bool fireBase::endTheOpenStream(void)
{
Firebase.end(firebaseData);
}

  
