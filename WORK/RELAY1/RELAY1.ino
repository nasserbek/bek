#define BLYNK_PRINT Serial
 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFiMulti.h>
long  blynkAtiveTimer;

ESP8266WiFiMulti wifiMulti;

WidgetLED RELAY_LED_V2(V2);   
WidgetTerminal terminal(V3);

#define RELAY_PIN 0
#define WiFi_TIMEOUT 30000  // 15sec Wifi connection timeout
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
char auth[] = "4acmqJCbVBLr8_liazx69mWNbF2hjNtw";//ESP8266_ESP01_1
const char* BLYNK_AUTH_TOKEN_ESP8266_ESP01_1        =        "4acmqJCbVBLr8_liazx69mWNbF2hjNtw";//ESP8266_ESP01_1
const char* BLYNK_AUTH_TOKEN;
const char* WIFI_SSID_SFR    = "SFR_BEK-23C0";
const char* WIFI_SSID_METEOR_PLS ="BEK_METEOR_2.4G";
const char* WIFI_SSID_BBOX   ="Bbox-Bek-2.4G" ;

const char* WIFI_PASSWORD_SFR     =  "ali09042010";
const char* WIFI_PASSWORD_METEOR_PLS  =  "Ali09042010_";
const char* WIFI_PASSWORD_BBOX    =  "Ali09042010_";

char ssid[] = "Bbox-Bek-2.4G";
char pass[] = "Ali09042010_";

IPAddress blynkLocalServer;
IPAddress BLYNK_SERVER_BBOX(192,168,1,46);
IPAddress BLYNK_SERVER_METEOR_SFR(192,168,1,168);
IPAddress BLYNK_SERVER_METEOR_ETH_PLS(192,168,1,194);

void  blynkTimers(void)
{
//  Blynk.virtualWrite(V30, inactivityPowerOffTimer);
//  Blynk.virtualWrite(V31, inactivityRestartTimer);
//  Blynk.virtualWrite(V72, zapTimerSec);
}


void timersMillis(void)
{
//    internetSurvilanceTimer = millis();
//    liveTimerOff            = millis();
//    OtaTimeoutTimer            = millis();
//    ackTimer                = millis();
//    restartAfterResetNG     = millis();
//    NetgeerResetGooglLostTimer = millis();
//    blynkNotActiveTimer     = millis();
//    routerResetTimer        = millis();
//    resetNetgeerAfterInternetLossTimer = millis();
//    Router_24_hoursTimer       = millis();
} 

void ledInit(void)
{
  RELAY_LED_V2.on();
}

bool  wifi_connect()
{
  _wifiIsConnected = false;

////#ifdef METEOR_ETH_PLS
//    wifiMulti.addAP(WIFI_SSID_SFR , WIFI_PASSWORD_SFR);
////#endif
//
////#ifdef METEOR_WIFI
//    wifiMulti.addAP(WIFI_SSID_METEOR_PLS, WIFI_PASSWORD_METEOR_PLS);
////#endif  
//  
//
////#ifdef NICE
//   wifiMulti.addAP(WIFI_SSID_SFR, WIFI_PASSWORD_SFR);
////#endif  

 
    wifiMulti.addAP(WIFI_SSID_BBOX, WIFI_PASSWORD_BBOX);
 

  
  Serial.println("Connecting Wifi...");
  //Connecting to the strongest WiFi connection
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected to: " + String (WiFi.SSID() )) ;
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  //print IP of the connected WiFi network
    _wifiIsConnected = true;
  }
  else  // if not WiFi not connected
  {
    _wifiIsConnected = false;
  }
  Serial.println("WIFI: "); Serial.println( _wifiIsConnected ? F("Connected") : F("Not Connected"));
  return _wifiIsConnected ;
}

bool blynkconnect()
{
    _blynkIsConnected = false;

    if(String (WiFi.SSID() ) == WIFI_SSID_METEOR_PLS)
    blynkLocalServer = BLYNK_SERVER_METEOR_ETH_PLS ;

    if(String (WiFi.SSID() ) == WIFI_SSID_BBOX )
    blynkLocalServer = BLYNK_SERVER_BBOX;

    if(String (WiFi.SSID() ) == WIFI_SSID_SFR )
    blynkLocalServer = BLYNK_SERVER_METEOR_SFR;
    
    Blynk.config(BLYNK_AUTH_TOKEN, blynkLocalServer, 8080);
    Blynk.connect(BlynkServerTimeout);
    delay(1000);
    _blynkIsConnected = Blynk.connected();
    if (_blynkIsConnected)
    {
      Serial.println("BLYNK: ");

      if (_blynkIsConnected)
      {
        Serial.print("Connected to ");
        Serial.println(blynkLocalServer.toString());
      }
      else
      {
        Serial.println("Not Connected");
      }

      blynkAtiveTimer     = millis();
      blynkActive = false;
  //    ledInit();
      terminal.clear();
      terminal.println(WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) + " Server IP: " + blynkLocalServer.toString() + "\n");
      terminal.flush();
    }
  return _blynkIsConnected;
}




BLYNK_WRITE(V1)
{
    relayState = param.asInt();

    digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);

    Blynk.virtualWrite(V2, relayState);
}

BLYNK_CONNECTED()
{
    Blynk.syncVirtual(V1);
}

void setup()
{
      Serial.begin(115200);
      pinMode(RELAY_PIN, OUTPUT);
      digitalWrite(RELAY_PIN, HIGH);
      
      wifiAvailable = wifi_connect();
      BLYNK_AUTH_TOKEN = BLYNK_AUTH_TOKEN_ESP8266_ESP01_1;
//      auth = BLYNK_AUTH_TOKEN_ESP8266_ESP01_1;
      blynkConnected = blynkconnect();
      blynkTimers();
     
      timersMillis();    

  //    resetInactivityTimer();    


//    Blynk.config(auth, BLYNK_SERVER_BBOX, 8080);
//    Blynk.connect();
}

void loop()
{
    Blynk.run();
}
