#ifndef SAFEMODE_H
#define SAFEMODE_H

int inactivityPowerOffTimer  = (1L * 60UL ) ; //60 Min;
int inactivityRestartTimer  = (10L * 60UL ) ; //600 Min;

unsigned long lastCheck = 0;
extern uint32_t lastActivityTime;


RTC_DATA_ATTR int crashCount = 0;

#define MAX_CRASH_COUNT 5

bool safeMode = false;

// --------------------------------------------------
// Called whenever activity is detected
// --------------------------------------------------
void resetInactivityTimer()
{
    lastActivityTime = millis();
}

// --------------------------------------------------
// Detect reboot reason
// --------------------------------------------------

void checkCrashCounter()
{
    esp_reset_reason_t reason = esp_reset_reason();

    Serial.print("Reset reason: ");
    Serial.println(reason);

    // ------------------------------------------
    // Count only abnormal resets
    // ------------------------------------------

    if (
        reason == ESP_RST_PANIC ||
        reason == ESP_RST_TASK_WDT ||
        reason == ESP_RST_INT_WDT ||
        reason == ESP_RST_WDT ||
        reason == ESP_RST_BROWNOUT
    )
    {
        crashCount++;
    }
    else
    {
        // Normal reboot -> clear crashes
        crashCount = 0;
    }

    Serial.print("Crash count: ");
    Serial.println(crashCount);

    // ------------------------------------------
    // Enter safe mode
    // ------------------------------------------

    if (crashCount >= MAX_CRASH_COUNT)
    {
        safeMode = true;
    }
}

// --------------------------------------------------

void normalModeSetup()
{
     Serial.println("NORMAL MODE");
     wifiAvailable = myBlynk.wifi_init();
     relaySetup();
     if(wifiAvailable)
        {
            awsConnected = connectAWS();
            blynkConnected = blynkInit();
            myBlynk.blynkTimers();
        }
     timersMillis();    
     Serial.print("Version: ");     Serial.println(VERSION_ID);
     Serial.print("AWS IOT This is: ");     Serial.println(THINGNAME); 
     resetInactivityTimer();
}

// --------------------------------------------------

void safeModeSetup()
{
    Serial.println("SAFE MODE");

    // Minimal startup only

    wifiAvailable = myBlynk.wifi_init();
    delay(1000);
    otaWifiGithub= false;         
    OtaTimeoutTimer = millis();
    gitHubURL  = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/SAFE/ESP/ESP.ino.esp32.bin" ;
    OtaGithub();
                   
    // No Blynk
    // No AWS
    // No relays
    // No heavy tasks

    // Could start:
    // OTA
    // Web server
    // Diagnostic page
}

// --------------------------------------------------

void safeModeCheck()
{
    checkCrashCounter();

    if (safeMode)
    {
        safeModeSetup();
    }
    else
    {
        normalModeSetup();
    }
}

void safeModeLoop()
{
    if (safeMode)
    {
        // Recovery mode

        static uint32_t lastMsg = 0;

        if (millis() - lastMsg > 5000)
        {
            lastMsg = millis();

            Serial.println("SAFE MODE ACTIVE");
        }

        return;
    }
}  
// --------------------------------------------------






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
              if ((uint32_t)(millis() - lastActivityTime) >= PowerOffTimer &&  stateDVR == DVR_ON && !dvrSleep)//INACTIVITY_TIMEOUT_MS_POWER_OFF)
              {
                  inactivityVideoPowerOff();
              }
              
              if ((uint32_t)(millis() - lastActivityTime) >= RestartTimer &&  stateDVR == DVR_OFF && dvrSleep)//INACTIVITY_TIMEOUT_MS_RESTART)
              {
                  ESP.restart();
              }
        }
        
        else if (blynkActive && dvrSleep && stateDVR == DVR_OFF )  videoPowerOnAfterSleep(); //Check every second activity to power on after sleep
    }
}





#endif
