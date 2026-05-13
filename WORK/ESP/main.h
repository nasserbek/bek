#include <Wire.h>
#include <RCSwitch.h>
#include <ESP32httpUpdate.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "esp_system.h"

#include "blynk_app.h"

#include "headers.h"
#include <Wire.h>
#include <WiFi.h>

 //AWS
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//OTA
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>



QueueHandle_t g_event_queue_handle = NULL;
EventGroupHandle_t g_event_group = NULL;

#define USE_SERIAL Serial

/********************* AWS MQTT BROKER *******************************************************/
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

int   MapIndex            = 0;
bool  PowerOnTune         = false;
bool  autoResetRouter     = 0;
int   ActiveBoard         = ESP1;
int   selectedBoard       = ESP1;
bool  liveLed             = false;
bool  liveLedUpdate       = false;
bool  internetLossUpdate  = false;
bool  sendVerWifi         = false;
bool  DvrChOn             = false;
int   LiveSec             = 0;
int   LiveMin             = 0;
int   LiveHour            = 0;
bool  RC_Api              = false;

bool lastAck = false;
bool awsConnected = false;
String resultS = "";  //Variable to store the MQTT input message
char Json[40];        //Variable to store the serialized Json
StaticJsonDocument<54> doc1; //Json to receive in

bool _nodeRedEvent  = false;
bool  nodeRedActive = false;
int  nodeRedData    = 0;
int  _nodeRedData    = 0;
int  nodeRedEventID =0;
int  _nodeRedEventID =0;
int   nodeRedeventdata;
int hmi = 0; //0 Blynk 1 Node red 
/********************* AWS MQTT BROKER *******************************************************/

bool zapScanOnly = false;
int lastSelectedCh = 1;
bool autoRemoteLocalRc = false;
bool catchCh = false;
bool zapSetup = false;
int previousCh =0;
int queuData;
int queuDataID;
bool queuValidData=false;
bool streamWebDdns = WEB;
bool routerResetStart =false;
bool RC_Remote_ESP1 = false;
bool RC_Remote_ESP2 = false;
bool RC_Remote_ESP3 = false;

bool V_Remote_ESP1 = false;
bool V_Remote_ESP2 = false;
bool V_Remote_ESP3 = false;

int  rcValue = 0;
int  selected_room = 1;  
int  selected_Rx = 0;
int  Av_Rx = 1;
int  recevierCh=1;
bool zapOnOff = false;

struct Roomdata
{
  int  frequency;
  int  pll;
  byte vCh;
  byte rCh;
  bool selected = false;
};

struct Roomdata roomId[21];

struct Channels
{
  int  frequency;
  int  id;
  bool zap=false;
  bool mux=false;
  long  muxTimer;
};

struct Channels videoCh[21];

uint _pll[21];


#define AV_CH1    1080  // 49 
#define AV_CH2    1120  // 51 
#define AV_CH3    1160  // 63 
#define AV_CH4    1200  // 25 
#define AV_CH5    1240  // 65 
#define AV_CH6    1280  // 64 
#define AV_CH7    1320  // 52 
#define AV_CH8    1360  // 48

#define AV_CH3B   1180  // 24 
#define AV_CH4B   1220  // XX 

#define RC_CH1    349111  // 49  
#define RC_CH2    349452  // 51 
#define RC_CH3    349488  // 63 
#define RC_CH4    349811  // 25 
#define RC_CH5    349511  // 65 
#define RC_CH6    349632  // 64 
#define RC_CH7    349463  // 52 
#define RC_CH8    349680  // 48

#define RC_CH9    349652  // 53  
#define RC_CH10   349211  // 50
#define RC_CH11   349649  // 24 
#define RC_CH12   349455  // 66 
#define RC_CH13   349695  // 62 
#define RC_CH14   349644  // 29
#define RC_CH15   349423  // 68 
#define RC_CH16   349635  // 28 
#define RC_CH17   349500  // 27 
#define RC_CH18   349491  // 26  
#define RC_CH19   349483  // xx 
#define RC_CH20   349411  // xx 

//FREQ 2025
const uint freqTable[21] =       {0, AV_CH1,   AV_CH2,   AV_CH3,   AV_CH4,   AV_CH5,   AV_CH6,   AV_CH7,   AV_CH8,   AV_CH1,   AV_CH2,   AV_CH3,   AV_CH4,   AV_CH5,  AV_CH6,  AV_CH7,  AV_CH8,    AV_CH6,   AV_CH8,  AV_CH3,  AV_CH4B}; 
//                                   49        51        63        25        65        64        52        48        53        50        24        66        62       29       68      66-27-50  67-25-48 68-63    Xxx

//RC ESP1 IN ORDER ROOM NR
const unsigned long CH_433[35] = {0, RC_CH1,   RC_CH2,   RC_CH3,   RC_CH4,   RC_CH5,   RC_CH6,   RC_CH7,   RC_CH8,   RC_CH9,   RC_CH10,  RC_CH11,  RC_CH12,  RC_CH13, RC_CH14, RC_CH15, RC_CH16,   RC_CH17,  RC_CH18, RC_CH19, RC_CH20}; 
 //                                  49        51        63        25        65        64        52        48        53        50        24        66        62       29       68       65         66        ROUTER   68       spare
//RC ESP1 IN ORDER ROOM NR
//const unsigned long CH_433[35] ={0, 349649,   349811,  349491,    349500,   349635,  349644,      349680,   349111 ,   349211,    349452  , 349463,   349652 ,        349695,   349488,    349632,   349511,  349455, 349443 , 349423 ,  349311}; 
 //                                 24        25        26        27        28       29           48        49         50        51         52        53              62        63         64        65        66     ROUTER   68         spare



//FREQ 2025
//const uint freqTable[21] =       {0, AV_CH3B,   AV_CH4,   AV_CH5,   AV_CH6,   AV_CH7,   AV_CH4,   AV_CH8,   AV_CH1,   AV_CH6,   AV_CH2,   AV_CH7,   AV_CH4,   AV_CH2,  AV_CH3,  AV_CH6,  AV_CH5,    AV_CH6,   AV_CH8,  AV_CH3,  AV_CH4B}; 
////                                  24          25-48-67  26-65     27-66-50  28-52     29-53     48-25-67  49-64     50-27-66  51-62     52-28     53-29     62-51    63-68    64-49    65-26      66-27-50  67-25-48 68-63    Xxx

//RC ESP1 IN ORDER ROOM NR
// const unsigned long CH_433[35] = {0, RC_CH1,    RC_CH2,   RC_CH3,   RC_CH4,   RC_CH5,   RC_CH6,   RC_CH7,   RC_CH8,   RC_CH9,   RC_CH10,  RC_CH11,  RC_CH12,  RC_CH13, RC_CH14, RC_CH15, RC_CH16,   RC_CH17,  RC_CH18, RC_CH19, RC_CH20}; 
 //                                  24            25        26        27        28        29        48        49        50        51        52        53        62       63       64       65         66        ROUTER   68       spare


//FREQ ESP1 IN ORDER ROOM NR
//const uint freqTable[21] =       {0, 1180,    1360    ,  1240,     1280,       1320,      1200,        1360,       1080,     1280,             1120,        1320,       1200,          1120 ,      1160,        1080   ,    1240,         1280,        1360 ,     1160,     1220}; 
//                                  24        25-48-67   26-65     27-66-50    28-52      29-53        48-25-67     49-64    50-27-66          51-62        52-28       53-29          62-51       63-68        64-49       65-26         66-27-50     67-25-48   68-63       12                   Xxx




bool ch1_on = false;
bool ch2_on = false;
bool ch3_on = false;
bool ch4_on = false;
bool ch5_on = false;
bool ch6_on = false;
bool ch7_on = false;
bool ch8_on = false;
bool ch9_on = false;
bool ch10_on = false;
bool ch11_on = false;
bool ch12_on = false;
bool ch13_on = false;
bool ch14_on = false;
bool ch15_on = false;
bool ch16_on = false;
bool ch17_on = false;
bool ch18_on = false;
bool ch19_on = false;
bool chx_on = false;
uint8_t bus = 1;



int blynkStatus = 1;
bool googleConnected=false;
bool blynkInitDone=false;
bool blynkConnected = false;
bool FBConnected = false;

bool sendToBlynk = false;
bool fireBaseOn =false;
bool blynkOn    =true;
bool wifiOn     =true;
bool smsOn      =true;

int ackTimer =  500;
int scanTimer = 5000;
int zapTimer = 10000;
int zapTimerOff = 5000;

int routerTimer = 5000;
long  routerResetTimer, resetNetgeerAfterInternetLossTimer,zaptime, zaptimeOff,scantime, AckTime, internetSurvilanceTimer, liveTimerOn,liveTimerOff,OtaTimeoutTimer,restartAfterResetNG,NetgeerResetGooglLostTimer,Router_24_hoursTimer,blynkNotActiveTimer;
bool pingGoogle= false;
bool googlePingOk= true;
bool netGeerReset = false;
bool liveBit = false;
IPAddress ip (192, 168, 0, 1); // The remote ip to ping
bool aliveTimout = false;
int stateMachine =0;
bool wifiIde = true;
bool wifiWebUpdater = true;
bool otaWifiGithub = true;
int repetionRC = 4;
int pulseRC = 300; //Default protocol 1

int deepSleepTimerHours = 1 ;
bool startLostInternetTimer = false;
bool InternetLoss = false;




boolean fbEvent = false;
boolean blynkEvent = false;
boolean smsEvent=false;

int recevierFreq=1080;
int remoteControlRcCh=0;
int rebootCmd=0;
int otaCmd=0;
int verCmd=0;
int smsOnOffCmd=0;
int firebaseOnOffCmd=0;
int blynkOnOffCmd=0;
int wifiOnOffCmd=0;





boolean old_fireBaseOn =true;
boolean old_blynkOn    =false;
boolean old_wifiOn    =false;
boolean old_smsOn    =false;


boolean otaBlynk=false;
boolean otaFirebase=false;
boolean otaSms=false;
boolean bootBlynk =false;
boolean fbonBlynk = false;
boolean fbonSms = false;
boolean fboffBlynk = false;
boolean fboffSms = false;

boolean sim800Available = false;
boolean old_sim800Available = false;
byte    errorCode = 0;

char smsSettings [] ="Wifi--->   \nFirebase--->   \nBlynk--->   \nSms--->   \nError code--->   \nSIM800--->   ";
bool firstRun=true;

int blynkT433ChNumber=0;
int blynkT315ChNumber=0;
int blynkSevenSeg=0;

boolean wifiAvailable = false;
hw_timer_t *_timer = NULL;
hw_timer_t *_timer1 = NULL;

int smsSendDelay = 30000;
bool smsSent = false;
unsigned long smsPrevMillis = 0;




#define EEPROM_SIZE 100
String overTheAirURL = "https://raw.githubusercontent.com/nasserbek/bek/master/bek.ino.ttgo-t1.bin";  // URL to download the firmware from
/************************************SMS ***********************************/

bool        smsFbStopt=true;
int         smsSwRestart=0;
int         smsWifiReconnect=0;
int         smsT433Cmd=0;
int         smsT315Cmd=0;
int         sms7Segments=0;
int         smsUpDown = 0;
int         smsFreq =0;
int         smsT433Cmd_RESET=0;
int         smsT315Cmd_RESET=0;
int         smsT433_Ch_Status=0;
int         smsT513_Ch_Status=0;
int         fbValue=0;
int         smsValue=0;
String smsReceived ="sms";
int         smsLen=0;

/************************************Alive Control ***********************************/
unsigned long alivePrevMillis = 0;
int aliveIntervalSec = 2;
bool aliveState = false;
bool aliveSent = false;

unsigned long prevMillisSms = 0;
int     intervalSms = 3000;
boolean smsProcess=false;
boolean smsProcessed=false;
bool    softwareReset=false;

/************************************ AV ***********************************/
int upDn=0;
int manual_freq = 0;
int old_manual_freq=0;
bool AV_Status;
bool DVR_Status;
int sevenSegmentsReading=1;
int blynksevenSegmentsReading =1;
int blynkFreq=0;
int blynkRC = 0;
int smssevenSegmentsReading =1;
int oldSegmentsValue=1;
int oldAVCmd =0;
int avOutput = 0;
int old_avOutput=0;

                  /************PCF8574 I2C ***********/
int av_pll_addr =0x61;  //0110 0 001 becomes 0xC2
int pcf8574CmdAddr = 0x20;          // PCF8574 device 1    Commands
int pcf8574StatusAddr = 0x21;          // PCF8574 device 2    7 Segment Display Reader
                /***************RC 433-315Mhz**************/
bool cmdSent = false;
int T433_Cmd =0;
int T315_Cmd =0;
int T433_Ch_Status[32];
int T315_Ch_Status[16];
int T433_St;
int T315_St;
RCSwitch mySwitch = RCSwitch();

void BoardDefines()
{
  if(ActiveBoard == ESP1)      //TTGO R64 SCATOLA 1CH TTGO
  {
      #define BOARD ESP1
      #define VERSION_ID " ESP1_1 - "
      #define BLYNK_AUTH_TOKEN                BLYNK_AUTH_TOKEN_ESP1 //ESP1
      #define THINGNAME "ESP1"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "esp1/sub/rc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "esp1/sub/video"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "esp1/sub/zap"
      #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "esp1/sub/rx"
      #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "esp1/sub/avrc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "esp1/sub/dvr"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "esp1/sub/reboot"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "esp1/sub/zapchanel"
      #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "esp1/sub/localWebOta"
      #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "esp1/sub/GitHubWebOta"
      #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "esp1/sub/ideOta"
      #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "esp1/sub/version"
      #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "esp1/sub/scan"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "esp1/sub/repeat"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "esp1/sub/preset"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "esp1/sub/zapauto"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "esp1/sub/zton"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "esp1/sub/ztoff" 
      #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "esp1/sub/live"
      #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "esp1/sub/blynk"
      #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "esp1/sub/terminal"
      #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin"  // URL to download the firmware from
  }
  
  
  if(ActiveBoard == ESP2)      // R65 SCATOLA 4CH ESP32S
  {
      #define BOARD ESP2
      #define VERSION_ID " V1.16 "
      #define BLYNK_AUTH_TOKEN                BLYNK_AUTH_TOKEN_ESP2 //ESP2
      #define THINGNAME "ESP2"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "esp2/sub/rc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "esp2/sub/video"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "esp2/sub/zap"
      #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "esp2/sub/rx"
      #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "esp2/sub/avrc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "esp2/sub/dvr"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "esp2/sub/reboot"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "esp2/sub/zapchanel"
      #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "esp2/sub/localWebOta"
      #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "esp2/sub/GitHubWebOta"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "esp2/sub/ideOta" 
      #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "esp2/sub/version"
      #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "esp2/sub/scan"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "esp2/sub/repeat"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "esp2/sub/preset"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "esp2/sub/zapauto"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "esp2/sub/zton"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "esp2/sub/ztoff" 
      #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "esp2/sub/live"
      #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "esp2/sub/blynk"
      #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "esp2/sub/terminal"
      #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin"  // URL to download the firmware from
  }
  
  
  
  if(ActiveBoard == ESP3)      //R66 SWAN CASE 2CH ESP32S
  {
      #define BOARD ESP3
      #define VERSION_ID " ESP1_1 - "
      #define BLYNK_AUTH_TOKEN                BLYNK_AUTH_TOKEN_ESP3 //ESP3
      #define THINGNAME "ESP3"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "esp3/sub/rc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "esp3/sub/video"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "esp3/sub/zap"
      #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "esp3/sub/rx"
      #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "esp3/sub/avrc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "esp3/sub/dvr"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "esp3/sub/reboot"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "esp3/sub/zapchanel"
      #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "esp3/sub/localWebOta"
      #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "esp3/sub/GitHubWebOta" 
      #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "esp3/sub/ideOta"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "esp3/sub/version" 
      #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "esp3/sub/scan"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "esp3/sub/repeat"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "esp3/sub/preset"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "esp3/sub/zapauto"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "esp3/sub/zton"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "esp3/sub/ztoff" 
      #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "esp3/sub/live"
      #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "esp3/sub/blynk"
      #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "esp3/sub/terminal"
      #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin"  // URL to download the firmware from
    }  
  
  
  if(ActiveBoard == ESP0)      //R66 SWAN CASE 2CH ESP32S
  {    
      #define VERSION_ID " TEST_2 - "
      #define BLYNK_AUTH_TOKEN                BLYNK_AUTH_TOKEN_ESP14
      #define THINGNAME "ESP14"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "test/sub/rc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "test/sub/video"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "test/sub/zap"
      #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "test/sub/rx"
      #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "test/sub/avrc"
      #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "test/sub/dvr"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "test/sub/reboot"
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "test/sub/zapchanel"
      #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "test/sub/localWebOta"
      #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "test/sub/GitHubWebOta"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "test/sub/ideOta" 
      #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "test/sub/version"
      #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "test/sub/scan"
      #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "test/sub/repeat"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "test/sub/preset"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "test/sub/zapauto"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "test/sub/zton"  
      #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "test/sub/ztoff"   
      #define AWS_IOT_SUBSCRIBE_TOPIC_LIVE   "test/sub/live"
      #define AWS_IOT_SUBSCRIBE_TOPIC_BLYNK   "test/sub/blynk"
      #define AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL   "test/sub/terminal"
      #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin"  // URL to download the firmware from
   } 

}
