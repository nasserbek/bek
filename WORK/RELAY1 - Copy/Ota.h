#ifndef OTA_H
#define OTA_H

/************************ OTA GITHUB***************************/
void OtaGithub(void) 
{
//  myBlynk.TerminalPrint("Starting Ota Web Update from Github");
while (!otaWifiGithub) 
       {
        enableWDG(false);
        if (  millis() - OtaTimeoutTimer > WIFI_IDE_TIMER )
        {
           otaWifiGithub = true;
           resetWdg();
           OtaTimeoutTimer = millis();
           ESP.restart();
        }
      WiFiClientSecure client;
      client.setInsecure();
    //  t_httpUpdate_return ret  = ESPhttpUpdate.update(gitHubURL);
       t_httpUpdate_return ret = ESPhttpUpdate.update(client, gitHubURL);
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.printf("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                Serial.printf("HTTP_UPDATE_OK");
                break;
                }
      }
}
/************************ END OTA GITHUB***************************/


void localWebWifiOta (void)
 {
//   struct tm now = printLocalTime();
//   String hourMin = String(now.tm_hour) + ":" + String(now.tm_min);
//    myBlynk.TerminalPrint(hourMin +":Starting Local Web Server");
   wifiWebUpdater = false;
   OtaTimeoutTimer = millis();
   localWebWifiOtaSetup();
   while (!wifiWebUpdater) 
       {
//         blueLedFlash(5000) ;        
        enableWDG(false);
        if (  millis() - OtaTimeoutTimer > WIFI_IDE_TIMER )
        {
           wifiWebUpdater = true;
           resetWdg();
           OtaTimeoutTimer = millis();
           ESP.restart();
        }
        server.handleClient();
        yield();


        delay(1);
       }
 }







#endif
