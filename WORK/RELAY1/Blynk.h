#ifndef BLYNK_APP_H
#define BLYNK_APP_H

extern bool queuValidData;
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"


void processBlynkQueu(void)
{
     switch (queuDataID)
          {
            case Q_EVENT_V0:
            break;
            
            case Q_EVENT_V1:
                  relayState = queuData;
                  digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);
                  if(relayState) 
                    {
                      RELAY_LED_V2.on();
                      Blynk.setProperty(V1, "color", BLYNK_YELLOW);
                    }
                  else 
                  {
                    RELAY_LED_V2.off();
                    Blynk.setProperty(V1, "color", BLYNK_RED);
                  }
                  DEBUG_PRINTLN("Received V2 Relay command " + String(queuData) );          
            break;      
                  
            case Q_EVENT_V2:
             break;
             
             case Q_EVENT_V3:
             break;

            case Q_EVENT_V6:
                    DEBUG_PRINTLN("Received V6 Local Web command "  ); 
                    localWebWifiOta();
            break;

            case Q_EVENT_V7:
                   otaWifiGithub= false;         
                   OtaTimeoutTimer = millis();
                   DEBUG_PRINTLN("Received V7 GIThUB OTA command "  ); 
                   OtaGithub();
            break;
 
            case Q_EVENT_V8:
                  rebootCmd=queuData;
                  DEBUG_PRINTLN("Received V8 REBOOT command " ); 
                  delay(1000);
                  rebootSw();
            break;             
          }
}      


void blynkLoop(void)
{
       if ( blynkConnected )
          {
           if(!liveLedUpdate) liveLedUpdate = true; 
      
            Blynk.run();
            int event;
            queuValidData = QueueReceive(&queuDataID);
 
            if(queuValidData) 
                  {
                    blynk_getData();
                    queuData = blynkData; 
                    DEBUG_PRINTLN("queuDataID   " + String(queuDataID) );          
                    processBlynkQueu(); 
                  }
            resetInternetLoss();
          }

       else if( !InternetLoss && !blynkConnected)  
          {
            DEBUG_PRINTLN("Blynk Disconnected , Internet Loss!!!");
            InternetLoss = true; 
            netGeerReset = false; 
            routerResetStart  = false;
            resetNetgeerAfterInternetLossTimer = millis();
            blynkEvent=false; 
          }
       blynkRunTimer();
}








void checkBlynk() {
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
  {
    unsigned long startConnecting = millis();
    wifiAvailable = true;

    while (!Blynk.connected()) {
      
      if (millis() > startConnecting + BlynkServerTimeout) {
        DEBUG_PRINTLN("Wifi connected but Blynk is Disconnected, connectig agin to Blynk....");
        DEBUG_PRINTLN("");
        DEBUG_PRINTLN("WiFi is connected to: " + String (WiFi.SSID() )) ;
        DEBUG_PRINTLN("ESP Local IP address: ");
        DEBUG_PRINTLN(WiFi.localIP());  //print IP of the connected WiFi network
    
        blynkConnected = blynk_muliservers_connect;
        if(!blynkConnected) DEBUG_PRINTLN("Unable to connect to Blynk server. ");
        break;
      }
    }
  }
  else
  {
     DEBUG_PRINTLN("WIFI Diconnected!! Trying to reconnect.");//); 
     wifiAvailable  = wifi_connect();
     blynkConnected = false;
  }
    
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("--------------------------------");
    internetConnected = checkInternet();
    DEBUG_PRINT("WIFI is ");
    DEBUG_PRINTLN(wifiAvailable ? F("Connected") : F("Not Connected"));
    DEBUG_PRINT("BLYNK is ");
    DEBUG_PRINTLN(blynkConnected ? F("Connected") : F("Not Connected"));
    DEBUG_PRINTF("Checking the connection again in %is.\n", blynkIntervalInterval / 1000);

    if(InternetLoss)
    {
      unsigned long remaining = RESTART_AFTER_NG_RESET_TIMER - (millis() - restartAfterResetNG);
      
      unsigned long minutes = remaining / 60000;
      unsigned long seconds = (remaining % 60000) / 1000;
      
      DEBUG_PRINT("Restart in ");
      DEBUG_PRINT(minutes);
      DEBUG_PRINT(":");
      if (seconds < 10) DEBUG_PRINT('0');
      DEBUG_PRINTLN(seconds);      
    
    }
}



BLYNK_WRITE(V1)
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_V1;
  QueueSend(eventdata);
}


BLYNK_WRITE(V3) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_V3;
  QueueSend(eventdata);
}

BLYNK_WRITE(V6) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_V6;
  QueueSend(eventdata);
}

BLYNK_WRITE(V7) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_V7;
  QueueSend(eventdata);
}

BLYNK_WRITE(V8) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_V8;
  QueueSend(eventdata);
}
BLYNK_WRITE(V102)  //TERMINAL
{
  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
    terminal.println("You said: 'Marco'") ;
    terminal.println("I said: 'Polo'") ;
    terminal.flush();
  }

  else if (String("id") == param.asStr())
  {
    terminal.println( WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) + "\n");
    terminal.println( String( ESP.getChipId() ) );
    terminal.flush();
  }

  else if (String("c") == param.asStr())
  {
    terminal.clear();
  }

  else {
    // Send it back
    terminal.print("You said:");
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();

  }

  // Ensure everything is sent
  terminal.flush();

}

BLYNK_CONNECTED()
{
//    Blynk.syncVirtual(V1);
}



// --------------------------------------------------
// Called whenever activity is detected
// --------------------------------------------------
void resetInactivityTimer()
{
    lastActivityTime = millis();
}



void SendLiveLed()
  {
    if (liveLed)  liveLed = false; 
    else liveLed = true;
    liveLedUpdate =false;

 }



#endif
