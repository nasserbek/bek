//#define DEBUG_SERIAL
#define NETGEER
#define BEK

//#define FREE
//#define HUAWEI
//#define SFR
//#define OPPO
//#define BB


#ifdef DEBUG_SERIAL
 #define DEBUG_PRINT(x)    Serial.print (x)
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_WRITE(x)    Serial.write (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_WRITE(x)  
#endif

//sim
#define MODEM_TX        27
#define MODEM_RX        26
#define MODEM_PWKEY     4
#define MODEM_POWER_ON  23

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



#ifdef OPPO
  #define WIFI_SSID "OPPO Reno2 Bek"                                              
  #define WIFI_PASSWORD "ali09042010"
#endif

#ifdef FREE
  #define WIFI_SSID "Freebox_bek"                                              
#endif

#ifdef HUAWEI
  #define WIFI_SSID "HUAWEI-BEK"                                                
#endif

#ifdef NETGEER
  #define WIFI_SSID "NETGEER_BEK"                                                
#endif
#define WIFI_PASSWORD "ali09042010"

#ifdef SFR
  #define WIFI_SSID "SFR_BEK"                                                
#endif
#define WIFI_PASSWORD "ali09042010"

#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"




//firebase
/************************************FIREBASE ***********************************/

#define    CMD_PATH         "/PC/"
#define    IDLE_PATH         "/"
#define    FB_AV_7SEG           "/AvReceiver_channel"
#define    FB_FREQ              "/AvReceiver_freq" 
#define    FB_T433_CH_NR        "/RcTransmitter_channel"
#define    FB_OTA               "/Ota"
#define    FB_RESET             "/Reboot"
#define    FB_SMS_ON            "/Gsm"
#define    FB_VERSION           "/Version"
#define    FB_FB_OFF            "/Firebase"
#define    FB_BLYNK_ON_OFF      "/Blynk"
#define    FB_WIFI_OFF          "/Wifi"
#define    FB_SETTINGS         "/Settings"
#define    FB_T315_CH_NR        "/RcTransmitter_channel_315"





//BLYNK
/*****e*******************************BLYNK***********************************/
#define    FB_FREQ_ID           0
#define    FB_T433_CH_NR_ID     1
#define    FB_AV_7SEG_ID        2
#define    ROOM_201_TO_205      3
#define    FB_OTA_ID            7
#define    FB_RESET_ID          8
#define    FB_FB_OFF_ID         10
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



#define    FB_VERSION_ID        307
#define    FB_BLYNK_ON_OFF_ID   309
#define    FB_WIFI_OFF_ID       310
#define    FB_SETTINGS_ID       311
#define    IDLE_PATH_ID        312
#define    FB_ZAP_CHANNEL_ID   316





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
#define    FB_NOTIFIER      "PC/FB_SMS"



const String FB_T433_STATUS_PATH[32] =
{"","PC/T433_CH1_ST","PC/T433_CH2_ST","PC/T433_CH3_ST","PC/T433_CH4_ST","PC/T433_CH5_ST","PC/T433_CH6_ST",
"PC/T433_CH7_ST","PC/T433_CH8_ST","PC/T433_CH9_ST","PC/T433_CH10_ST","PC/T433_CH11_ST","PC/T433_CH12_ST",
"PC/T433_CH13_ST","PC/T433_CH14_ST","PC/T433_CH15_ST","PC/T315_CH1_ST","PC/T315_CH2_ST","PC/T315_CH3_ST",
"PC/T315_CH4_ST","PC/T315_CH5_ST","PC/T315_CH6_ST","PC/T315_CH7_ST","PC/T315_CH8_ST","PC/T315_CH9_ST",
"PC/T315_CH10_ST","PC/T315_CH11_ST","PC/T315_CH12_ST","PC/T315_CH13_ST","PC/T315_CH14_ST","PC/T315_CH15_ST",""};


//av
  #define AV_OUTPUT_AN    34
  #define RC_TX_PIN       15
  #define BLU_LED         13 

  #define NETGEER_PIN      2

#define PLL_ADDRESS             0x61  //0110 0 001 becomes 0xC2
#define PCF8574_CMD_ADDRESS     0x20           // PCF8574 device 1    Commands
#define PCF8574_STATUS_ADDRESS  0x21          // PCF8574 device 2    7 Segment Display Reader
   
#define   RC_CODE_LENGTH  24
#define   AV_CH_UP        5
#define   AV_CH_DOWN      6
#define   AV_FREQ         1
#define   AV_FREQ_UP      2
#define   AV_FREQ_DOWN    3
#define   ON              true
#define   OFF             false



//utilities
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


//ota
// TTGO T-Call pin definitions
#define MODEM_RST            5
#define I2C_SDA              21
#define I2C_SCL              22
#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00
#define BAUDRATE            38400
#define EEPROM_GITHUB_ADD   80
#define EEPROM_WIFI_ADD     83
