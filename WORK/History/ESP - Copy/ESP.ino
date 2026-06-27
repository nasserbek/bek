#include "MainCalls.h"

blynk myBlynk;

void setup() 
{
     Serial.begin(115200);
     
     initWDG(MIN_5,EN);
     
     createHandleGroup();

     looadRoomData();     

     safeModeCheck();

     i2cSetup();
     mySwitch.enableTransmit(RC_TX_PIN);  
     internetConnected = checkInternet();
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

void loop(void) 
{
       stateDVR = digitalRead(AV_RX_DVR_PIN);
       
       resetWdg();
       internetCheck();
       resetBoardID();
    //   awsLoop();
       blynkLoop();
       safeModeLoop();
       if(inactivityCtrl == MILLS)restartForInactivity(); //This checks inactivity once per second only controlled by mills().
       
       bool zapScan = zapOnOff || zapScanOnly;
       
      if (zapScan) zappingAvCh ( zapScan, zapTimer); 
}
