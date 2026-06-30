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
      blynkAuthToken = BLYNK_AUTH_TOKEN_RELAY;
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
