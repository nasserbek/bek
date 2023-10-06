//#define CSR

#define VERSION_ID " CSR.0 " 
                   
#define BLYNK_SERVER "blynk.cloud" //BLYNK2 REMOTE SERVER 
#define BLYNK1_SERVER "blynk-cloud.com" //BLYNK REMOTE SERVER


#define WIFI_SSID_GIGA    "GIGACUBE_BEK"                                                
#define WIFI_SSID_FREE    "Freebox-bek"                                              
#define WIFI_SSID_XIAOMI    "XIAOMI_BEK" 
#define WIFI_SSID_HUAWEI  "HUAWEI_BEK" 


#ifdef CSR
    #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/CSR/build/esp32.esp32.ttgo-t1/CSR.ino.bin"  // URL to download the firmware from
    #define NOTIFIER_ID " CSR -"
    
    //2023
    #define BLYNK_TEMPLATE_ID           "TMPL5Y2na6zpd"
    #define BLYNK_TEMPLATE_NAME         "CSR"
    #define BLYNK_AUTH_TOKEN            "B1pZ48rPHfdQ8LxlqCoiPk8fxWBbv7B0"
    
    #define BLYNK1_AUTH_TOKEN "D4AU1HexWcErQ9vtpkP_EgocpnoArZKC" //MAIN BOX
#else
    #define gitHubURL  "https://raw.githubusercontent.com/nasserbek/bek/master/VHY/VHY.ino.ttgo-t1.bin" // URL to download the firmware from
    #define NOTIFIER_ID " CSR2 -"
    #define BLYNK_TEMPLATE_ID  "TMPL5Y2na6zpd"
    #define BLYNK_DEVICE_NAME "CSR"
    #define BLYNK_AUTH_TOKEN "_cqYD1seWElWB-S1IxutIEI7VWmDpb05"  
      
    #define BLYNK1_AUTH_TOKEN "ya1T2eipkMhB3NvyLeAyRVRHqPAUXUG-"  //SPARE BOX
#endif


// FOR FIREBASE ONLY

//#define FREE
//#define BOUY
//#define XIAOMI
//#define HUAWEI
#define GIGACUBE

#ifdef FREE
  #define WIFI_SSID_FB WIFI_SSID_FREE                                              
#endif


#ifdef XIAOMI
  #define WIFI_SSID_FB WIFI_SSID_XIAOMI                                             
#endif

#ifdef GIGACUBE
  #define WIFI_SSID_FB WIFI_SSID_GIGA                                                
#endif

#ifdef HUAWEI
  #define WIFI_SSID_FB WIFI_SSID_HUAWEI                                                
#endif

#define WIFI_PASSWORD "ali09042010"

#define DEBUG_SERIAL


#define BLYNK_ACTIVE_TIMEOUT   60000 // 1 MIN
#define LIVE_TIMER_OFF  3000
#define WIFI_IDE_TIMER 600000  //10 MIN
#define WIFI_SURVILANCE_TIMER 120000  // 2 MIN
#define WIFI_RECONNECT_TIMER 10000  // 10 sec
#define DEEP_SLEEP_TIME_60_MIN 3600000000   // 1 HOUR in micro 
#define BLYNK_ACTIVITY_STOP_TIMER 1800000  // 30 MIN

#define PING_GOOGLE_BLYNK_TIMER 30000  // 30 sec
#define INTERNET_LOSS_TO_RESET_NG_TIMER 120000   // 2 MIN
#define RESTART_AFTER_NG_RESET_TIMER 1800000   // 30 MIN
#define SMS_24_HOURS 86400000  // 24 HOURS
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



#define   SMS_FB_ON                    "Fbon"
#define   SMS_FB_OFF                   "Fboff"
#define   SMS_SW_RESET                 "Reset"
#define   SMS_WIFI_ON                  "Wifion"
#define   SMS_WIFI_OFF                 "Wifioff"
#define   SMS_OTA_GSM                  "Otag"
#define   SMS_OTA_WEB                  "Otaw"
#define   SMS_OTA_IDE                  "Otai"
#define   SMS_VER                      "Ver"
#define   SMS_SLEEP                    "Sleep"
#define   SMS_BLYNK_ON                 "Blon"
#define   SMS_BLYNK_OFF                "Bloff"
#define   SMS_SMS_ON                   "Smson"
#define   SMS_SMS_OFF                  "Smsoff"
#define   SMS_SETTINGS                 "Settings"




#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"
#define BLYNK_BLACK      "#444444"
#define BLYNK_GRAY      "#cdd6f2"





//firebase
/************************************FIREBASE ***********************************/
#define    FB_NOTIFIER          "/FB_SMS"

// ROOMS Commands
#define    CMD_PATH         "/PC/"
#define    IDLE_PATH         "/"

#define    FB_FREQ              "/Rooms_CMD_Frequency" 
#define    FB_VIDEO_CH         "/Rooms_CMD_Video_ch"

#define    FB_T315_CH_NR        "/Rooms_CMD_T315_ch"
#define    FB_T433_CH_NR        "/Rooms_CMD_T433_ch"

#define    FB_REMOTE_CTRL_BOTH  "/Rooms_CMD_Remote_control_Both"
#define    FB_REMOTE_CTRL_RC    "/Rooms_CMD_Remote_control_Solo_RC"
#define    FB_REMOTE_CTRL_VIDEO "/Rooms_CMD_Remote_control_Solo_Video"

#define    FB_ROOM_NR_1_5       "/Rooms_CMD_Room_nr_Room_1_5"
#define    FB_ROOM_NR_6_10      "/Rooms_CMD_Room_nr_Room_6_10"
#define    FB_ROOM_NR_11_15     "/Rooms_CMD_Room_nr_Room_11_15"
#define    FB_ROOM_NR_16_20     "/Rooms_CMD_Room_nr_Room_16_20"

//ROOMS Status
#define    FB_ACK_LED            "PC/Rooms_ST_I2C_status"
#define    FB_T315_STATUS        "PC/Rooms_ST_T315_status"
#define    FB_T433_STATUS        "PC/Rooms_ST_T433_status"
#define    FB_VIDEO_CH_STATUS    "PC/Rooms_ST_Video_ch"
#define    FB_FREQ_STATUS        "PC/Rooms_ST_Frequency"


// SETTINGS Commands
#define    FB_OTA_GITHUB        "/Settings_CMD_Github_OTA"
#define    FB_OTA_GSM           "/Settings_CMD_GSM_OTA"
#define    FB_RESET             "/Settings_CMD_Restart_ESP"
#define    FB_RESET_NETGEER     "/Settings_CMD_Netgeer_reset"
#define    FB_DVR_CMD           "/Settings_CMD_DVR"

// SETTINGS STATUS
#define    FB_DAYS              "PC/Settings_ST_Bek1_days"
#define    FB_HOURS             "PC/Settings_ST_Bek1_hours"
#define    FB_MINUTES           "PC/Settings_ST_Bek1_minutes"
#define    FB_SECONDS           "PC/Settings_ST_Bek1_seconds"


// ZAPCommands
#define    FB_ZAP_CMD           "/Zapping_CMD_Zapping"
#define    FB_ZAP_CH_1          "/Zapping_CMD_Ch1"
#define    FB_ZAP_CH_2          "/Zapping_CMD_Ch2"
#define    FB_ZAP_CH_3          "/Zapping_CMD_Ch3"
#define    FB_ZAP_CH_4          "/Zapping_CMD_Ch4"
#define    FB_ZAP_CH_5          "/Zapping_CMD_Ch5"
#define    FB_ZAP_CH_6          "/Zapping_CMD_Ch6"
#define    FB_ZAP_CH_7          "/Zapping_CMD_Ch7"
#define    FB_ZAP_CH_8          "/Zapping_CMD_Ch8"
#define    FB_ZAP_CH_9          "/Zapping_CMD_Ch9"
#define    FB_ZAP_CH_10          "/Zapping_CMD_Ch10"
#define    FB_ZAP_CH_11          "/Zapping_CMD_Ch11"
#define    FB_ZAP_CH_12          "/Zapping_CMD_Ch12"
#define    FB_ZAP_CH_13          "/Zapping_CMD_Ch13"
#define    FB_ZAP_CH_14          "/Zapping_CMD_Ch14"
#define    FB_ZAP_CH_15          "/Zapping_CMD_Ch15"

#define    FB_ZAP_CH_16         "/Zapping_CMD_Ch16"
#define    FB_ZAP_CH_17          "/Zapping_CMD_Ch17"
#define    FB_ZAP_CH_18         "/Zapping_CMD_Ch18"
#define    FB_ZAP_CH_19         "/Zapping_CMD_Ch19"
#define    FB_ZAP_CH_20         "/Zapping_CMD_Ch20"
// ZAP STATUS
#define    FB_ZAP_STATUS         "PC/Zapping_ST_Zapping"


    
//BLYNK
/*****e*******************************BLYNK***********************************/
#define    FB_FREQ_ID           0
#define    FB_T433_CH_NR_ID     1
#define    FB_AV_7SEG_ID        2
#define    ROOM_201_TO_205      3
#define    FB_OTA_ID            7
#define    FB_RESET_ID          8
#define    FB_SEND_TO_BLYNK_ID  10
#define    FB_SMS_ON_ID         11
#define    FB_T315_CH_NR_ID     14
#define    FB_NETGEER_ID        15
#define    ROOM_206_TO_210      16
#define    ROOM_211_TO_215      17
#define    ROOM_216_TO_220      18
#define    ROOM_AV_RC           19
#define    FB_ZAP_ID            71
#define    FB_ZAP_TIMER_ID      72
#define    FB_ZAP_CHANNEL_ID1   81
#define    FB_ZAP_CHANNEL_ID2   82
#define    FB_ZAP_CHANNEL_ID3   83
#define    FB_ZAP_CHANNEL_ID4   84
#define    FB_ZAP_CHANNEL_ID5   85
#define    FB_ZAP_CHANNEL_ID6   86
#define    FB_ZAP_CHANNEL_ID7   87
#define    FB_ZAP_CHANNEL_ID8   88
#define    FB_AV_CH_PLUS_ID     90
#define    FB_AV_CH_MINUS_ID    91
#define    FB_AV_FR_MINUS_ID    92
#define    FB_AV_FR_PLUS_ID     93
#define    FB_RC_PULSE_ID       98
#define    FB_WIFI_IDE_ID       100
#define    FB_RC_REPETION_ID    101
#define    FB_SLEEP_TIMER_ID    102
#define    FB_FB_OFF_ID         103
#define    FB_WIFI_WEB_ID       104
#define    FB_WIFI_OTA_ID       105


#define    FB_VERSION_ID        307
#define    FB_BLYNK_ON_OFF_ID   309
#define    FB_WIFI_OFF_ID       310
#define    FB_SETTINGS_ID       311
#define    IDLE_PATH_ID        312
#define    FB_ZAP_CHANNEL_ID   316
#define    FB_DVR_ON_ID         317
#define    FB_DVR_OFF_ID         318




/*
#define    FB_RC_LED           "/PC/RcTransmitter_Led"
#define    FB_ACK_LED          "/PC/AvReceiver_Led"
#define    FB_NACK_LED          "/PC/AvReceiver_nLed"
#define    FB_AV_OUTPUT        "/PC/AvReceiver_Rsss" 
#define    FB_WIFI_ST          "PC/Wifi_Led"
#define    FB_FIREBASE_ST      "PC/Firebase_Led"
#define    FB_BLYNK_ST         "PC/Blynk_Led"
#define    FB_SIM800_ST        "PC/Gsm_Led"
#define    FB_ALIVE            "PC/Alive"


#define    FB_BLYNK_ON_OFF_SEND      "PC/FB_BL_ON"
#define    FB_WIFI_OFF_SEND          "PC/FB_WIFI_OFF"
#define    FB_FB_OFF_SEND            "PC/FB_FB_OFF"
#define    FB_VERSION_SEND           "PC/FB_VER"
#define    FB_SMS_ON_SEND            "PC/FB_SMS_ON"
#define    FB_T433_CH_NR_SEND       "PC/FB_T433_CH_NR"
#define    FB_FREQ_SEND             "PC/FB_FREQ" 
#define    FB_RESET_SEND             "PC/FB_RESET"
#define    FB_OTA_SEND               "PC/FB_OTA"
#define    FB_AV_7SEG_SEND           "PC/FB_AV_7SEG" 


#define    PATH             "/PC/"
#define    FB_AV_CMD        "PC/FB_AV_CMD" 
#define    FB_AV_ON         "PC/FB_AV_ON"
#define    FB_AV_ST         "PC/FB_AV_ST" 
#define    FB_T433_ST       "PC/FB_T433_ST"
#define    FB_T315_CH_NR    "PC/FB_T315_CH_NR"
#define    FB_T315_ST       "PC/FB_T315_ST"
#define    FB_DV_ON         "PC/FB_DV_ON"
#define    FB_DV_ST         "PC/FB_DV_ST" 
#define    FB_LCD           "PC/FB_LCD"


*/

const String FB_T433_STATUS_PATH[32] =
{"","PC/T433_CH1_ST","PC/T433_CH2_ST","PC/T433_CH3_ST","PC/T433_CH4_ST","PC/T433_CH5_ST","PC/T433_CH6_ST",
"PC/T433_CH7_ST","PC/T433_CH8_ST","PC/T433_CH9_ST","PC/T433_CH10_ST","PC/T433_CH11_ST","PC/T433_CH12_ST",
"PC/T433_CH13_ST","PC/T433_CH14_ST","PC/T433_CH15_ST","PC/T315_CH1_ST","PC/T315_CH2_ST","PC/T315_CH3_ST",
"PC/T315_CH4_ST","PC/T315_CH5_ST","PC/T315_CH6_ST","PC/T315_CH7_ST","PC/T315_CH8_ST","PC/T315_CH9_ST",
"PC/T315_CH10_ST","PC/T315_CH11_ST","PC/T315_CH12_ST","PC/T315_CH13_ST","PC/T315_CH14_ST","PC/T315_CH15_ST",""};





#define PLL_ADDRESS             0x61  //0110 0 001 becomes 0xC2
#define PCF8574_CMD_ADDRESS     0x20           // PCF8574 device 1    Commands
#define PCF8574_STATUS_ADDRESS  0x21          // PCF8574 device 2    7 Segment Display Reader
   

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

#define EEPROM_VER_ADD      32
#define EEPROM_VER_LEN      40
#define EEPROM_GITHUB_ADD   80
#define EEPROM_BLYNK_ADD    81
#define EEPROM_FB_ADD       82
#define EEPROM_WIFI_ADD     83
#define EEPROM_SMS_ADD      84
#define EEPROM_ERR_ADD      85
#define EEPROM_SIM800_ADD    86
#define RC_REPETION_ADD     90

#define INTERNET_LOST       '1'
#define WDG_ERR             '2'
#define WIFI_OFF            '3'
#define SW_RESET            '4'
#define INTERNET_FAILURE    '5'
#define WIFI_FAILURE        '6'
#define TEN_HOURS_TIMER     '7'
#define IDE_WIFI            '8'
#define DEEP_SLEEP          '9'

//ota

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00
#define BAUDRATE            38400
#define EEPROM_GITHUB_ADD   80
#define EEPROM_WIFI_ADD     83
#define RC_CODE_LENGTH      24


// TTGO T-Call pin definitions
#define MODEM_TX            27
#define MODEM_RX            26
#define MODEM_PWKEY         4
#define MODEM_POWER_ON      23
#define MODEM_RST           5
#define I2C_SDA             21
#define I2C_SCL             22
//av
#define AV_OUTPUT_AN        34
#define RC_TX_PIN           15
#define MRT                 13 

#define AV_RX_DVR_PIN_2     2


#define NETGEER_PIN_0       0

enum {
NONE,
Q_EVENT_FREQ_V0,
Q_EVENT_RC_CH_NR_V1,
Q_EVENT_VIDEO_CH_V2,
Q_EVENT_ROOM_ID_1_TO_5_V3,
//(V4, days);

//ACK_LED_V5(V5);   //Ack
//T433_LED_V6(V6);   //T433 St
Q_EVENT_OTA_GSM_V7,
Q_EVENT_REBOOT_V8,
//ROOM INDEX V9; 
Q_EVENT_ALL_CH_V10,  //SPARE
//V11, seconds);
//SMS_LED_V12(V12);  //sms
//T315_LED_V13(V13);  //T315 St
Q_EVENT_BLYNK1_V14,
Q_EVENT_NETGEER_V15,
Q_EVENT_ROOM_ID_6_TO_10_V16,
Q_EVENT_ROOM_ID_11_TO_15_V17,
Q_EVENT_ROOM_ID_16_TO_20_V18,
Q_EVENT_ROOM_AV_RC_V19,
//V20, hours);
//V21, minutes);
//V22 STREAMING
Q_EVENT_ROUTER_RESET_TIMER_V23,
//V24 Version,
Q_EVENT_ROOM_ID_21_25_V25,
Q_EVENT_RESET_FREQ_V26,
Q_EVENT_DVR_ON_OFF_V27,
Q_EVENT_03_SEL_V30,
Q_EVENT_21_SEL_V31,
Q_EVENT_27_SEL_V32,
Q_EVENT_50_SEL_V33,
Q_EVENT_20_SEL_V34,
Q_EVENT_52_SEL_V35,
Q_EVENT_53_SEL_V36,
Q_EVENT_22_SEL_V37,
Q_EVENT_208_SEL_V38,
Q_EVENT_215_SEL_V39,
Q_EVENT_217_SEL_V40,

Q_EVENT_ZAP_V71,
Q_EVENT_ZAP_TIMER_V72,
//ZAP_LED_V80(V80);  //Zap Status
Q_EVENT_ZAP_CHANNEL1_V81,
Q_EVENT_ZAP_CHANNEL2_V82,
Q_EVENT_ZAP_CHANNEL3_V83,
Q_EVENT_ZAP_CHANNEL4_V84,
Q_EVENT_ZAP_CHANNEL5_V85,
Q_EVENT_ZAP_CHANNEL6_V86,
Q_EVENT_ZAP_CHANNEL7_V87,
Q_EVENT_ZAP_CHANNEL8_V88,
Q_EVENT_ZAP_CHANNEL9_V89,

Q_EVENT_AV_CH_PLUS_V90,
Q_EVENT_AV_CH_MINUS_V91,
Q_EVENT_AV_FR_MINUS_V92,
Q_EVENT_AV_FR_PLUS_V93,

Q_EVENT_ZAP_CHANNEL10_V94,
Q_EVENT_ZAP_CHANNEL11_V95,
Q_EVENT_ZAP_CHANNEL12_V96,
Q_EVENT_ZAP_CHANNEL13_V97,

Q_EVENT_RC_PULSE_V98,
Q_EVENT_WIFI_IDE_V100,
Q_EVENT_RC_REPETION_V101,
Q_EVENT_TERMINAL_V102,
Q_EVENT_OFF_V103,
Q_EVENT_WIFI_WEB_V104,
Q_EVENT_OTA_GITHUB_V105,

Q_EVENT_ZAP_CHANNEL14_V106,
Q_EVENT_ZAP_CHANNEL15_V107,
Q_EVENT_ZAP_CHANNEL16_V108,
Q_EVENT_ZAP_CHANNEL17_V109,
Q_EVENT_ZAP_CHANNEL18_V110,
Q_EVENT_ZAP_CHANNEL19_V111,
Q_EVENT_ZAP_CHANNEL20_V112,

Q_EVENT_VERSION_SMS_307,
Q_EVENT_BLYNK_ON_OFF_SMS_309,
Q_EVENT_WIFI_OFF_SMS_310,
Q_EVENT_SETTINGS_SMS_311,
Q_EVENT_IDLE_PATH_SMS_312,
Q_EVENT_ZAP_CHANNEL_SMS_316,
Q_EVENT_DVR_ON_SMS_317,
Q_EVENT_DVR_OFF_SMS_318,
Q_EVENT_C6_SMS_319,
} ;

#define MAX_NR_CHANNELS  20

#define WEB 0
#define DDNS 1

enum {
  SM_CH1_A,
  SM_CH1_B,
  SM_CH2_A,
  SM_CH2_B,
  SM_CH3_A,
  SM_CH3_B,
  SM_CH4_A,
  SM_CH4_B,
  SM_CH5_A,
  SM_CH5_B,
  SM_CH6_A,
  SM_CH6_B,
  SM_CH7_A,
  SM_CH7_B,
  SM_CH8_A,
  SM_CH8_B,
  SM_CH9_A,
  SM_CH9_B,
  SM_CH10_A,
  SM_CH10_B,
  SM_CH11_A,
  SM_CH11_B,
  SM_CH12_A,
  SM_CH12_B,
  SM_CH13_A,
  SM_CH13_B,
  SM_CH14_A,
  SM_CH14_B,
  SM_CH15_A,
  SM_CH15_B,

  SM_CH16_A,
  SM_CH16_B,
  SM_CH17_A,
  SM_CH17_B,
  SM_CH18_A,
  SM_CH18_B,
  SM_CH19_A,
  SM_CH19_B,
  SM_CH20_A,
  SM_CH20_B,
};
