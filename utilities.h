#ifndef UTILITIES_H
#define UTILITIES__H

#include <Arduino.h>
#include <EEPROM.h>



class ntpServerUtil {
  
  private:

      const char* ntpServer = "pool.ntp.org";
      const long  gmtOffset_sec = 3600;
      const int   daylightOffset_sec = 3600;
      struct      tm timeinfo;
      String      dateAndTimeStr  ;

      
  public:
    char  dateAndTimeChar[40];
    ntpServerUtil(void) ;
    void sendDateTime(boolean Ver);
    void printLocalTime(boolean ver);
    bool systemTimer(bool _condition , unsigned long &_prevMillis, int _intervalSec);
};

#endif
