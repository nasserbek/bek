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
//               //   Blynk.virtualWrite(V2, relayState);  
                  DEBUG_PRINTLN("Received V2 Relay command " + String(queuData) );          
            break;      
                  
            case Q_EVENT_V2:
             break;
             
             case Q_EVENT_V3:
             break;

            case Q_EVENT_V6:
                    DEBUG_PRINTLN("Received V6 Local Web command " + String(queuData) ); 
                    localWebWifiOta();
            break;

            case Q_EVENT_V7:
                   otaWifiGithub= false;         
                   OtaTimeoutTimer = millis();
                   OtaGithub();
            break;
 
            case Q_EVENT_V8:
                  rebootCmd=queuData;
                  delay(1000);
                  rebootSw();
            break;             
          }
}      


void blynkLoop(void)
{
       blynkConnected=_blynkIsConnected; 
       
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
            DEBUG_PRINTLN(blynkConnected ? F("Blynk is Connected") : F("Blynk is Disconnected") );
           DEBUG_PRINTLN(internetConnected? F("Internet is Connected") : F("Internet is Disconnected") );

            InternetLoss = true; 
            netGeerReset = false; 
            routerResetStart  = false;
            resetNetgeerAfterInternetLossTimer = millis();
          
            blynkEvent=false; 
          }
       blynkRunTimer();
}



void  blynkTimers(void)
{
  Blynk.virtualWrite(V30, inactivityPowerOffTimer);
  Blynk.virtualWrite(V31, inactivityRestartTimer);
  Blynk.virtualWrite(V72, zapTimerSec);
}




bool blynkconnect()
{

    if(String (WiFi.SSID() ) == WIFI_SSID_METEOR_PLS)
    blynkLocalServer = BLYNK_SERVER_METEOR_ETH_PLS ;

    if(String (WiFi.SSID() ) == WIFI_SSID_BBOX )
    blynkLocalServer = BLYNK_SERVER_BBOX;

    if(String (WiFi.SSID() ) == WIFI_SSID_SFR )
    blynkLocalServer = BLYNK_SERVER_METEOR_SFR;
    
    Blynk.config(BLYNK_AUTH_TOKEN, blynkLocalServer, 8080);
    Blynk.connect(BlynkServerTimeout);
    delay(1000);
    _blynkIsConnected = Blynk.connected();
    if (_blynkIsConnected)
    {
      Serial.println("BLYNK: ");

      if (_blynkIsConnected)
      {
        Serial.print("Connected to ");
        Serial.println(blynkLocalServer.toString());
      }
      else
      {
        Serial.println("Not Connected");
      }

      blynkAtiveTimer     = millis();
      blynkActive = false;
  //    ledInit();
      terminal.clear();
      terminal.println(WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString() + " WiFi RSSI: " + String (WiFi.RSSI()) + " Server IP: " + blynkLocalServer.toString() + "\n");
      terminal.flush();
    }
  return _blynkIsConnected;
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
BLYNK_CONNECTED()
{
    Blynk.syncVirtual(V1);
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

void checkBlynk() {
  if (wifiMulti.run(WiFi_TIMEOUT) == WL_CONNECTED)
  {
    unsigned long startConnecting = millis();
    _blynkIsConnected = true;
    _wifiIsConnected = true;

    while (!Blynk.connected()) {
      
      if (millis() > startConnecting + BlynkServerTimeout) {
        Serial.println("Wifi connected but Blynk is Disconnected, connectig agin to Blynk....");
        _blynkIsConnected = blynkConnected = blynkconnect();
        if (blynkConnected) 
              {
//                 AvRxIndex(Av_Rx);
//                 chSelect("ch01");
//                 versionBlynk(VERSION_ID);
//                 loadCrashCount();
//                 terminalSend (VERSION_ID + " " + String(crashCount) + " Craches" );
             }
        else Serial.println("Unable to connect to Blynk server. ");
        break;
      }
    }
  }
  else
  {
     Serial.println("WIFI Diconnected!! Trying to reconnect.");//); DEBUG_PRINT( _wifiIsConnected ? F("Connected") : F("Not Connected"));
     _wifiIsConnected  = wifi_connect();
    _wifiIsConnected = false;
    _blynkIsConnected = false;
   }

  Serial.printf("Checking again Blynk connected in %is.\n", blynkIntervalInterval / 1000);
  Serial.println(".");
}

void blinkLedWidget()
{
//  if (!blynkActive && !zapOnOff  && !zapScanOnly && dvrSleep && stateDVR == DVR_OFF)
//  {
//    if (ledStatus) {
//      LIVE_LED_V121.setColor(BLYNK_RED);
//      Serial.println("LED on V121: red");
//      ledStatus = false;
//    } else {
//      LIVE_LED_V121.setColor(BLYNK_GREEN);
//      Serial.println("LED on V121: green");
//      ledStatus = true;
//    }
//   ledYellow = false;
//  }
//  else if(!ledYellow)
//  {
//    LIVE_LED_V121.setColor(BLYNK_YELLOW);
//    Serial.println("LED on V121: yellow");
//    ledStatus = false;
//    ledYellow =true;
//  }
}







#endif
