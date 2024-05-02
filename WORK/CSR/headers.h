//#define CSR     // R64 SCATOLA 1CH TTGO
#define CSR2      // R65 SCATOLA 4CH ESP32S
//#define CSR3   //R66 SWAN CASE 2CH ESP32S






//#define TEST
#define ROUTER_CH 18  //PHYSICAL CH ON REMOTE CONTROL IS 1
#define BLYNK_TEMPLATE_ID           "TMPL5Y2na6zpd"
#define BLYNK_TEMPLATE_NAME         "CSR"

    #define AWS_IOT_PUBLISH_TOPIC_RC     "csr1/sub/rc"
    #define AWS_IOT_PUBLISH_TOPIC_RC_2   "csr2/sub/rc"
    #define AWS_IOT_PUBLISH_TOPIC_RC_3   "csr3/sub/rc"

    #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_1   "csr1/sub/video"
    #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_2   "csr2/sub/video"
    #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_3   "csr3/sub/video"


    
#ifdef CSR      //TTGO R64 SCATOLA 1CH TTGO
    #define VERSION_ID "V28.0"
    #define BLYNK_AUTH_TOKEN                "B1pZ48rPHfdQ8LxlqCoiPk8fxWBbv7B0" //CSR
    #define THINGNAME "CSR1"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "csr1/sub/rc"
    #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "csr1/sub/video"
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "csr1/sub/zap"
    #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "csr1/sub/rx"
    #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "csr1/sub/avrc"
    #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "csr1/sub/dvr"
    #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "csr1/sub/reboot"
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "csr1/sub/zapchanel"
    #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "csr1/sub/localWebOta"
    #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "csr1/sub/GitHubWebOta"
    #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "csr1/sub/ideOta"
    #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "csr1/sub/version"
    #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "csr1/sub/scan"
    #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "csr1/sub/repeat"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "csr1/sub/preset"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "csr1/sub/zapauto"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "csr1/sub/zton"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "csr1/sub/ztoff" 
    #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/CSR/CSR1.bin"  // URL to download the firmware from
#endif

#ifdef CSR2     // R65 SCATOLA 4CH ESP32S
 
    #define VERSION_ID "V28.0"
    #define BLYNK_AUTH_TOKEN                "_cqYD1seWElWB-S1IxutIEI7VWmDpb05" //CS2
    #define THINGNAME "CSR2"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "csr2/sub/rc"
    #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "csr2/sub/video"
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "csr2/sub/zap"
    #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "csr2/sub/rx"
    #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "csr2/sub/avrc"
    #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "csr2/sub/dvr"
    #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "csr2/sub/reboot"
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "csr2/sub/zapchanel"
    #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "csr2/sub/localWebOta"
    #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "csr2/sub/GitHubWebOta"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "csr2/sub/ideOta" 
    #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "csr2/sub/version"
    #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "csr2/sub/scan"
    #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "csr2/sub/repeat"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "csr2/sub/preset"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "csr2/sub/zapauto"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "csr2/sub/zton"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "csr2/sub/ztoff" 
    #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/CSR/CSR2.bin"  // URL to download the firmware from
#endif


#ifdef CSR3     //R66 SWAN CASE 2CH ESP32S
   
    #define VERSION_ID " V28.0"
    #define BLYNK_AUTH_TOKEN                "6DH6QZgVXrGXU5VzOpJSJgHoyXWL7aWS" //CSR3
    #define THINGNAME "CSR3"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_RC      "csr3/sub/rc"
    #define AWS_IOT_SUBSCRIBE_TOPIC_VIDEO   "csr3/sub/video"
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAP     "csr3/sub/zap"
    #define AWS_IOT_SUBSCRIBE_TOPIC_RX      "csr3/sub/rx"
    #define AWS_IOT_SUBSCRIBE_TOPIC_AV_RC   "csr3/sub/avrc"
    #define AWS_IOT_SUBSCRIBE_TOPIC_DVR     "csr3/sub/dvr"
    #define AWS_IOT_SUBSCRIBE_TOPIC_REBOOT  "csr3/sub/reboot"
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH   "csr3/sub/zapchanel"
    #define AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA   "csr3/sub/localWebOta"
    #define AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA   "csr3/sub/GitHubWebOta" 
    #define AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA   "csr3/sub/ideOta"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_VERSION   "csr3/sub/version" 
    #define AWS_IOT_SUBSCRIBE_TOPIC_SCAN   "csr3/sub/scan"
    #define AWS_IOT_SUBSCRIBE_TOPIC_REPEAT   "csr3/sub/repeat"   
    #define AWS_IOT_SUBSCRIBE_TOPIC_PRESET  "csr3/sub/preset"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO   "csr3/sub/zapauto"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON   "csr3/sub/zton"  
    #define AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF   "csr3/sub/ztoff" 
    #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/CSR/CSR3.bin"  // URL to download the firmware from
#endif

#ifdef TEST     //R66 SWAN CASE 2CH ESP32S
    
    #define VERSION_ID " V26.0 "
    #define BLYNK_AUTH_TOKEN                "C7dsGNwfPTFwBRlhuQk5k9g81kPeiM2B" 
    #define THINGNAME "CSR4"   
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
    #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/CSR/BIN/TEST.bin"  // URL to download the firmware from
#endif

#define INTERNET_LOSS_TO_RESET_NG_TIMER 120000   // 2 MIN
#define RESTART_AFTER_NG_RESET_TIMER 60000   // 2 MIN
#define ROUTER_24_HOURS 86400000  // 24 HOURS
#define WiFi_TIMEOUT 30000  // 15sec Wifi connection timeout
                   
#define BLYNK_SERVER "blynk.cloud" //BLYNK2 REMOTE SERVER 

#define WIFI_SSID_METEOR_FREE      "BEK_FREE"
#define WIFI_SSID_METEOR_BU      "BEK_BU"
#define WIFI_SSID_METEOR_BUF      "BEK_BUF"
#define WIFI_SSID_METEOR_BOX    "BEK_BOX"
#define WIFI_SSID_GIGA        "GIGACUBE_BEK"                                                
#define WIFI_SSID_FREE        "Freebox-bek"                                              
#define WIFI_SSID_XIAOMI      "XIAOMI_BEK" 

#define WIFI_PASSWORD         "ali09042010"
#define WIFI_PASSWORD_METEOR  "Ali09042010_"

#define WIFI_SSID_TEMP      "TP-Link_97D8" 
#define WIFI_PASSWORD_TEMP  "67942015"
//av1
#define I2C_SDA             21  //green
#define I2C_SCL             22  //yellow

#define SDA_2              33  //green
#define SCL_2              32  //yellow

//RC
#define RC_TX_PIN           15
#define RC_CODE_LENGTH      24

//RELAYS
#define AV_RX_DVR_PIN_2     2
#define I2C_1_2_RELAY       4
#define I2C_3_4_RELAY       0

#define DEBUG_SERIAL

enum {
CH_MODE_0,
CH_MODE_1,
CH_MODE_2,
CH_MODE_3,
CH_MODE_4,
} ;

enum {
ESP1,
ESP2,
ESP3,
TEST4,
} ;


enum {
ACK_GOOD,
ACK_BAD,
} ;


enum {
BLYNK,
NODE_RED,
} ;

enum {
NA,
R_24,
R_25,
R_26,
R_27,
R_28,
R_29,
R_48,
R_49,
R_50,
R_51,
R_52,
R_53,
R_62,
R_63,
R_64,
R_65,
R_66,
ROUTER,
R_68,
} ;

enum {
NONE,
Q_EVENT_FREQ_V0,
Q_EVENT_RC_CH_NR_V1,
Q_EVENT_VIDEO_CH_V2,
Q_EVENT_REPEAT_V3,

Q_SCAN_ACTIVE_CH_V4,
Q_EVENT_AUTOMATIC_RC_L_R_V5,
Q_EVENT_OTA_LOCAL_WEB_WIFI_V6,
Q_EVENT_OTA_GITHUB_V7,

Q_EVENT_REBOOT_V8,
Q_EVENT_SELECTED_RECIEVER_V9,
Q_EVENT_ZAP_SCAN_ONLY_V10, 
Q_EVENT_WIFI_IDE_V11,
Q_EVENT_BOARD_V12,
//I2C_LED_V13(V13);  // USED
Q_EVENT_RM_ID_11_V14,  //52
Q_EVENT_RM_ID_12_V15,  //53


Q_EVENT_RC_REMOTE_CSR1_V16,
Q_EVENT_RC_REMOTE_CSR3_V17,
Q_EVENT_AWS_RECONNECT_V18,
Q_EVENT_ROOM_AV_RC_V19,
Q_EVENT_RC_REMOTE_CSR2_V20,
Q_EVENT_RM_ID_10_V21,//51,
// NOT DECLARED IN CONSOLE V22,
Q_EVENT_RM_ID_13_V23,  //62
//V24 Version,
Q_EVENT_ZAP_TIMER_OFF_V25,
Q_EVENT_RESET_FREQ_V26,
Q_EVENT_DVR_ON_OFF_V27,

Q_EVENT_REL1_CH_V30,
Q_EVENT_REL2_CH_V31,
Q_EVENT_REL3_CH_V32,
Q_EVENT_REL4_CH_V33,
Q_EVENT_ZAP_ALL_ON_OFF_V34,
Q_EVENT_SHARE_VIDEO_WITH_CSR1_V35,
Q_EVENT_SHARE_VIDEO_WITH_CSR2_V36,
Q_EVENT_SHARE_VIDEO_WITH_CSR3_V37,
Q_EVENT_WIFI_RSSI_V38,
Q_EVENT_RESET_ROUTER_V39,
Q_EVENT_AUTOMATIC_RESET_ROUTER_V40,

Q_EVENT_ZAP_V71,
Q_EVENT_ZAP_TIMER_V72,
Q_EVENT_ZAP_CHANNEL1_V81,
Q_EVENT_ZAP_CHANNEL2_V82,
Q_EVENT_ZAP_CHANNEL3_V83,
Q_EVENT_ZAP_CHANNEL4_V84,
Q_EVENT_ZAP_CHANNEL5_V85,
Q_EVENT_ZAP_CHANNEL6_V86,
Q_EVENT_ZAP_CHANNEL7_V87,
Q_EVENT_ZAP_CHANNEL8_V88,
Q_EVENT_ZAP_CHANNEL9_V89,
Q_EVENT_ZAP_CHANNEL10_V94,
Q_EVENT_ZAP_CHANNEL11_V95,
Q_EVENT_ZAP_CHANNEL12_V96,
Q_EVENT_ZAP_CHANNEL13_V97,

Q_EVENT_RC_PULSE_V98,
Q_EVENT_RM_ID_20_V100, //12
Q_EVENT_RC_REPETION_V101,
Q_EVENT_TERMINAL_V102,


Q_EVENT_ZAP_CHANNEL14_V106,
Q_EVENT_ZAP_CHANNEL15_V107,
Q_EVENT_ZAP_CHANNEL16_V108,
Q_EVENT_ZAP_CHANNEL17_V109,
Q_EVENT_ZAP_CHANNEL18_V110,
Q_EVENT_ZAP_CHANNEL19_V111,
Q_EVENT_RM_ID_10_V112,



//LIVE_LED_V121, //
Q_EVENT_RM_ID_02_V122, //25
Q_EVENT_RM_ID_03_V123, //26
Q_EVENT_RM_ID_04_V124,//27
Q_EVENT_RM_ID_05_V125,//28
Q_EVENT_RM_ID_06_V126,//29
Q_EVENT_RM_ID_07_V127,//48
Q_EVENT_RM_ID_08_V93,//49
Q_EVENT_RM_ID_09_V80,//50



Q_EVENT_RM_ID_14_V103,  //63
Q_EVENT_RM_ID_15_V104,  //64
Q_EVENT_RM_ID_16_V105,  //65
Q_EVENT_RM_ID_17_V90,  //66
Q_EVENT_RM_ID_18_V91,  //67
Q_EVENT_RM_ID_19_V92,  //68

} ;



#define BLYNK_ACTIVE_TIMEOUT   60000 // 1 MIN
#define LIVE_TIMER_OFF  3000
#define WIFI_IDE_TIMER 300000  //10 MIN
#define WIFI_SURVILANCE_TIMER 120000  // 2 MIN
#define WIFI_RECONNECT_TIMER 10000  // 10 sec
#define DEEP_SLEEP_TIME_60_MIN 3600000000   // 1 HOUR in micro 
#define BLYNK_ACTIVITY_STOP_TIMER 1800000  // 30 MIN

#define PING_GOOGLE_BLYNK_TIMER 30000  // 30 sec


#define ROUTER_24_HOURS 86400000  // 24 HOURS
#define ROUTER_RESET_TIMER  2000  // 2 SEC
#define MUX_ROOM_ZAP        30000  // 2 SEC


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


#define MAX_NR_CHANNELS  20

#define WEB 0
#define DDNS 1

enum {
  SM_CH1_A,
  SM_CH1_B,
  SM_CH1_C,
  SM_CH2_A,
  SM_CH2_B,
  SM_CH2_C,
  SM_CH3_A,
  SM_CH3_B,
  SM_CH3_C,
  SM_CH4_A,
  SM_CH4_B,
  SM_CH4_C,
  SM_CH5_A,
  SM_CH5_B,
  SM_CH5_C,
  SM_CH6_A,
  SM_CH6_B,
  SM_CH6_C,
  SM_CH7_A,
  SM_CH7_B,
  SM_CH7_C,
  SM_CH8_A,
  SM_CH8_B,
  SM_CH8_C,
  SM_CH9_A,
  SM_CH9_B,
  SM_CH9_C,
  SM_CH10_A,
  SM_CH10_B,
  SM_CH10_C,
  SM_CH11_A,
  SM_CH11_B,
  SM_CH11_C,
  SM_CH12_A,
  SM_CH12_B,
  SM_CH12_C,
  SM_CH13_A,
  SM_CH13_B,
  SM_CH13_C,
  SM_CH14_A,
  SM_CH14_B,
  SM_CH14_C,
  SM_CH15_A,
  SM_CH15_B,
  SM_CH15_C,
  SM_CH16_A,
  SM_CH16_B,
  SM_CH16_C,
  SM_CH17_A,
  SM_CH17_B,
  SM_CH17_C,
  SM_CH18_A,
  SM_CH18_B,
  SM_CH18_C,
  SM_CH19_A,
  SM_CH19_B,
  SM_CH19_C,
  SM_CH20_A,
  SM_CH20_B,
  SM_CH20_C,
};
