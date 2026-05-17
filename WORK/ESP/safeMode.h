#ifndef SAFEMODE_H
#define SAFEMODE_H

int inactivityPowerOffTimer  = (1L * 60UL ) ; //60 Min;
int inactivityRestartTimer  = (10L * 60UL ) ; //600 Min;

unsigned long lastCheck = 0;
extern uint32_t lastActivityTime;
//extern bool blynkActive;
//extern bool zapOnOff;
//extern bool zapScanOnly;
//extern int stateDVR;
//extern bool dvrSleep;
//extern void dvrOnOff (bool powerOn); 


void inactivityVideoPowerOff()
{
  myBlynk.TerminalPrint("Turning Off Video for non activity for 1 Hour.."); 
  dvrOnOff (POWER_OFF);
  dvrSleep = true;  
}

void videoPowerOnAfterSleep()
{
      myBlynk.TerminalPrint("Turning On Video after sleeping..."); 
      dvrOnOff (POWER_ON);
      dvrSleep = false; 
}

// --------------------------------------------------
// Called whenever activity is detected
// --------------------------------------------------
void resetInactivityTimer()
{
    lastActivityTime = millis();
}


//This checks once per second only.
void restartForInactivity()
{
  uint32_t PowerOffTimer  = (inactivityPowerOffTimer * 60UL * 1000UL) ; //inactivityPowerOffTimer in Minutes ;
  uint32_t RestartTimer  =  (inactivityRestartTimer  * 60UL * 1000UL) ; //inactivityRestartTimer in Minutes ;
  if (millis() - lastCheck >= 1000)
    {
        lastCheck = millis();
        
        if(!blynkActive &&  !zapOnOff && !zapScanOnly)
        {
              if ((uint32_t)(millis() - lastActivityTime) >= inactivityPowerOffTimer)//INACTIVITY_TIMEOUT_MS_POWER_OFF)
              {
                  inactivityVideoPowerOff();
              }
              
              if ((uint32_t)(millis() - lastActivityTime) >= inactivityRestartTimer)//INACTIVITY_TIMEOUT_MS_RESTART)
              {
                  ESP.restart();
              }
        }
        
        else if (blynkActive && dvrSleep && stateDVR == DVR_OFF )  videoPowerOnAfterSleep(); //Check every second activity to power on after sleep
    }
}





#endif
