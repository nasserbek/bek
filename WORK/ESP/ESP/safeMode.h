#ifndef SAFEMODE_H
#define SAFEMODE_H

int inactivityPowerOffTimer  = (1L * 60UL ) ; //60 Min;

bool enableRestart  = false;
unsigned long lastCheck = 0;
extern uint32_t lastActivityTime;

extern bool wifiIde ;  
extern void ArduinoIdeWifi();


Preferences prefs;

uint32_t crashCount = 0;


#define MAX_CRASH_COUNT 12 

bool safeMode = false;

// --------------------------------------------------
void getTimeDate()
{
    
    struct tm now = printLocalTime();
    char buildTime[20];
     
    int day, year, hour, minute, second;
    char monthStr[4];

    sscanf(__DATE__, "%s %d %d", monthStr, &day, &year);
    sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);

    int month = 0;

    if      (strcmp(monthStr, "Jan") == 0) month = 1;
    else if (strcmp(monthStr, "Feb") == 0) month = 2;
    else if (strcmp(monthStr, "Mar") == 0) month = 3;
    else if (strcmp(monthStr, "Apr") == 0) month = 4;
    else if (strcmp(monthStr, "May") == 0) month = 5;
    else if (strcmp(monthStr, "Jun") == 0) month = 6;
    else if (strcmp(monthStr, "Jul") == 0) month = 7;
    else if (strcmp(monthStr, "Aug") == 0) month = 8;
    else if (strcmp(monthStr, "Sep") == 0) month = 9;
    else if (strcmp(monthStr, "Oct") == 0) month = 10;
    else if (strcmp(monthStr, "Nov") == 0) month = 11;
    else if (strcmp(monthStr, "Dec") == 0) month = 12;

    sprintf(buildTime,
            "%02d/%02d/%02d %02d:%02d",
            day,
            month,
            year % 100,
            hour,
            minute);
            
    VERSION_ID = BOARD + " " + buildTime;  
}

// --------------------------------------------------
// Called whenever activity is detected
// --------------------------------------------------
void resetInactivityTimer()
{
    lastActivityTime = millis();
}

//void normalModeSetup()
//{
//    DEBUG_PRINTLN("NORMAL MODE");
//    gpioSetup();
//    wifiAvailable = myBlynk.wifi_init();
//    getTimeDate();
//    
//     blynkConnected = blynkInit();
//     myBlynk.blynkTimers();
//     
//     timersMillis();    
//     DEBUG_PRINT("Version: ");     DEBUG_PRINTLN(VERSION_ID);
//     resetInactivityTimer();
//}



void checkSleep()
{
  uint32_t PowerOffTimer  = (inactivityPowerOffTimer * 60UL * 1000UL) ; //inactivityPowerOffTimer in Minutes 1000UL = 1 sec;
  if ((uint32_t)(millis() - lastActivityTime) >= PowerOffTimer  && !blynkActive &&  !zapOnOff && !zapScanOnly)
  {
        DEBUG_PRINTLN("Entering light sleep");
        myBlynk.TerminalPrint("Entering light sleep for 10 sec...."); 

        delay(100);
        esp_sleep_enable_timer_wakeup(LIGHT_SLEEP_WAKE);
        esp_light_sleep_start();

        DEBUG_PRINTLN("Woke up");
        myBlynk.TerminalPrint("Woke up"); 

  }     

}

void loadCrashCount()
{
    prefs.begin("system", false);

    crashCount = prefs.getUInt("crash", 0);

    DEBUG_PRINT("Loaded crash count: ");
    DEBUG_PRINTLN(crashCount);
}

// --------------------------------------------------

void saveCrashCount()
{
    prefs.putUInt("crash", crashCount);

    DEBUG_PRINT("Saved crash count: ");
    DEBUG_PRINTLN(crashCount);
}



// --------------------------------------------------
// Detect reboot reason
// --------------------------------------------------
void printResetReason()
{
    esp_reset_reason_t reason = esp_reset_reason();

    DEBUG_PRINT("Reset reason code: ");
    DEBUG_PRINTLN((int)reason);

    DEBUG_PRINT("Meaning: ");

    switch(reason)
    {
        case ESP_RST_POWERON:   DEBUG_PRINTLN("POWERON"); break;
        case ESP_RST_SW:        DEBUG_PRINTLN("SW RESET"); break;
        case ESP_RST_PANIC:     DEBUG_PRINTLN("PANIC"); break;
        case ESP_RST_TASK_WDT:  DEBUG_PRINTLN("TASK WDT"); break;
        case ESP_RST_INT_WDT:   DEBUG_PRINTLN("INT WDT"); break;
        case ESP_RST_WDT:       DEBUG_PRINTLN("WDT"); break;
        case ESP_RST_BROWNOUT:  DEBUG_PRINTLN("BROWNOUT"); break;
        case ESP_RST_DEEPSLEEP: DEBUG_PRINTLN("DEEPSLEEP"); break;
        default:                DEBUG_PRINTLN("OTHER"); break;
    }
}

void checkCrashCounter()
{

    loadCrashCount();

    esp_reset_reason_t reason = esp_reset_reason();

    DEBUG_PRINT("Reset reason: ");
    DEBUG_PRINTLN((int)reason);

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

    DEBUG_PRINT("Crash count: ");
    DEBUG_PRINTLN(crashCount);

    // --------------------------------------------------
    // SAFE MODE
    // --------------------------------------------------

    if (crashCount >= 10)
    {
        DEBUG_PRINTLN("SAFE MODE");
        safeMode = true;
        // minimal startup only
        return;
    }

    DEBUG_PRINTLN("NORMAL MODE");

    printResetReason();
}




// --------------------------------------------------
void activateLocalWifiWeb()
{
       wifiWebUpdater = false;
       OtaTimeoutTimer = millis();
       localWebWifiOta ();  
}

void activateWifiIde()
{
                 wifiIde = false;  
                 OtaTimeoutTimer = millis();
                 ArduinoIdeWifi();  
}

//void safeModeSetup()
//{
//    DEBUG_PRINTLN("SAFE MODE");
//    unsigned long startConnecting = millis();
//    // Minimal startup only
//       wifiAvailable = myBlynk.wifi_init();
//       if(wifiAvailable)activateLocalWifiWeb();
//
//       else while (!myBlynk.wifi_init()) {
//          DEBUG_PRINTLN("Wifi Disconnected");
//          wifiAvailable = myBlynk.wifi_init();
//          
//          
//          
//          if (millis() > startConnecting + WIFI_DISCONNECTED_RESTART) {
//            wifiAvailable = false;
//            DEBUG_PRINTLN("Unable to connect to wifi, restarting ");
//            ESP.restart();
//            break;
//          }
//    }
//                     
////    wifiAvailable = myBlynk.wifi_init();
////    delay(1000);
////    otaWifiGithub= false;         
////    OtaTimeoutTimer = millis();
////    gitHubURL  = "https://raw.githubusercontent.com/nasserbek/bek/master/WORK/ESP/SAFE/ESP/ESP.ino.esp32.bin" ;
////    OtaGithub();
//                   
//    // No Blynk
//    // No AWS
//    // No relays
//    // No heavy tasks
//
//    // Could start:
//    // OTA
//    // Web server
//    // Diagnostic page
//}

// --------------------------------------------------

//void safeModeCheck()
//{
//    checkCrashCounter();
//
//    if (safeMode)
//    {
//        safeModeSetup();
//    }
//    else
//    {
//        normalModeSetup();
//    }
//}

bool stable = false;

//void safeModeLoop()
//{
//   if (millis() > 300000 && !stable) // 5 minutes stable
//    {
//        stable = true;
//        crashCount = 0;
//        saveCrashCount();
//    }
//    
//    if (safeMode)
//    {
//        // Recovery mode
//
//        static uint32_t lastMsg = 0;
//
//        if (millis() - lastMsg > 5000)
//        {
//            lastMsg = millis();
//
//            DEBUG_PRINTLN("SAFE MODE ACTIVE");
//            
//        }
//
//        return;
//    }
//}  
// --------------------------------------------------






void inactivityVideoPowerOff()
{
  struct tm now = printLocalTime();
  String hourMin = String(now.tm_hour) + ":" + String(now.tm_min);
  myBlynk.TerminalPrint   (hourMin +":Turning Off Video for non activity for 1 Hour.."); 
  dvrOnOff (POWER_OFF);
  dvrSleep = true;  
  lastActivityTime    = millis();
}

void videoPowerOnAfterSleep()
{
  struct tm now = printLocalTime();
  String hourMin = String(now.tm_hour) + ":" + String(now.tm_min);
  myBlynk.TerminalPrint (hourMin +":Turning On Video for activity after sleep..");  
  dvrOnOff (POWER_ON);
  dvrSleep = false; 
  lastActivityTime    = millis();
}




//This checks once per second only.
void checkDvrInactivity()
{
  uint32_t PowerOffTimer  = (inactivityPowerOffTimer * 60UL * 1000UL) ; //inactivityPowerOffTimer in Minutes ;
  if (millis() - lastCheck >= 1000)
    {
        lastCheck = millis();
        
        if(!blynkActive &&  !zapOnOff && !zapScanOnly)  
        {
              if ((uint32_t)(millis() - lastActivityTime) >= PowerOffTimer /* &&  stateDVR == DVR_ON */&& !dvrSleep)//INACTIVITY_TIMEOUT_MS_POWER_OFF)
              {
                  inactivityVideoPowerOff();
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
