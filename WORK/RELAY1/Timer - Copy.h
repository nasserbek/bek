#ifndef TIMER_H
#define TIMER_H


void TimerHandler0()
{
    ESP.restart();
}


void initWDG(int wdtTimeout, bool enable)
{
    g_wdtTimeout = wdtTimeout;

    if (enable)
    {
        WDGTimer.once_ms(g_wdtTimeout, TimerHandler0);
    }
}

/***********************************************************************************/

void resetWdg()
{
    WDGTimer.detach();                          // stop current timer
    WDGTimer.once_ms(g_wdtTimeout, TimerHandler0); // restart
}


void enableWDG(bool enable)
{
    if (enable)
    {
        WDGTimer.detach();
        WDGTimer.once_ms(g_wdtTimeout, TimerHandler0);
    }
    else
    {
        WDGTimer.detach();
    }
}  



void rebootSw(void)
{
 ESP.restart();
}


void internetCheck(void)
{
       if (  ( (millis() - restartAfterResetNG) >=  RESTART_AFTER_NG_RESET_TIMER) && InternetLoss && !blynkConnected )//&& netGeerReset )
          {
            DEBUG_PRINTLN("Resetaring " + String ( RESTART_AFTER_NG_RESET_TIMER/ (60UL * 1000UL) ) +" min after Internet or Blynk Loss");
            netGeerReset = false;
            resetInternetLoss();
            wifiAvailable = wifi_connect();
            ESP.restart(); 
          }
}     

#endif
