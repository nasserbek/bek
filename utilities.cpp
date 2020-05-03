#include "Utilities.h"
#include "headers.h"

ntpServerUtil::ntpServerUtil(void) 
  {
  }


void ntpServerUtil::printLocalTime(boolean ver)
{
  if (ver)
  {
    char tm_mon[10];
    char tm_day[3];
    char tm_year[10];
    char tm_min[3];
    char tm_hour[3];
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if(!getLocalTime(&timeinfo))
      {
        DEBUG_PRINTLN("Failed to obtain time");
        return;
      }
    strftime(tm_day,3,  "%d", &timeinfo);
    strftime(tm_mon,10, "%B", &timeinfo);
    strftime(tm_year,10,"%Y", &timeinfo);
    strftime(tm_hour,3, "%H", &timeinfo);
    strftime(tm_min,3,  "%M", &timeinfo);
    if (EEPROM.read(EEPROM_GITHUB_ADD))
     {
      dateAndTimeStr ="GHB " + String (tm_day)+ " " + String (tm_mon)+ " " + String (tm_year)+ " "  + String (tm_hour)+ ":" + String (tm_min);
      EEPROM.write(EEPROM_GITHUB_ADD, 0); EEPROM.commit();
     }
    else dateAndTimeStr ="IDE " + String (tm_day)+ " " + String (tm_mon)+ " " + String (tm_year)+ " "  + String (tm_hour)+ ":" + String (tm_min);
  }
  sendDateTime(ver);
}

void ntpServerUtil::sendDateTime(boolean Ver)
{
  int len = sizeof(dateAndTimeStr); int i;
  byte eepromByte[EEPROM_VER_LEN];
  //char  dateAndTimeChar[EEPROM_VER_LEN];
  
  if (Ver)
  {
   dateAndTimeStr.toCharArray(dateAndTimeChar, EEPROM_VER_LEN);
   for (i=EEPROM_VER_ADD;  i<EEPROM_VER_ADD+EEPROM_VER_LEN ;  i++)  //From 0 to 31 for RC cmds
    {
     eepromByte[i-EEPROM_VER_ADD] = byte (dateAndTimeChar[i-EEPROM_VER_ADD]);
     EEPROM.write(i, eepromByte[i-EEPROM_VER_ADD]); EEPROM.commit();
    }
   DEBUG_PRINT("Write to eeprom: ");DEBUG_PRINTLN(dateAndTimeStr);
  }

  else 
   {
    for (i=EEPROM_VER_ADD;  i<EEPROM_VER_ADD+EEPROM_VER_LEN ;  i++)
    {
     eepromByte[i-EEPROM_VER_ADD] = EEPROM.read(i);
     dateAndTimeChar[i-EEPROM_VER_ADD] = char (eepromByte[i-EEPROM_VER_ADD]) ;
    }
    dateAndTimeStr  = String (dateAndTimeChar); 
    DEBUG_PRINT("Read from Eeprom: ");DEBUG_PRINTLN(dateAndTimeStr);
    } 
}
  

bool ntpServerUtil::systemTimer(bool _condition , unsigned long &_prevMillis, int _intervalSec)
{
    if (_condition && (  (millis() - _prevMillis) > (_intervalSec*1000) ) )
    {
     _prevMillis = millis()  ;
     return true;
    }
    else return false;
}  
