#ifndef SAFEMODE_H
#define SAFEMODE_H

int inactivityPowerOffTimer  = (1L * 60UL ) ; //60 Min;
int inactivityRestartTimer  = (10L * 60UL ) ; //600 Min;
bool enableRestart  = false;
unsigned long lastCheck = 0;
extern uint32_t lastActivityTime;

extern bool wifiIde ;  
extern void ArduinoIdeWifi();

Preferences prefs;

uint32_t crashCount = 0;


#define MAX_CRASH_COUNT 12 

bool safeMode = false;

void blueLedFlash()
{
  unsigned long currentMillis = millis();

  if (currentMillis - blueLedPreviousMillis >= blueLedInterval) {
    blueLedPreviousMillis = currentMillis;

    blueLedState = !blueLedState;              // toggle LED
    digitalWrite(BLUE_LED, blueLedState);
  }  
}
void checkSleep()
{
  uint32_t PowerOffTimer  = (inactivityPowerOffTimer * 60UL * 1000UL) ; //inactivityPowerOffTimer in Minutes 1000UL = 1 sec;
  if ((uint32_t)(millis() - lastActivityTime) >= PowerOffTimer  && !blynkActive &&  !zapOnOff && !zapScanOnly)
  {
        Serial.println("Entering light sleep");
        myBlynk.TerminalPrint("Entering light sleep for 10 sec...."); 

        delay(100);
        esp_sleep_enable_timer_wakeup(LIGHT_SLEEP_WAKE);
        esp_light_sleep_start();

        Serial.println("Woke up");
        myBlynk.TerminalPrint("Woke up"); 

  }     

}

void loadCrashCount()
{
    prefs.begin("system", false);

    crashCount = prefs.getUInt("crash", 0);

    Serial.print("Loaded crash count: ");
    Serial.println(crashCount);
}

// --------------------------------------------------

void saveCrashCount()
{
    prefs.putUInt("crash", crashCount);

    Serial.print("Saved crash count: ");
    Serial.println(crashCount);
}

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
void printResetReason()
{
    esp_reset_reason_t reason = esp_reset_reason();

    Serial.print("Reset reason code: ");
    Serial.println((int)reason);

    Serial.print("Meaning: ");

    switch(reason)
    {
        case ESP_RST_POWERON:   Serial.println("POWERON"); break;
        case ESP_RST_SW:        Serial.println("SW RESET"); break;
        case ESP_RST_PANIC:     Serial.println("PANIC"); break;
        case ESP_RST_TASK_WDT:  Serial.println("TASK WDT"); break;
        case ESP_RST_INT_WDT:   Serial.println("INT WDT"); break;
        case ESP_RST_WDT:       Serial.println("WDT"); break;
        case ESP_RST_BROWNOUT:  Serial.println("BROWNOUT"); break;
        case ESP_RST_DEEPSLEEP: Serial.println("DEEPSLEEP"); break;
        default:                Serial.println("OTHER"); break;
    }
}

void checkCrashCounter()
{

    loadCrashCount();

    esp_reset_reason_t reason = esp_reset_reason();

    Serial.print("Reset reason: ");
    Serial.println((int)reason);

    bool crashDetected = false;

    switch(reason)
    {
        case ESP_RST_PANIC:
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:
        case ESP_RST_BROWNOUT:

            crashDetected = true;
        break;
        
        case ESP_RST_POWERON:
              crashCount = 0;
              saveCrashCount();
        break;
        
        default:
            break;
    }

    if (crashDetected)
    {
        crashCount++;

        saveCrashCount();
    }

    Serial.print("Crash count: ");
    Serial.println(crashCount);

    // --------------------------------------------------
    // SAFE MODE
    // --------------------------------------------------

    if (crashCount >= 10)
    {
        Serial.println("SAFE MODE");
        safeMode = true;
        // minimal startup only
        return;
    }

    Serial.println("NORMAL MODE");

    printResetReason();
}

// --------------------------------------------------

void normalModeSetup()
{
     Serial.println("NORMAL MODE");
     
     wifiAvailable = myBlynk.wifi_init();
     
     relaySetup();

     blynkConnected = blynkInit();
     myBlynk.blynkTimers();
     
     timersMillis();    
     Serial.print("Version: ");     Serial.println(VERSION_ID);
     Serial.print("AWS IOT This is: ");     Serial.println(THINGNAME); 
     resetInactivityTimer();
}

// --------------------------------------------------
void activateLocalWifiWeb()
{
       wifiWebUpdater = false;
       crashCount = 0;
       saveCrashCount();
       OtaTimeoutTimer = millis();
       localWebWifiOta ();  
}

void activateWifiIde()
{
                 wifiIde = false;  
                 crashCount = 0;
                 saveCrashCount();       
                 OtaTimeoutTimer = millis();
                 ArduinoIdeWifi();  
}
void safeModeSetup()
{
    Serial.println("SAFE MODE");
    unsigned long startConnecting = millis();
    // Minimal startup only
       wifiAvailable = myBlynk.wifi_init();
       if(wifiAvailable)activateLocalWifiWeb();

       else while (!myBlynk.wifi_init()) {
          Serial.println("Wifi Disconnected");
          wifiAvailable = myBlynk.wifi_init();
          
          blueLedFlash() ; 
          
          if (millis() > startConnecting + WIFI_DISCONNECTED_RESTART) {
            wifiAvailable = false;
            Serial.println("Unable to connect to wifi, restarting ");
            ESP.restart();
            break;
          }
    }
                     
//    wifiAvailable = myBlynk.wifi_init();
//    delay(1000);
//    otaWifiGithub= false;         
//    OtaTimeoutTimer = millis();
//    gitHubURL  = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/SAFE/ESP/ESP.ino.esp32.bin" ;
//    OtaGithub();
                   
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
   if (millis() > 300000) // 5 minutes stable
    {
        crashCount = 0;
        saveCrashCount();
    }
    
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
  struct tm now = printLocalTime();
  String hourMin = String(now.tm_hour) + ":" + String(now.tm_min);
  myBlynk.TerminalPrint   (hourMin +":Turning Off Video for non activity for 1 Hour.."); 
//  myBlynk.sendNotify      (hourMin +":Turning Off Video for non activity for 1 Hour..");
  dvrOnOff (POWER_OFF);
  dvrSleep = true;  
}

void videoPowerOnAfterSleep()
{
  struct tm now = printLocalTime();
  String hourMin = String(now.tm_hour) + ":" + String(now.tm_min);
  myBlynk.TerminalPrint (hourMin +":Turning On Video for activity after sleep..");  
//  myBlynk.sendNotify    (hourMin +":Turning On Video for activity after sleep..");
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
                 if(enableRestart) ESP.restart();
              }
        }
        
        else if (dvrSleep && stateDVR == DVR_OFF)
        {
            if( blynkActive )  videoPowerOnAfterSleep(); //Check every second activity to power on after sleep  
            resetInactivityTimer();     
        }
    }
}





#endif
