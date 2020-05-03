#ifndef GSM_OTA_H
#define GSM_OTA_H




#include <Arduino.h>


class otaUpload {
  
  private:

  public:
 
    otaUpload(void) ;
    void init(bool _start);
    void startOtaUpdate(const String& ota_url);
    void FlashingStatus(void);
    bool setPowerBoostKeepOn(int en);
    void printDeviceInfo(void);
    bool parseURL(String url, String& protocol, String& host, int& port, String& uri);
    void httpClient(void);

 };

#endif
