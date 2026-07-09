
#ifndef AUX_H
#define AUX_H
extern void relayCmd(int vPin, bool cmd);
#include "routers.h"

tm printLocalTime()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo, 10000))   // wait up to 10 seconds
    {
        DEBUG_PRINTLN("Failed to obtain time");
        struct tm empty = {};
        return empty;
    }

    return timeinfo;
}

void relayOnOff (int relay, bool cmd)
{
          switch (relay)
            {
              case 1:
                    relayCmd(V112 ,cmd);
              break;
             case 2:
                  relayCmd(V122 ,cmd);
              break;
               
             case 3:
                  relayCmd(V123 ,cmd);
              break;  
             
             case 4:
                  relayCmd(V124 ,cmd);
              break;  
             
             case 5:
                  relayCmd(V125 ,cmd);
              break;  
             
             case 6:
                  relayCmd(V126 ,cmd);
              break;  
             
             case 7:
                  relayCmd(V127 ,cmd);
              break;  
             
             case 8:
                  relayCmd(V93 ,cmd);
              break;  
             
             case 9:
                  relayCmd(V80 ,cmd);
              break;  
             
             case 10:
             relayCmd(V21 ,cmd);
              break;  
             
             case 11:
                  relayCmd(V14 ,cmd);
              break;  
             
             case 12:
                  relayCmd(V15 ,cmd);
              break;  
             
             case 13:
                  relayCmd(V23 ,cmd);
              break;  
             
             case 14:
                  relayCmd(V103 ,cmd);
              break;  
             
             case 15:
                  relayCmd(V104 ,cmd);
              break;  
             
             case 16:
                  relayCmd(V105 ,cmd);
              break;  
             
             case 17:
                  relayCmd(V90 ,cmd);
              break;  
             
             case 18:
                  relayCmd(V91 ,cmd);
              break;  
             
             case 19:
                  relayCmd(V92 ,cmd);
              break;  

             case 20:
                  relayCmd(V100 ,cmd);
              break; 
          }
            
}

void inactivityRealyPowerOff(bool relaySt)
{
  struct tm now = printLocalTime();
  String hourMin = String(now.tm_hour) + ":" + String(now.tm_min);
  if(relaySt)terminal.println (hourMin +":Turning Off Relay for after 2 Hours of Realy ON and DVR Off.."); 
  if(!relaySt)terminal.println (hourMin +":Turning On Relay for after 2 Hours od Realy Off and DVR On.."); 
  relayOnOff (relayNumber, !relaySt);
  lastActivityTime    = millis();
}


//This checks once per second only.
void checkRelayInactivity()
{
  relayState = digitalRead(RELAY_PIN);
  uint32_t PowerOffTimer  = (inactivityPowerOffTimer * 60UL * 1000UL) ; //2 hours inactivityPowerOffTimer in Minutes ;
  if (millis() - lastCheck >= 1000)
    {
        lastCheck = millis();
        if(relayState == HIGH && (uint32_t)(millis() - lastActivityTime) >= PowerOffTimer) inactivityRealyPowerOff(relayState);
        else if (relayState == LOW && !blynkActive &&(uint32_t)(millis() - lastActivityTime) >= PowerOffTimer) inactivityRealyPowerOff(relayState);
    }
}

uint32_t identifyBoard()
{
    uint32_t myChip = ESP.getChipId();

    Serial.printf("Chip ID = %06X\n", myChip);

    for (int i = 0; i < sizeof(relayTable)/sizeof(relayTable[0]); i++)
    {
        if (relayTable[i].chipID == myChip)
        {
            blynkAuthToken = relayTable[i].token;
            relayNumber = relayTable[i].relayNumber;

            Serial.printf("Relay %d detected\n", relayNumber);
            return myChip;
        }
    }

    Serial.println("ERROR: Unknown ESP board!");
    return 0;
}


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


NetworkConfig* getCurrentNetwork()
{
    String currentSSID = WiFi.SSID();

    for (int i = 0; i < NUM_NETWORKS; i++)
    {
        if (currentSSID == nets[i].ssid  )
        {
            return &nets[i];
        }
    }
    return nullptr;
}


bool  wifi_connect()
{
  bool wifiConnection = false;
  for (int i = 0; i < NUM_NETWORKS; i++)
    {
      wifiMulti.addAP(nets[i].ssid,nets[i].wifiPw);
    } 

  DEBUG_PRINTLN("Connecting to Wifi...");
  //Connecting to the strongest WiFi connection
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
  {
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("WiFi is connected to: " + String (WiFi.SSID() )) ;
    DEBUG_PRINTLN("ESP Local IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());  //print IP of the connected WiFi network
    wifiConnection = true;
    
  }
  else  // if not WiFi not connected
  {
    DEBUG_PRINTLN("WIFI Connection Failed");  
    wifiConnection = false;
  }
  return wifiConnection ;
}


bool blynk_muliservers_connect()
{
    NetworkConfig* net = getCurrentNetwork();

    if (net == nullptr)
    {
        DEBUG_PRINT("Unknown SSID: ");
        DEBUG_PRINTLN(WiFi.SSID());
        return false;
    }

    IPAddress servers[] =
    {
        net->server1,
        net->server2,
        net->server3
    };

    DEBUG_PRINTLN("--------------------------------");
    DEBUG_PRINT("Connected SSID : ");
    DEBUG_PRINTLN(net->ssid);

    Blynk.disconnect();
    delay(100);

    for (uint8_t i = 0; i < 3; i++)
    {
        DEBUG_PRINT("Trying Blynk Server : ");
        DEBUG_PRINTLN(servers[i]);

        Blynk.config(blynkAuthToken, servers[i], 8080);

        if (Blynk.connect(BlynkServerTimeout))
        {
            DEBUG_PRINTLN("Blynk Connected");

            terminal.clear();
            terminal.println(
                String(net->ssid) +
                " IP:" + WiFi.localIP().toString() +
                " RSSI:" + String(WiFi.RSSI()) +
                " Server:" + servers[i].toString()
            );
            terminal.clear();
            terminal.println("R" + String(relayNumber) + " ID" + String(chipID) + " " + WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) + " Server IP: " + servers[i].toString() + "\n");
            terminal.flush();
            Blynk.virtualWrite(V24, VERSION_ID);
            blynkAtiveTimer = millis();
            blynkActive = true;        // I think this should be true
            return true;
        }

        DEBUG_PRINTLN("Connection Failed");
    }

    DEBUG_PRINTLN("No Blynk server available");
    return false;
}



bool checkInternet()
{
WiFiClient client;
  if (client.connect("1.1.1.1", 80))
  {
      Serial.println("Internet OK");
      client.stop();
      return true;
  }
  else
  {
      Serial.println("No Internet");
      return false;
  }  
}

void resetInternetLoss()
{
      InternetLoss = false;   
      netGeerReset = false; 
      routerResetStart  = false;
      resetNetgeerAfterInternetLossTimer = millis();
      restartAfterResetNG = millis();
      routerResetTimer        = millis();
}

void blynkRunTimer()
{
  timer.run();
  if ( (  (millis() - blynkAtiveTimer) >=  BLYNK_ACTIVE_TIMEOUT ) && blynkActive )
  {
    blynkActive = false; 
    blynkAtiveTimer     = millis();
  }
}

bool QueueSend(int value)
{
    uint8_t next = (g_queue_tail + 1) % EVENT_QUEUE_SIZE;

    if (next == g_queue_head)
        return false; // Queue full

    g_event_queue[g_queue_tail] = value;
    g_queue_tail = next;

    return true;
}

bool QueueReceive(int *value)
{
    if (g_queue_head == g_queue_tail)
        return false; // Queue empty

    *value = g_event_queue[g_queue_head];
    g_queue_head = (g_queue_head + 1) % EVENT_QUEUE_SIZE;

    return true;
}

bool blynk_getData()
{
  if (_blynkEvent)
  {
    blynkData = _blynkData;
    blynkEventID = _blynkEventID;
    _blynkEvent = false;
    blynkActive = true;
    blynkAtiveTimer     = millis();
    lastActivityTime    = millis();
    return true;
  }
  else return false;
}


void localWebWifiOtaSetup(void)
{
//  myBlynk.TerminalPrint("Local Web connected to http://esp8266.local or " + WiFi.localIP().toString() +" with admin admin " );
  //use mdns for host name resolution
  DEBUG_PRINTLN("Local Web connected to http://esp32.local or " + WiFi.localIP().toString() +" with admin admin " ); 

  if (!MDNS.begin(host)) { //http://esp32.local
  //   myBlynk.TerminalPrint("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
   DEBUG_PRINTLN("mDNS responder started"); 

 //  myBlynk.TerminalPrint("mDNS responder started");
  //return index page which is stored in serverIndex 
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });

    server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

 //handling uploading firmware file 
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // flashing firmware to ESP
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
  Serial.println("HTTP server started");
  server.on("/test", HTTP_GET, []() {
    Serial.println("TEST REQUEST RECEIVED");
    server.send(200, "text/plain", "OK");
});

 }

     
void timersMillis(void)
{
//    internetSurvilanceTimer = millis();
//    liveTimerOff            = millis();
//    OtaTimeoutTimer            = millis();
//    ackTimer                = millis();
//    restartAfterResetNG     = millis();
//    NetgeerResetGooglLostTimer = millis();
//    blynkNotActiveTimer     = millis();
//    routerResetTimer        = millis();
//    resetNetgeerAfterInternetLossTimer = millis();
//    Router_24_hoursTimer       = millis();
} 





#endif
