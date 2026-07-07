// setup.h

#ifndef SETUP_H
#define SETUP_H
extern void loadCrashCount();
extern uint32_t crashCount;

int card = 0;
extern IPAddress blynkLocalServer;
uint64_t chipID;

//struct EspInfo
//{
//  uint32_t chipID;
//  const char* token;
//  byte espNumber;
//};

struct ESPInfo {
    uint64_t chipID;
    const char *token;
    byte espNumber;
};

ESPInfo espTable[] =
{
  {0xD5AD0F,  "2NVzjDY96Cbam0_TxJqTVSsgI7LgWq0_", 1},
  {0xD5C2FB,  "n77QtZp08I7AOG8AcCpBhxJle1S6GXa0", 2},
  {0x6FDF948C, "lsH8XwzGGUUneZTqYMN-5_hfx8YepjjY", 3},
  {0x80AB6FDF948C,  "R45hOtUvRUsELLwghdmLxGO8AJsci0Z5", 4},
  {0x234567,  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 5},
  {0x345678,  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 6},
  {0x456789,  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 7},
  {0x56789A,  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 8},
  {0x6789AB,  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 9},
  {0x789ABC,  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 10}
};

const char* blynkAuthToken = nullptr;
byte espNumber = 0;

uint64_t identifyBoard()
{
  uint64_t mac = ESP.getEfuseMac();

  Serial.printf("Full eFuse MAC : %012llX\n", mac);
  Serial.printf("High 16 bits   : %04X\n", (uint16_t)(mac >> 32));
  Serial.printf("Low 32 bits    : %08X\n", (uint32_t)mac);
  
  for (int i = 0; i < sizeof(espTable) / sizeof(espTable[0]); i++)
  {
    if (espTable[i].chipID == mac)
    {
      blynkAuthToken = espTable[i].token;
      espNumber = espTable[i].espNumber;

      Serial.printf("Esp %d detected\n", espNumber);
      return mac;
    }
  }

  Serial.println("ERROR: Unknown ESP board!");
  return 0;
}


bool blynkInit(void)
{
  StaticJsonDocument<54> doc; //Json to send from
  String str = VERSION_ID  + "  ...  " + WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI());
  blynkConnected = myBlynk.init();
  if (blynkConnected)
  {
    myBlynk.sendAvRxIndex(Av_Rx);
    myBlynk.streamSelect("ch01");
    dvrOnOff (POWER_ON);
    int rssi = WiFi.RSSI();
    myBlynk.wifiRSSI(WiFi.RSSI());

    myBlynk.sendVersion(VERSION_ID);
    loadCrashCount();
    myBlynk.TerminalPrint("Connecion Stablished " + VERSION_ID + " " + String(crashCount) + " Craches" );
  }

//  awsTerminal(awsConnected, str ) ;
  return  blynkConnected;
}

tm printLocalTime() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    DEBUG_PRINTLN("Failed to obtain time");

    // return empty structure
    struct tm emptyTime = {};
    return emptyTime;
  }

  char buf[64];
  strftime(buf, sizeof(buf), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  DEBUG_PRINTLN(buf);

  return timeinfo;
}


void gpioSetup(void)
{
  DIP1 = CommonPins[0];
  DIP2 = CommonPins[1];
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);
  delay(200); // let signals stabilize
  // Read switches
  int b0 = !digitalRead(DIP1); // invert because pullup
  int b1 = !digitalRead(DIP2);

  // Convert to mode number
  card = (b1 << 1) | b0;

  // Configure time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  chipID = identifyBoard();
  if (chipID != 0)
  {
    Serial.println("Esp Number is: " + String(espNumber) + " Chip ID is: " + String(chipID) + " blynkAuthToken " + String(blynkAuthToken) );
    BOARD            = "Esp" + String(espNumber) ;
    card = espNumber - 1;
  }
  else
  {
    DEBUG_PRINT("Esp Selecter:ESP");
    DEBUG_PRINTLN(card+1);
  }

  switch (card) {
    case 0:
      ActiveBoard = ESP1;
      ActiveBoard == ESP1 ;
      BOARD = "ESP1";
      VERSION_ID = " ESP1 ";
      BLYNK_AUTH_TOKEN                = BLYNK_AUTH_TOKEN_ESP1 ; //ESP1
      break;

    case 1:
      ActiveBoard = ESP2;
      BOARD = "ESP2";
      VERSION_ID = " ESP2 ";
      BLYNK_AUTH_TOKEN                = BLYNK_AUTH_TOKEN_ESP2 ; //ESP2
      break;

    case 2:
      ActiveBoard = ESP3;
      BOARD = "ESP3";
      VERSION_ID = " ESP3 ";
      BLYNK_AUTH_TOKEN         =        BLYNK_AUTH_TOKEN_ESP3  ; //ESP3
      break;

    case 3:
      ActiveBoard = TEST4;
      BOARD = "ESP0";
      VERSION_ID = " ESP0 - ";
      BLYNK_AUTH_TOKEN   =              BLYNK_AUTH_TOKEN_TEST ;
      break;
  }

  if (chipID != 0) BLYNK_AUTH_TOKEN                = blynkAuthToken ;

  gitHubURL  = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/ESP.ino.esp32.bin" ; // URL to download the firmware from

  if (ActiveBoard == ESP2) {
    for (int i = 0; i < 8; i++) {
      pinMode(relayPins[i], OUTPUT);
      digitalWrite(relayPins[i], LOW);
    }
    selectRelay(TCA9548A_CH1);// Relay K1 ON

    I2C_SDA           = LilluGoPins[0]; //green
    I2C_SCL           = LilluGoPins[1]; //yellow
    BOARD_LED         = LilluGoPins[2];
    RC_TX_PIN         = LilluGoPins[3];
    AV_RX_DVR_PIN     = LilluGoPins[4];
  }


  else
  {
    I2C_SDA           = Esp32Pins[0]; //green 21
    I2C_SCL           = Esp32Pins[1]; //yellow 22
    BOARD_LED         = Esp32Pins[2]; //5
    RC_TX_PIN         = Esp32Pins[3]; // 15
    AV_RX_DVR_PIN     = Esp32Pins[4]; // 4

  }
  pinMode(AV_RX_DVR_PIN, OUTPUT);
  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, HIGH);
  delay(50); // let signals stabilize
  digitalWrite(AV_RX_DVR_PIN, LOW);  // AV RECEIVER OFF POWER UP NC CONTACT

}



void timersMillis(void)
{
  internetSurvilanceTimer = millis();
  liveTimerOff            = millis();
  OtaTimeoutTimer            = millis();
  ackTimer                = millis();
  restartAfterResetNG     = millis();
  NetgeerResetGooglLostTimer = millis();
  blynkNotActiveTimer     = millis();
  routerResetTimer        = millis();
  resetNetgeerAfterInternetLossTimer = millis();
  Router_24_hoursTimer       = millis();
}

void i2cSetup(void)
{
  bool ta9548a = false;
  Wire.begin(I2C_SDA , I2C_SCL );
  delay(500);
  ta9548a = !TCA9548A(0);
  DEBUG_PRINTLN ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
  if (blynkConnected) myBlynk.TerminalPrint ( ta9548a ? F("TCA9548A Connected") : F("TCA9548A Not Connected"));
}

void resetBoardID(void)
{
  if (ActiveBoard == ESP1 )
  {
    V_Remote_ESP1 = false;
    RC_Remote_ESP1 = false;
  }

  if (ActiveBoard == ESP2 )
  {
    V_Remote_ESP2 = false;
    RC_Remote_ESP2 = false;
  }

  if (ActiveBoard == ESP3 )
  {
    V_Remote_ESP3 = false;
    RC_Remote_ESP3 = false;
  }
}


#endif
