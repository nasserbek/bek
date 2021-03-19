#include "FB.h"
#include "headers.h"

#define FIREBASE_HOST "bekiot.firebaseio.com"                     //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "Zu8TzSH0rAq9NREWBbsW7tgLh6mQ9AWJIUQAoSJp"       //Your Firebase Database Secret goes here


#include <FirebaseESP32.h>
FirebaseData firebaseData;

extern EventGroupHandle_t g_event_group;
extern QueueHandle_t g_event_queue_handle;

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
    if (firebaseData1.dataType() == "int")
      {
        _firebaseEvent = true;
        DEBUG_PRINTLN(firebaseData1.intData());
        _eventValue = firebaseData1.intData();
      }
    else if (firebaseData1.dataType() == "string") 
      {
        _firebaseEvent = true;
        DEBUG_PRINTLN(firebaseData1.stringData());
        _eventValue = stringToInt(firebaseData1.stringData() );
      }
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN();

    _eventPath = firebaseData1.dataPath();
  
    if (_eventPath == FB_VIDEO_CH_PATH )     _eventID = Q_EVENT_VIDEO_CH_V2;
    if (_eventPath == FB_FREQ )        _eventID = Q_EVENT_FREQ_V0;
    if (_eventPath == FB_T433_CH_NR)   _eventID = Q_EVENT_RC_CH_NR_V1;
    if (_eventPath == FB_RESET)        _eventID = Q_EVENT_REBOOT_V8;
    if (_eventPath == FB_OTA_GITHUB)          _eventID = Q_EVENT_OTA_GITHUB_V105;
    if (_eventPath == IDLE_PATH)       _eventID = IDLE_PATH_ID;
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




int fireBase::getInteger( String _path)
{
 String sValue;
   if ( Firebase.getString(firebaseData,_path ,  sValue) ) return (stringToInt(sValue) );
   else 
   {
    DEBUG_PRINTLN(_path);
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN("Can't read stream data...");
    DEBUG_PRINTLN("REASON: " + firebaseData.errorReason());
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN();
    return -1;
    }

}

bool fireBase::SendString(String _path,String _cmdString)
{
   if (Firebase.setString(firebaseData,_path, _cmdString))return true;
    else 
    {
    DEBUG_PRINTLN(_path);
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN("Can't read stream data...");
    DEBUG_PRINTLN("REASON: " + firebaseData.errorReason());
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN();
    return false;
    }
}


bool fireBase::SendInt(String _path,int _cmdInt)
{
   if (Firebase.setInt(firebaseData,_path, _cmdInt))return true;
    else 
    {
    DEBUG_PRINTLN(_path);
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN("Can't read stream data...");
    DEBUG_PRINTLN("REASON: " + firebaseData.errorReason());
    DEBUG_PRINTLN("------------------------------------");
    DEBUG_PRINTLN();
    return false;
    }
}

bool fireBase::endTheOpenStream(void)
{
Firebase.end(firebaseData);
}


  
