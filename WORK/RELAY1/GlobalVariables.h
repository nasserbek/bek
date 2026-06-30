#include "headers.h"
#define BLYNK_PRINT Serial
 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Updater.h>
#include <ESP8266TimerInterrupt.h>
#include <Ticker.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "time.h"



struct RelayInfo
{
    uint32_t chipID;
    const char* token;
    byte relayNumber;
};

RelayInfo relayTable[] =
{
    {0x6A3F12, "GqtBGDTWvFUNqEyiKTLJKgxBkVPHW4Xn", 1},
    {0x72C4A8, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 2},
    {0x5D19EF, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 3},
    {0x123456, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 4},
    {0x234567, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 5},
    {0x345678, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 6},
    {0x456789, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 7},
    {0x56789A, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 8},
    {0x6789AB, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 9},
    {0x789ABC, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",10}
};
const char* blynkAuthToken = nullptr;
byte relayNumber = 0;

String BOARD            = "ESP01-R1";
String VERSION_ID       = " R1.0 ";
Ticker WDGTimer;
int g_wdtTimeout = 10000;   // ms
long  blynkAtiveTimer;

ESP8266WiFiMulti wifiMulti;
BlynkTimer timer;
WidgetLED RELAY_LED_V2(V2);   
WidgetTerminal terminal(V102);
ESP8266WebServer server(80);
const char* host = "esp8266";
unsigned int BlynkServerTimeout  =  5000;  //  5s server connection timeout (SCT)

bool relayState = false;
bool wifiAvailable = false;
bool  blynkActive = false;
bool _blynkIsConnected  = false;
bool  blynkConnected = false;
 int inactivityPowerOffTimer  ; //1 Hour;
 int inactivityRestartTimer  ; //10 Hours;
 int zapTimerSec;
 
bool _wifiIsConnected = false;

const char* BLYNK_AUTH_TOKEN_RELAY        =        "GqtBGDTWvFUNqEyiKTLJKgxBkVPHW4Xn";//RELAY1
//const char* blynkAuthToken;

const char* WIFI_SSID_SFR    = "SFR_BEK-23C0";
const char* WIFI_SSID_METEOR_PLS ="BEK_METEOR_2.4G";
const char* WIFI_SSID_BBOX   ="Bbox-Bek-2.4G" ;

const char* WIFI_PASSWORD_SFR     =  "ali09042010";
const char* WIFI_PASSWORD_METEOR_PLS  =  "Ali09042010_";
const char* WIFI_PASSWORD_BBOX    =  "Ali09042010_";

char ssid[] = "Bbox-Bek-2.4G";
char pass[] = "Ali09042010_";

IPAddress blynkLocalServer;
IPAddress BLYNK_SERVER_BBOX(192,168,1,195);
IPAddress BLYNK_SERVER_METEOR_SFR(192,168,1,168);
IPAddress BLYNK_SERVER_METEOR_ETH_PLS(192,168,1,194);

//QueueHandle_t g_event_queue_handle = NULL;
#define EVENT_QUEUE_SIZE 50

int g_event_queue[EVENT_QUEUE_SIZE];
volatile uint8_t g_queue_head = 0;
volatile uint8_t g_queue_tail = 0;
bool _blynkEvent = true;
int  _blynkData = 0;
int  _blynkEventID = 0;
int eventdata;
bool wifiWebUpdater = false;
uint32_t  routerResetTimer, resetNetgeerAfterInternetLossTimer,zaptime, zaptimeOff,scantime, AckTime, internetSurvilanceTimer, liveTimerOn,liveTimerOff,OtaTimeoutTimer,restartAfterResetNG,NetgeerResetGooglLostTimer,Router_24_hoursTimer,blynkNotActiveTimer;
uint32_t lastActivityTime = 0;
bool internetConnected = false;
//bool inactivityCtrl = MILLS;
int  blynkData = 0;
int  blynkEventID = 0;
bool liveLedUpdate = false;
bool queuValidData = false;
int queuData;
bool InternetLoss = false;
int queuDataID;
bool netGeerReset = false; 
bool routerResetStart  = false;
bool blynkEvent=false; 






/************************ LOCAL WIFI SERVER UPLOAD***************************/


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
        "<tr>"
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
/********************************************************************************************/


unsigned int LiveUpdateInterval =  15000;  //  15s function call frequency   (FCF)
unsigned int blynkIntervalInterval    = 10000;  // 10 s check Blyk and Wifi
bool liveLed = false;
bool otaWifiGithub= false;   
const char* gitHubURL        =  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/RELAY1/RELAY1.ino.generic.bin"  ;// URL to download the firmware from
bool rebootCmd = false;
