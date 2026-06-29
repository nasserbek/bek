
#ifndef AUX_H
#define AUX_H





bool  wifi_connect()
{
  _wifiIsConnected = false;

////#ifdef METEOR_ETH_PLS
//    wifiMulti.addAP(WIFI_SSID_SFR , WIFI_PASSWORD_SFR);
////#endif
//
////#ifdef METEOR_WIFI
//    wifiMulti.addAP(WIFI_SSID_METEOR_PLS, WIFI_PASSWORD_METEOR_PLS);
////#endif  
//  
//
////#ifdef NICE
//   wifiMulti.addAP(WIFI_SSID_SFR, WIFI_PASSWORD_SFR);
////#endif  

 
    wifiMulti.addAP(WIFI_SSID_BBOX, WIFI_PASSWORD_BBOX);
 

  
  Serial.println("Connecting Wifi...");
  //Connecting to the strongest WiFi connection
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected to: " + String (WiFi.SSID() )) ;
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  //print IP of the connected WiFi network
    _wifiIsConnected = true;
  }
  else  // if not WiFi not connected
  {
    _wifiIsConnected = false;
  }
  Serial.println("WIFI: "); Serial.println( _wifiIsConnected ? F("Connected") : F("Not Connected"));
  return _wifiIsConnected ;
}


bool checkInternet()
{
WiFiClient client;
IPAddress ip;
    if (WiFi.hostByName("raw.githubusercontent.com", ip))
    {
        Serial.print("GitHub IP: ");
        Serial.println(ip);
    }
    else
    {
        Serial.println("DNS FAILED");
    }
    
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
    blynkActive = false; blynkAtiveTimer     = millis();
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

void ledInit(void)
{
  RELAY_LED_V2.on();
}



#endif
