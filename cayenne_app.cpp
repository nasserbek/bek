#include "CAYENNE_app.h"
#include "headers.h"


//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial

#include <CayenneMQTTESP32.h>


#ifdef BEK
    char username[] = "4596b3c0-a752-11e6-839f-8bfd46afe676";
    char password[] = "cccf2869ad1fb73dd04185b89f87d05947dd68de";
    char clientID[] = "31158910-3684-11eb-b767-3f1a8f1211ba";
#else
    char username[] = "4596b3c0-a752-11e6-839f-8bfd46afe676";
    char password[] = "cccf2869ad1fb73dd04185b89f87d05947dd68de";
    char clientID[] = "31158910-3684-11eb-b767-3f1a8f1211ba";
#endif

                                      
//char ssid[] = WIFI_SSID;
//char wifiPassword[] = WIFI_PASSWORD;

// Default function for sending sensor data at intervals to Cayenne.
// You can also use functions for specific channels, e.g CAYENNE_OUT(1) for sending channel 1 data.
CAYENNE_OUT_DEFAULT()
{
  // Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
  Cayenne.virtualWrite(0, millis());
  // Some examples of other functions you can use to send data.
  //Cayenne.celsiusWrite(1, 22.0);
  //Cayenne.luxWrite(2, 700);
  //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}

// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}




cayenne::cayenne(void) 
{

}



void cayenne::init() 
{
  Cayenne.begin(username, password, clientID, WIFI_SSID, WIFI_PASSWORD);
}

void cayenne::cayenneRun()
{
    Cayenne.loop();
}
