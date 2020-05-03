#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>
#include <EEPROM.h>
#include "sim.h"


class NTPVERSION {
  
  private:

      const char* ntpServer = "pool.ntp.org";
      const long  gmtOffset_sec = 3600;
      const int   daylightOffset_sec = 3600;
      struct      tm timeinfo;
      String      dateAndTimeStr  ;

      
  public:
    char  dateAndTimeChar[40];
    NTPVERSION(void) ;
    void sendDateTime(boolean Ver);
    void printLocalTime(boolean ver);
};

#endif
