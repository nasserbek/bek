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

bool autoResetRouter = 0;
int activeBoard   = ESP1;
int selectedBoard = ESP1;
bool liveLed = false;
bool liveLedUpdate = false;
bool internetLossUpdate = false;
bool sendVerWifi = false;
bool DvrChOn = false;
int LiveSec = 0;
int LiveMin = 0;
int LiveHour = 0;
bool RC_Api = false;

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
bool streamWebDdns = DDNS;
bool routerResetStart =false;
bool RC_Remote_CSR1 = false;
bool RC_Remote_CSR2 = false;
bool RC_Remote_CSR3 = false;

bool V_Remote_CSR1 = false;
bool V_Remote_CSR2 = false;
bool V_Remote_CSR3 = false;

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



//FREQ CSR IN ORDER ROOM NR
const uint freqTable[21] =       {0, 1180,    1360    ,  1240,     1280,       1320,      1200,        1360,       1080,     1280,             1120,        1320,       1200,          1120 ,      1160,        1080   ,    1240,         1280,        1360 ,     1160,     1220}; 
//                                  24        25-48-67   26-65     27-66-50    28-52      29-53        48-25-67     49-64    50-27-66          51-62        52-28       53-29          62-51       63-68        64-49       65-26         66-27-50     67-25-48   68-63       12                   Xxx

//RC CSR IN ORDER ROOM NR
//20042024 const unsigned long CH_433[35] ={0, 349649,   349811,  349491,    349500,   349635,  349644,      349680,  349443,   349211,    349452  , 349463,   349652 ,        349695,   349488,    349632,   349511,  349455,  349111, 349311 }; 
 //                                 24        25        26        27        28       29           48        49        50        51        52        53              62        63         64        65        66       67     68

//RC CSR IN ORDER ROOM NR
const unsigned long CH_433[35] ={0, 349649,   349811,  349491,    349500,   349635,  349644,      349680,   349111 ,   349211,    349452  , 349463,   349652 ,        349695,   349488,    349632,   349511,  349455, 349443 , 349423 ,  349311}; 
 //                                 24        25        26        27        28       29           48        49         50        51         52        53              62        63         64        65        66     ROUTER   68         spare




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
