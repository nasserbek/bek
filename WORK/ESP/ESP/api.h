// api.h

#ifndef API_H
#define API_H



// Blynk cloud server
const char* blynkHost = "blynk.cloud";
unsigned int port = 80;

WiFiClient apiClient;

bool httpRequest(const String& method,
                 const String& url,
                 const String& request,
                 String&       response)
{
  DEBUG_PRINT(F("Connecting to "));
  DEBUG_PRINT(blynkHost);
  DEBUG_PRINT(":");
  DEBUG_PRINT(port);
  DEBUG_PRINT("... ");

      if (apiClient.connect(blynkHost, port)) {
        DEBUG_PRINTLN("OK");
      } else {
        DEBUG_PRINTLN("failed");
        return false;
      }

  DEBUG_PRINT(method); DEBUG_PRINT(" "); DEBUG_PRINTLN(url);

  apiClient.print(method); apiClient.print(" ");
  apiClient.print(url); apiClient.println(F(" HTTP/1.1"));
  apiClient.print(F("blynkHost: ")); apiClient.println(blynkHost);
  apiClient.println(F("Connection: close"));
  if (request.length()) {
    apiClient.println(F("Content-Type: application/json"));
    apiClient.print(F("Content-Length: ")); apiClient.println(request.length());
    apiClient.println();
    apiClient.print(request);
  } else {
    apiClient.println();
  }

  //DEBUG_PRINTLN("Waiting response");
  int timeout = millis() + 5000;
  while (apiClient.available() == 0) {
    if (timeout - millis() < 0) {
      DEBUG_PRINTLN(">>> apiClient Timeout !");
      apiClient.stop();
      return false;
    }
  }

  //DEBUG_PRINTLN("Reading response");
  int contentLength = -1;
  while (apiClient.available()) {
    String line = apiClient.readStringUntil('\n');
    line.trim();
    line.toLowerCase();
    if (line.startsWith("content-length:")) {
      contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
    } else if (line.length() == 0) {
      break;
    }
  }

  //DEBUG_PRINTLN("Reading response body");
  response = "";
  response.reserve(contentLength + 1);
  while (response.length() < contentLength) {
    if (apiClient.available()) {
      char c = apiClient.read();
      response += c;
    } else if (!apiClient.connected()) {
      break;
    }
  }
  apiClient.stop();
  return true;
}



void apiSend(char* RemoteBoard, String virtualPin, int value) {
  String response;

  // Send value to the cloud
  // similar to Blynk.virtualWrite()
  String request = "&pin="+ virtualPin +"&value=" ;
  DEBUG_PRINT("Sending value: " );
  DEBUG_PRINTLN(value);
  if (RemoteBoard == "ESP1" && BOARD != "ESP1")
  {
         if (httpRequest("GET", String("/external/api/update?token=") + BLYNK_AUTH_TOKEN_ESP1 + request + value, "", response)) {
          if (response.length() != 0) {
            DEBUG_PRINT("WARNING: ");
            DEBUG_PRINTLN(response);
          }
        }
  }

    if (RemoteBoard == "ESP2" && BOARD != "ESP2")
  {
         if (httpRequest("GET", String("/external/api/update?token=") + BLYNK_AUTH_TOKEN_ESP2 + request + value, "", response)) {
          if (response.length() != 0) {
            DEBUG_PRINT("WARNING: ");
            DEBUG_PRINTLN(response);
          }
        }
  }

   if (RemoteBoard == "ESP3" && BOARD != "ESP3")
  {
         if (httpRequest("GET", String("/external/api/update?token=") + BLYNK_AUTH_TOKEN_ESP3 + request + value, "", response)) {
          if (response.length() != 0) {
            DEBUG_PRINT("WARNING: ");
            DEBUG_PRINTLN(response);
          }
        }
  }
  
}


#endif
