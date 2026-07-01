#ifndef BLYNK_APP_H
#define BLYNK_APP_H

extern bool queuValidData;
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"

void relayCmd(int vPin, int cmd, const char* colorOn, const char* colorOff)
{
      digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);
      if(relayState) Blynk.setProperty(vPin, "color", colorOn);
      else  Blynk.setProperty(vPin, "color", colorOff);
      DEBUG_PRINTLN("Received " + String(vPin) + "Relay command " + String(cmd) ); 
}

void processBlynkQueu(void)
{
     switch (queuDataID)
          {
             case Q_EVENT_LOCALWEB_V6:
                    DEBUG_PRINTLN("Received V6 Local Web command "  ); 
                    localWebWifiOta();
            break;

            case Q_EVENT_GITHUB_V7:
                   otaWifiGithub= false;         
                   OtaTimeoutTimer = millis();
                   DEBUG_PRINTLN("Received V7 GIThUB OTA command "  ); 
                   OtaGithub();
            break;
 
            case Q_EVENT_REBOOT_V8:
                  rebootCmd=queuData;
                  DEBUG_PRINTLN("Received V8 REBOOT command " ); 
                  delay(1000);
                  rebootSw();
            break;   

            case Q_EVENT_RM_ID_1_V112 :
                 relayCmd(V112, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break; 
     

             case Q_EVENT_RM_ID_02_V122:
                  relayCmd(V122, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;
               
             case Q_EVENT_RM_ID_03_V123:
                  relayCmd(V123, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_04_V124:
                  relayCmd(V124, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_05_V125:
                  relayCmd(V125, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_06_V126:
                  relayCmd(V126, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_07_V127:
                  relayCmd(V127, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_08_V93:
                  relayCmd(V93, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_09_V80:
                  relayCmd(V80, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_10_V21:
             relayCmd(V21, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_11_V14:
                  relayCmd(V14, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_12_V15:
                  relayCmd(V15, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_13_V23:
                  relayCmd(V23, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_14_V103:
                  relayCmd(V112, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_15_V104:
                  relayCmd(V104, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_16_V105:
                  relayCmd(V105, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_17_V90:
                  relayCmd(V90, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_18_V91:
                  relayCmd(V91, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  
             
             case Q_EVENT_RM_ID_19_V92:
                  relayCmd(V92, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
              break;  

             case Q_EVENT_RM_ID_20_V100:
                  relayCmd(V100, int queuData, const char* BLYNK_DARK_BLUE, const char* BLYNK_RED)
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




BLYNK_WRITE(V6) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_LOCALWEB_V6;
  QueueSend(eventdata);
}

BLYNK_WRITE(V7) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_GITHUB_V7;
  QueueSend(eventdata);
}

BLYNK_WRITE(V8) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_REBOOT_V8;
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
    terminal.println("R" + String(relayNumber) + " ID" + String(chipID));
    terminal.println( WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) + "\n");
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

/*
Q_EVENT_RM_ID_01_V112,
Q_EVENT_RM_ID_02_V122,
Q_EVENT_RM_ID_03_V123, 
Q_EVENT_RM_ID_04_V124,
Q_EVENT_RM_ID_05_V125,
Q_EVENT_RM_ID_06_V126,
Q_EVENT_RM_ID_07_V127,
Q_EVENT_RM_ID_08_V93,
Q_EVENT_RM_ID_09_V80,
Q_EVENT_RM_ID_10_V21, 
Q_EVENT_RM_ID_11_V14, 
Q_EVENT_RM_ID_12_V15,  
Q_EVENT_RM_ID_13_V23,  
Q_EVENT_RM_ID_14_V103,  
Q_EVENT_RM_ID_15_V104,  
Q_EVENT_RM_ID_16_V105,  
Q_EVENT_RM_ID_17_V90,  
Q_EVENT_RM_ID_18_V91,  
Q_EVENT_RM_ID_19_V92, 
Q_EVENT_RM_ID_20_V100,
*/

BLYNK_WRITE(V112) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_01_V112;
  QueueSend(eventdata);
}

BLYNK_WRITE(V122) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_02_V122;
  QueueSend(eventdata);
}

BLYNK_WRITE(V123) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_03_V123;
  QueueSend(eventdata);
}

BLYNK_WRITE(V124) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_04_V124;
  QueueSend(eventdata);
}

BLYNK_WRITE(V125) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_05_V125;
  QueueSend(eventdata);
}

BLYNK_WRITE(V126) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_06_V126;
  QueueSend(eventdata);
}

BLYNK_WRITE(V127) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_07_V127;
  QueueSend(eventdata);
}

BLYNK_WRITE(V93) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_08_V93;
  QueueSend(eventdata);
}

BLYNK_WRITE(V80) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_09_V80;
  QueueSend(eventdata);
}

BLYNK_WRITE(V21) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_10_V21;
  QueueSend(eventdata);
}

BLYNK_WRITE(V14) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_11_V14;
  QueueSend(eventdata);
}

BLYNK_WRITE(V15) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_12_V15;
  QueueSend(eventdata);
}

BLYNK_WRITE(V23) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_13_V23;
  QueueSend(eventdata);
}

BLYNK_WRITE(V103) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_14_V103;
  QueueSend(eventdata);
}

BLYNK_WRITE(V104) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_15_V104;
  QueueSend(eventdata);
}

BLYNK_WRITE(V105) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_16_V105;
  QueueSend(eventdata);
}

BLYNK_WRITE(V90) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_17_V90;
  QueueSend(eventdata);
}

BLYNK_WRITE(V91) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_18_V91;
  QueueSend(eventdata);
}

BLYNK_WRITE(V92) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_19_V92;
  QueueSend(eventdata);
}

BLYNK_WRITE(V100) 
{
  _blynkEvent = true;
  _blynkData = param.asInt();
  eventdata = Q_EVENT_RM_ID_20_V100;
  QueueSend(eventdata);
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
