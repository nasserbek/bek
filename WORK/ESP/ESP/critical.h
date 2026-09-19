// critical.h

#ifndef CRITICAL_H
#define CRITICAL_H


#define _TIMERINTERRUPT_LOGLEVEL_     4
#include "ESP32TimerInterrupt.h"
ESP32Timer ITimer0(0);
extern void resetRouter(void);
extern void activateLocalWifiWeb();
extern void resetInternetLoss();
extern void activateWifiIde();


/***********************************************************************************/

void resetWdg()
{
   ITimer0.stopTimer();   
   delay(100); 
   ITimer0.restartTimer();  
}

/*********************************************************************/


void IRAM_ATTR TimerHandler0()
{
    ESP.restart();
}


void enableWDG(bool _enable)
  {
   if (_enable) ITimer0.restartTimer();                       //enable interrupt
   else ITimer0.stopTimer();                                //Disable interrupt
  }
  

  
void initWDG(int wdtTimeout,bool _enable) 
{
  DEBUG_PRINT(F("\nStarting TimerInterruptTest on "));
  DEBUG_PRINTLN(ARDUINO_BOARD);
  DEBUG_PRINTLN(ESP32_TIMER_INTERRUPT_VERSION);
  DEBUG_PRINT(F("CPU Frequency = "));
  DEBUG_PRINT(F_CPU / 1000000);
  DEBUG_PRINTLN(F(" MHz"));


    if (ITimer0.attachInterruptInterval((uint64_t)wdtTimeout * 1000, TimerHandler0))
    {
      DEBUG_PRINT(F("Starting ITimer0 OK, millis() = "));
      DEBUG_PRINTLN(millis());
    }
    else
    {
      DEBUG_PRINTLN(F("Can't set ITimer0"));
    }
}


void rebootSw(void)
{
 ESP.restart();
}



void ResetNetgeer(void)
          {
              if(!routerResetStart)
              {
                routerResetTimer        = millis();
                routerResetStart = true;
                DEBUG_PRINTLN("Netgeer Reset done: ");
                myBlynk.TerminalPrint("RESTARTING ROUTER...");
              }
         //     if(autoResetRouter) resetRouter();
          }



void internetCheck(void)
{
       if (  ( (millis() - restartAfterResetNG) >=  RESTART_AFTER_NG_RESET_TIMER) && InternetLoss && !blynkConnected )//&& netGeerReset )
          {
            Serial.printf(
                          "Restarting %lu minutes after Internet or Blynk loss.\n",
                          RESTART_AFTER_NG_RESET_TIMER / (60UL * 1000UL)
                      );
            netGeerReset = false;
            resetInternetLoss();
            ESP.restart(); 
          }

}     



int stringToInteger(String str)
{
char carray[5]; 
      str.toCharArray(carray, sizeof(carray));
      return ( atoi(carray));  
}

void goToDeepSleep(int sleepTimer)
{
    //  sendToHMI("Going to Deep Sleep", "Going to Deep Sleep", "Going to Deep Sleep",FB_NOTIFIER, "Going to Deep Sleep" );
      DEBUG_PRINT("Sleep for: ");  DEBUG_PRINT(sleepTimer * 60* 1000000);DEBUG_PRINTLN(" uSec");
      esp_sleep_enable_timer_wakeup(sleepTimer * 60 * 1000000); // in microseconds
      Serial.flush(); 
      esp_deep_sleep_start();
}

void looadRoomData()
{
  int freq;
       
 for(byte i=1;i<21;i++)
          {
            videoCh[i].frequency = freqTable[i];
            freq=videoCh[i].frequency;
            roomId[i].vCh  = roomId[i].rCh = videoCh[i].id = i;
            _pll[i] = ( 512 * (freq + 479.5) ) / 64 ;
          }
}

void createHandleGroup()
{
     //Create a program that allows the required message objects and group flags
    g_event_queue_handle = xQueueCreate(50, sizeof(int)); // Creates a queue of 50 int elements
    g_event_group = xEventGroupCreate();
}
/*************************************************END OF DON'T TOUCH*********************************************************************************************/
    

#endif
