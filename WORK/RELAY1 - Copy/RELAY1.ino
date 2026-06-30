#include "MainCalls.h"

void BlynkTimersInit()
{
  
}
void setup()
{
      Serial.begin(115200);
      initWDG(MIN_5,EN);
      pinMode(RELAY_PIN, OUTPUT);
      digitalWrite(RELAY_PIN, HIGH);
      wifiAvailable = wifi_connect();
      
      chipID = identifyBoard();
      Serial.println("Relay Numer is: " + String(relayNumber) + " Chip ID is: " + String(chipID) + " blynkAuthToken " + String(blynkAuthToken) );
      BOARD            = "Relay" + String(relayNumber) ;
      internetConnected = checkInternet();
      if (internetConnected)
      {
          configTzTime(
              "CET-1CEST,M3.5.0/2,M10.5.0/3",
              "pool.ntp.org",
              "time.nist.gov"
          );
      
          getTimeDate();
      }
      blynkConnected = blynk_muliservers_connect();
      timer.setInterval(blynkIntervalInterval, checkBlynk);   // check connection to server per blynkIntervalInterval
      lastActivityTime = millis();
      resetInternetLoss();
      digitalWrite(RELAY_PIN, LOW );
}

void loop()
{
    resetWdg();
    internetCheck();      
    if(!internetConnected) internetConnected = checkInternet();  // Ping Github to check internet
    blynkLoop();
}
