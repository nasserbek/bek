// critical.h

#ifndef CRITICAL_H
#define CRITICAL_H

#define _TIMERINTERRUPT_LOGLEVEL_     4
#include "TimerInterrupt_Generic.h"
ESP32Timer ITimer0(0);


extern void resetRouter(void);



bool IRAM_ATTR TimerHandler0(void * timerNo)
{
  ESP.restart();
  return true;
}

void enableWDG(bool _enable)
  {
   if (_enable) ITimer0.restartTimer();                       //enable interrupt
   else ITimer0.stopTimer();                                //Disable interrupt
  }
  
void resetWdg(void)
  {
   ITimer0.stopTimer();   
   delay(100); 
   ITimer0.restartTimer();                                  
  }
  
void initWDG(int wdtTimeout,bool _enable) 
{
  Serial.print(F("\nStarting TimerInterruptTest on "));
  Serial.println(ARDUINO_BOARD);
  Serial.println(ESP32_TIMER_INTERRUPT_VERSION);
  Serial.println(TIMER_INTERRUPT_GENERIC_VERSION);
  Serial.print(F("CPU Frequency = "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));

  if (ITimer0.attachInterruptInterval(wdtTimeout * 1000, TimerHandler0))
  {
    Serial.print(F("Starting  ITimer0 OK, millis() = "));
    Serial.println(millis());
    ITimer0.restartTimer();  
  }
  else
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
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
              if(autoResetRouter) resetRouter();
          }

/*
bool pingGoogleConnection(void)
{
       bool pingInternet= Ping.ping("www.google.com");
       DEBUG_PRINT("Ping Google: ");DEBUG_PRINTLN(pingInternet ? F("succesiful") : F("failed"));
       return (pingInternet);
}
*/

void internetCheck(void)
{
       if ( (  (millis() - routerResetTimer) >= routerTimer) && routerResetStart)
                {
    
                routerResetStart=false;
                routerResetTimer        = millis();
                restartAfterResetNG     = millis();
                netGeerReset = true;
                blynkInitDone = false;
                pingGoogle =false;
               }

       if ( ( (millis() - resetNetgeerAfterInternetLossTimer) >= INTERNET_LOSS_TO_RESET_NG_TIMER) && InternetLoss && !blynkConnected && !netGeerReset)
        {
              DEBUG_PRINTLN("Blynk Disconnected for 2 min, Reset Netgeer");
 //             ResetNetgeer();
              if(!routerResetStart){routerResetTimer        = millis();routerResetStart = true;DEBUG_PRINTLN("Netgeer Reset done: ");}
        }

       if (  ( (millis() - restartAfterResetNG) >=  RESTART_AFTER_NG_RESET_TIMER) && netGeerReset )
          {
            DEBUG_PRINTLN("Resetaring 30 min after Netgeer Rreset");
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
    //  sms.sim800DvrChOn(false)  ;
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
