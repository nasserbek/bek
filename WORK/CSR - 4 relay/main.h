#include <Wire.h>
#include <RCSwitch.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "esp_system.h"

//#include "av.h"
#include "blynk_app.h"
#include "headers.h"
#include <Wire.h>

 
QueueHandle_t g_event_queue_handle = NULL;
EventGroupHandle_t g_event_group = NULL;

int queuData;
int queuDataID;
bool queuValidData=false;
bool streamWebDdns = DDNS;
bool routerResetStart =false;


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

//FREQ CSR IN ORDER ID
//const uint freqTable[21] =      {0,1080,     1120,     1160,     1200,    1240,    1280,    1320,     1360,     1060,    1100,     1140,     1180,     1080,     1120,     1300,     1340,         1060,      1100,     1140,     1180}; 
//                                  11        10        63        29        65       27      52        48        28        66        17        24        62        64       XX         22        37         EX20      XX         Xxx

//FREQ CSR IN ORDER ROOM NR
const uint freqTable[21] =      {0,1120,     1080,     1200,     1180,    1280,    1060,    1200,     1360,     1320,     1120 ,     1160,     1080   ,  1240,     1280,     1160,     1360,         1240,      1320,     1140,     1180}; 
//                                  51       49        53        24        27       28      29        48        52        62        63        64        65        66        68         25           26          50      XX         Xxx
// 

// OLD RC const unsigned long CH_433[32] =  {0,4674831,4675071,  4675008,  4674876,  7722288,  4674879,  4675015,  4675056,  7722243,  4675011,  4674864,  4675023, 7722252, 4674819, 4675059,   4674828,  4675068,    4675020, 7722444,  7722447,7722300,7722303,7722480,7722435,7722255,7722492,7722483,7722495,7722432,7722291,0}; 

//RC CSR IN ORDER ID
//const unsigned long CH_433[32] ={0, 349443,   349452,   349488,   349644,   349511,  349500, 349463,   349680,   349635,   349455,   349652,   349649,   349695,  349632,    349491}; 
//                                  11        10        63        29        65       27      52        48        28        66        17        24        62        64         XX         22        37         EX20      XX         Xxx

//RC CSR IN ORDER ROOM NR
const unsigned long CH_433[35] ={0, 349452,   349443,  349652,    349649,   349500,  349635, 349644,  349680,   349463,    349695  , 349488,   349632 ,  349511,   349455,    349311,   349811,  349491,  349211}; 
//                                  49        51        63        29        65       27      52        48        28        66        53        24        62        64         68         25        26       50  EX20      XX         Xxx
// 



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

uint8_t bus = 1;
  
int selected_Rx = 1;
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

int zapTimer = 5000;
int routerTimer = 5000;
long  routerResetTimer, resetNetgeerAfterInternetLossTimer,zaptime, Sms_24_hoursTimer, internetSurvilanceTimer, liveTimerOn,liveTimerOff,wifiIDETimer,restartAfterResetNG,NetgeerResetGooglLostTimer,blynkNotActiveTimer;
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
int Av_Rx = 1;
int deepSleepTimerHours = 1 ;
bool startLostInternetTimer = false;
bool InternetLoss = false;

bool zapOnOff = false;


boolean fbEvent = false;
boolean blynkEvent = false;
boolean smsEvent=false;
int recevierCh=1;
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
