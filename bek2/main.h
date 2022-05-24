#include <EEPROM.h>
#include <Wire.h>
#include <RCSwitch.h>


#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP32Ping.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"


#include "gsm_ota.h"
#include "esp_system.h"
//#include "time.h"
#include "av.h"
#include "sim.h"
#include "FB.h"

#include "blynk_app.h"

#include "headers.h"

struct Roomdata
{
  int  frequency;
  int  pll;
  byte vCh;
  byte rCh;
  bool selected = false;
};

struct Roomdata roomId[20];

struct Channels
{
  int  frequency;
  int  id;
  bool zap=false;
  bool mux=false;
  long  muxTimer;
};

struct Channels videoCh[20];

uint _pll[20];

#ifdef SDB
const uint freqTable[20] =      {0,1060,    1220,     1260,     1280,     1120,     1320,     1080,     1360,     1200,     1100,     1160,     1240,     1200,     1160,     1120,     1080,     1140,     1180,     1220}; //24052022 switched channels
//                                  44      45        46        47        48*       49        50        51        52        53        03        11        20        21        22        27        37        38        XX
const unsigned long CH_433[32] ={0,4674831, 4675071,  4675008,  4674876,  7722288,  4674879,  4675015,  4675056,  7722243,  4675011,  7722252,  4675023,  4675020,  4674864,  4674828,  4674819,  4675068,  4675059,  7722444,7722447,7722300,7722303,7722480,7722435,7722255,7722492,7722483,7722495,7722432,7722291,0}; 

#else
const uint freqTable[20] =      {0,1080,    1120,     1160,     1200,     1240,     1280,     1320,    1360,    1100,     1060,     1140,     1180,     1220,     1260,     1080,     1200,     1160,     1120, 1240};
//                                  27      22        21        20        11        47       49        51       53        44        37        38        45        46        50        52        03        X       XX
const unsigned long CH_433[32] ={0,4674819, 4674828,  4674864,  4675020,  4675023,  4674876, 4674879,  4675056, 4675011,  4674831,  4675068,  4675059,  4675071,  4675008,  4675015,  7722243,  7722252,7 722288,7722444,7722447,7722300,7722303,7722480,7722435,7722255,7722492,7722483,7722495,7722432,7722291,0};
#endif

/*
1     16    27
2     15    22
3     14    21
4     13    20
5     12    11

6     4   47
18*   5   48
7     6   49
8     8   51
9     10  53
10    1   44
11    17  37
12    18  38
13    2   45
14    3   46
15    7   50
16    9   52
17    11  03


 * INDEX    
 * 1 1080    4674819   27        03      17      1
 * 2 1120    4674828   22        11      5       2
 * 3 1160    4674864   21        20      4       3
 * 4 1200    4675020   20        21      3       4
 * 5 1240    4675023   11        22      2       5
 * 
 * 6  1280   4674876   47        27      1       6
 * 7  1320   4674879   49        37      11      7
 * 8  1360   4675056   51        38      12      8
 * 9  1100   4675011   53        44      10      9
 * 10 1060   4674831   44        45      13      10
 * 
 * 11 1140   4675068   37        46      14      11
 * 12 1180   4675059   38        47      6       12
 * 13 1220   4675071   45        48*      *      13*
 * 14 1260   4675008   46        49      7       14
 * 15 1080   4675015   50        50      15      15
 * 
 * 16 1200   7722243   52        51      8       16
 * 17 1160   7722252   03        52      16      17
 * 18 1120   7722288   X         53      9       18
 * 19 1240   7722444   X         53      9       18
*/

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


/*********************************************** web upodater *********************************************/
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

const char* host = "esp32";
WebServer server(80);
/*
 * Login page
 */

const char* loginIndex = 
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";


 /*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";
/*********************************************** lcd fb *********************************************/
