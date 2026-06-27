#define DEBUG_SERIAL
enum {
NONE,
Q_EVENT_V0,
Q_EVENT_V1,
Q_EVENT_V2,
Q_EVENT_V3,
Q_EVENT_V4,
Q_EVENT_V5,
Q_EVENT_V6,
Q_EVENT_V7,
Q_EVENT_V8,
} ;
#define _TIMERINTERRUPT_LOGLEVEL_     4
#define RELAY_PIN 0
#define WiFi_TIMEOUT 30000  // 15sec Wifi connection timeout

#define INTERNET_LOSS_TO_RESET_NG_TIMER (2 * 60UL * 1000UL)   // 2 MIN
#define ROUTER_RESET_TIMER              (1 * 60UL * 1000UL)   //1 MIN
#define RESTART_AFTER_NG_RESET_TIMER    (7 * 60UL * 1000UL)   //7 MIN
#define INACTIVITY_TIMEOUT_MS_RESTART  (10UL * 60UL * 60UL * 1000UL)  //10 Hours
#define INACTIVITY_TIMEOUT_MS_POWER_OFF  (1UL * 60UL * 60UL * 1000UL)  //10 Hours
#define WIFI_DISCONNECTED_RESTART  (30 * 60UL * 1000UL)  //30 MIN
#define uS_TO_S_FACTOR 1000000ULL
#define LIGHT_SLEEP_WAKE 10*uS_TO_S_FACTOR

#define BLYNK_ACTIVE_TIMEOUT   60000 // 1 MIN
#define LIVE_TIMER_OFF  3000
#define WIFI_IDE_TIMER 900000  //15 MIN
#define WIFI_SURVILANCE_TIMER 120000  // 2 MIN
#define WIFI_RECONNECT_TIMER 10000  // 10 sec
#define DEEP_SLEEP_TIME_60_MIN 3600000000   // 1 HOUR in micro 
#define BLYNK_ACTIVITY_STOP_TIMER 1800000  // 30 MIN
#define PING_GOOGLE_BLYNK_TIMER 30000  // 30 sec
#define ROUTER_24_HOURS 86400000  // 24 HOURS

#ifdef DEBUG_SERIAL
 #define DEBUG_PRINT(x)    Serial.print (x)
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_WRITE(x)    Serial.write (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_WRITE(x)  
#endif


#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"
#define BLYNK_BLACK      "#444444"
#define BLYNK_GRAY      "#cdd6f2"

#define   ON              true
#define   OFF             false

#define   EN              true
#define   DIS            false
#define   SEC_1            1000
#define   SEC_2            2000
#define   SEC_3            3000
#define   SEC_4            4000
#define   SEC_5            5000
#define   SEC_10           10000
#define   SEC_20           20000
#define   SEC_30           30000
#define   SEC_60           60000
#define   MIN_5            300000
