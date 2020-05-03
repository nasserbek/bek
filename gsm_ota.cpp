#include "gsm_ota.h"
#include "headers.h"

#define DEBUG
#define FREE
#include "av.h"

String _overTheAirURL = "https://raw.githubusercontent.com/nasserbek/bek/master/BEKIOT.bin";  // URL to download the firmware from

#ifdef FREE
const char apn[]      = "free"; // Your APN)
#else
const char apn[]      = "sl2sfr"; // Your APN
#endif

const char gprsUser[] = ""; // User
const char gprsPass[] = ""; // Password
const char simPIN[]   = ""; // SIM card PIN code, if any
unsigned long _prevMillis = 0;
int           _interval = 500;
bool          _flashStatus = false;
reciever led;

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

// Define the serial console for debug prints, if needed
//#define TINY_GSM_DEBUG SerialMon
//#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>
#include <Update.h>
#include <EEPROM.h>

#include <HTTPClient.h>
#include <WiFi.h>

TinyGsm modem(SerialAT);


#ifdef DEBUG
#define DEBUG_PRINT(...) { SerialMon.print(millis()); SerialMon.print(" - "); SerialMon.println(__VA_ARGS__); }
#define DEBUG_FATAL(...) { SerialMon.print(millis()); SerialMon.print(" - FATAL: "); SerialMon.println(__VA_ARGS__); delay(10000); ESP.restart(); }
#else
#define DEBUG_PRINT(...) 
#define DEBUG_FATAL(...)
#endif




OTAGSM::OTAGSM()
{

}

void OTAGSM::init(bool _start)
{
  //SerialMon.setDebugOutput(true);
  printDeviceInfo();

  // Keep power when running from battery
  Wire.begin(I2C_SDA, I2C_SCL);
  bool   isOk = setPowerBoostKeepOn(1);
  DEBUG_PRINT(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  DEBUG_PRINT("  Firmware A is running");
  DEBUG_PRINT("--------------------------");
  DEBUG_PRINT(F("Starting OTA update in 10 seconds..."));
  delay(10000);

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(BAUDRATE , SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DEBUG_PRINT(F("Initializing modem..."));
  modem.restart();
  // Or, use modem.init() if you don't need the complete restart

  String modemInfo = modem.getModemInfo();
  DEBUG_PRINT(String("Modem: ") + modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }

  DEBUG_PRINT(F("Waiting for network..."));
  if (!modem.waitForNetwork(240000L)) {
    DEBUG_FATAL(F("Network failed to connect"));
  }

 
  DEBUG_PRINT(F("Connecting to GPRS"));
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    DEBUG_FATAL(F("APN failed to connect"));
  }
  else  DEBUG_PRINT(F("GPRS Connected successifully"));
  if (_start) startOtaUpdate(_overTheAirURL);  
}

void OTAGSM::startOtaUpdate(const String& ota_url)
{
  String protocol, host, url;
  int port;

  if (!parseURL(ota_url, protocol, host, port, url)) {
    DEBUG_FATAL(F("Cannot parse URL"));
  }

  DEBUG_PRINT(String("Connecting to ") + host + ":" + port);

  Client* client = NULL;
  if (protocol == "http") {
    client = new TinyGsmClient(modem);
    if (!client->connect(host.c_str(), port)) {
      DEBUG_FATAL(F("Client not connected"));
    }
  } else if (protocol == "https") {
    client = new TinyGsmClientSecure(modem);
    if (!client->connect(host.c_str(), port)) {
      DEBUG_FATAL(F("Client not connected"));
    }
  } else {
    DEBUG_FATAL(String("Unsupported protocol: ") + protocol);
  }
  
  DEBUG_PRINT(String("Requesting ") + url);

  client->print(String("GET ") + url + " HTTP/1.0\r\n"
               + "Host: " + host + "\r\n"
               + "Connection: keep-alive\r\n"
               + "\r\n");

  long timeout = millis();
  while (client->connected() && !client->available()) {
    if (millis() - timeout > 10000L) {
      DEBUG_FATAL("Response timeout");
    }
  }

  // Collect headers
  String md5;
  int contentLength = 0;

  while (client->available()) {
    String line = client->readStringUntil('\n');
    line.trim();
    //DEBUG_PRINT(line);    // Uncomment this to show response headers
    line.toLowerCase();
    if (line.startsWith("content-length:")) {
      contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
    } else if (line.startsWith("x-md5:")) {
      md5 = line.substring(line.lastIndexOf(':') + 1);
    } else if (line.length() == 0) {
      break;
    }
  }

  if (contentLength <= 0) {
    DEBUG_FATAL("Content-Length not defined");
  }

  bool canBegin = Update.begin(contentLength);
  if (!canBegin) {
    Update.printError(SerialMon);
    DEBUG_FATAL("OTA begin failed");
  }

  if (md5.length()) {
    DEBUG_PRINT(String("Expected MD5: ") + md5);
    if(!Update.setMD5(md5.c_str())) {
      DEBUG_FATAL("Cannot set MD5");
    }
  }

  
  
  DEBUG_PRINT("Flashing...");

  // The next loop does approx. the same thing as Update.writeStream(http) or Update.write(http)

  int written = 0;
  int progress = 0;
  uint8_t buff[256];
  while (client->connected() && written < contentLength) {
    FlashingStatus();
    timeout = millis();
    while (client->connected() && !client->available()) {
      delay(1);
      if (millis() - timeout > 10000L) {
        DEBUG_FATAL("Timeout");
      }
    }

    int len = client->read(buff, sizeof(buff));
    if (len <= 0) continue;
    Update.write(buff, len);
    written += len;

    int newProgress = (written*100)/contentLength;
    if (newProgress - progress >= 5 || newProgress == 100) {
      progress = newProgress;
 //     SerialMon.print(String("\r ") + progress + "%");
    }
  }
  DEBUG_PRINT();

  if (written != contentLength) {
    Update.printError(SerialMon);
    DEBUG_FATAL(String("Write failed. Written ") + written + " / " + contentLength + " bytes");
  }

  if (!Update.end()) {
    Update.printError(SerialMon);
    DEBUG_FATAL(F("Update not ended"));
  }

  if (!Update.isFinished()) {
    DEBUG_FATAL(F("Update not finished"));
  }

  DEBUG_PRINT("=== Update successfully completed. Rebooting.");
  EEPROM.write(EEPROM_GITHUB_ADD, 1); EEPROM.commit();
  EEPROM.write(EEPROM_WIFI_ADD, 1); EEPROM.commit(); //To enable Wifi and get NTP for version date
  ESP.restart();   
}

void OTAGSM::FlashingStatus(void)
{
    if (millis() - _prevMillis > _interval) 
    {
     _flashStatus = !_flashStatus;
     _prevMillis = millis()  ;  
     }
       if(_flashStatus) led.bluLed(ON);
       else led.bluLed(OFF);
}

bool OTAGSM::setPowerBoostKeepOn(int en)
{
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en) {
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    Wire.write(0x35); // 0x37 is default reg value
  }
  return Wire.endTransmission() == 0;
}

void OTAGSM::printDeviceInfo()
{
  DEBUG_PRINT();
  DEBUG_PRINT("--------------------------");
  DEBUG_PRINT(String("Build:    ") +  __DATE__ " " __TIME__);
  DEBUG_PRINT(String("Flash:    ") + ESP.getFlashChipSize() / 1024 + "K");
  DEBUG_PRINT(String("ESP sdk:  ") + ESP.getSdkVersion());
  DEBUG_PRINT(String("Chip rev: ") + ESP.getChipRevision());
  DEBUG_PRINT(String("Free mem: ") + ESP.getFreeHeap());
  DEBUG_PRINT("--------------------------");
}

bool OTAGSM::parseURL(String url, String& protocol, String& host, int& port, String& uri)
{
  int index = url.indexOf(':');
  if(index < 0) {
    return false;
  }

  protocol = url.substring(0, index);
  url.remove(0, (index + 3)); // remove protocol part

  index = url.indexOf('/');
  String server = url.substring(0, index);
  url.remove(0, index);       // remove server part

  index = server.indexOf(':');
  if(index >= 0) {
    host = server.substring(0, index);          // hostname
    port = server.substring(index + 1).toInt(); // port
  } else {
    host = server;
    if (protocol == "http") {
      port = 80;
    } else if (protocol == "https") {
      port = 443;
    }
  }

  if (url.length()) {
    uri = url;
  } else {
    uri = "/";
  }
  return true;
}



 void OTAGSM::httpClient(void) {
 
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin(_overTheAirURL); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
     if (httpCode > 0) { //Check for the returning code
          String payload = http.getString();
         DEBUG_PRINT(httpCode);
         DEBUG_PRINT(payload);
      }
 
    else {
      DEBUG_PRINT("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 
  delay(10000);
 }
