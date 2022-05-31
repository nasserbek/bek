#include <Update.h>
#include <HTTPClient.h>
//________________________________________________________________
String overTheAirURL = "";

void reboot()
{
#if defined(ARDUINO_ARCH_MEGAAVR)
  wdt_enable(WDT_PERIOD_8CLK_gc);
#elif defined(__AVR__)
  wdt_enable(WDTO_15MS);
#elif defined(__arm__)
  NVIC_SystemReset();
#elif defined(ESP8266) || defined(ESP32)
  ESP.restart();
#else
  #error "MCU reset procedure not implemented"
#endif
  for (;;) {}
}

BLYNK_WRITE(InternalPinOTA) {
  overTheAirURL = param.asString();
  HTTPClient http;
  http.begin(overTheAirURL);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {return;}
  int contentLength = http.getSize();
  if (contentLength <= 0) {return; }
  bool canBegin = Update.begin(contentLength);
  if (!canBegin) { return;}
  Client& client = http.getStream();
  int written = Update.writeStream(client);
  if (written != contentLength) {return;}
  if (!Update.end()) {return;}
  if (!Update.isFinished()) {return;}
reboot();
}
