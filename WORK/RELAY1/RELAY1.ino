#include "MainCalls.h"

void BlynkTimersInit()
{
 _blynkIsConnected = false;
  timer.setInterval(LiveUpdateInterval, SendLiveLed);// run some function at intervals per LiveUpdateInterval
  timer.setInterval(blynkIntervalInterval, checkBlynk);   // check connection to server per blynkIntervalInterval
  timer.setInterval(5000L, blinkLedWidget);  
}
void setup()
{
      Serial.begin(115200);
      initWDG(MIN_5,EN);
      pinMode(RELAY_PIN, OUTPUT);
      digitalWrite(RELAY_PIN, HIGH);
      wifiAvailable = wifi_connect();
      BLYNK_AUTH_TOKEN = BLYNK_AUTH_TOKEN_ESP8266_ESP01_1;
      blynkConnected = blynkconnect();
      if(blynkConnected)
      {
        blynkTimers();
        timersMillis();    
        resetInactivityTimer();
      }
      resetInternetLoss();
      BlynkTimersInit();
      
}

void loop()
{
    resetWdg();
    internetCheck();      
    if(!internetConnected) internetConnected = checkInternet();  // Ping Github to check internet
    blynkLoop();
}
