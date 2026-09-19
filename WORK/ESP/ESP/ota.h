// ota.h

#ifndef OTA_H
#define OTA_H

extern void enableWDG(bool _enable);
extern void resetWdg(void);
extern tm printLocalTime();

/*************************************************OTA ZONE********************************************************************************************/
/*********************************************** web upodater *********************************************/

#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>

const char* host = "esp32";

WebServer server(80);

/*
 * Login page
*/

const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

/*
 * Server Index Page
 */

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";
/********************************************************************************************/

/************************ OTA GITHUB***************************/
void OtaGithub(void) 
{
  myBlynk.TerminalPrint("Starting Ota Web Update from Github");
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

      t_httpUpdate_return ret = ESPhttpUpdate.update(gitHubURL);

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                DEBUG_PRINTF("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                DEBUG_PRINTLN("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                DEBUG_PRINTLN("HTTP_UPDATE_OK");
                break;
                }
      }
}
/************************ END OTA GITHUB***************************/

/************************ ARDUINO IDE WIFI***************************/
void ArduinoIdeWifiSetup (void)
     {
        ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      DEBUG_PRINTLN("Start updating " + type);
      myBlynk.TerminalPrint("Start updating " + type);
    })
    .onEnd([]() {
      DEBUG_PRINTLN("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      DEBUG_PRINTF("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      DEBUG_PRINTF("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) DEBUG_PRINTLN("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) DEBUG_PRINTLN("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) DEBUG_PRINTLN("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) DEBUG_PRINTLN("Receive Failed");
      else if (error == OTA_END_ERROR) DEBUG_PRINTLN("End Failed");
    });
  ArduinoOTA.begin();

  DEBUG_PRINTLN("Ready");
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());
 }


void ArduinoIdeWifi(void)
{
   myBlynk.TerminalPrint("Starting Arduino IDE on Wifi");
    ArduinoIdeWifiSetup () ;   
       while (!wifiIde) 
       {
        enableWDG(false);
        if (  millis() - OtaTimeoutTimer > WIFI_IDE_TIMER )
        {
           wifiIde = true;
           resetWdg();
           OtaTimeoutTimer = millis();
           ESP.restart();
        }
        else ArduinoOTA.handle();
       }
}
  

/************************END ARDUINO IDE WIFI***************************/ 


/************************ LOCAL WIFI SERVER UPLOAD***************************/

 void localWebWifiOtaSetup(void)
{
  myBlynk.TerminalPrint("Local Web connected to http://esp32.local or " + WiFi.localIP().toString() +" with admin admin " );
  //use mdns for host name resolution
  if (!MDNS.begin(host)) { //http://esp32.local
     myBlynk.TerminalPrint("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  
   myBlynk.TerminalPrint("mDNS responder started");
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
      DEBUG_PRINTF("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // flashing firmware to ESP
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        DEBUG_PRINTF("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
 }
 
void localWebWifiOta (void)
 {
   struct tm now = printLocalTime();
   String hourMin = String(now.tm_hour) + ":" + String(now.tm_min);
   myBlynk.TerminalPrint(hourMin +":Starting Local Web Server");
   localWebWifiOtaSetup();
   while (!wifiWebUpdater) 
       {
      
        enableWDG(false);
        if (  millis() - OtaTimeoutTimer > WIFI_IDE_TIMER )
        {
           wifiWebUpdater = true;
           resetWdg();
           OtaTimeoutTimer = millis();
           ESP.restart();
        }
        server.handleClient();
        delay(1);
       }
 }
/************************ END LOCAL WIFI SERVER UPLOAD***************************/
/*************************************************END OF OTA ZONE********************************************************************************************/

#endif
