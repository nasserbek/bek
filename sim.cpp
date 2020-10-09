#include "sim.h"
#include "headers.h"


#include <Adafruit_FONA.h>
HardwareSerial *sim800lSerial = &Serial1;
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
char sim800lNotificationBuffer[64];          //for notifications from the FONA
int _slot = 0; // this will be the slot number of the SMS
char _smsBuffer[250];
Adafruit_FONA sim800l = Adafruit_FONA(MODEM_PWKEY);
//For fona debug comment #define ADAFRUIT_FONA_DEBUG under E:\Onedrive\Arduino\libraries\Adafruit_FONA-master\includes\FONAConfig.h


  
sim800L::sim800L(void)
{
  this->_rx_pin = MODEM_RX;
  this->_tx_pin = MODEM_TX;
  this->_power_on_pin =MODEM_POWER_ON;
  this->_pwkey = MODEM_PWKEY;
}


bool sim800L::init()
{
  long timeout = millis();
  sim800PowerOn(true)  ;
  DEBUG_PRINTLN("ESP32 with GSM SIM800L");
  DEBUG_PRINTLN("Initializing........");
  delay(1000);
  sim800lSerial->begin(4800, SERIAL_8N1, _rx_pin , _tx_pin);   // Make it slow so its easy to read!
  while (!sim800l.begin(*sim800lSerial)) {
    if (millis() - timeout > 10000L) {
      DEBUG_PRINTLN("Couldn't find GSM SIM800L");
       return (simStatus = false);
    }
  }
  
 /* if (!sim800l.begin(*sim800lSerial)) {
       DEBUG_PRINTLN("Couldn't find GSM SIM800L");
       return (simStatus = false);
    }
 */   
  DEBUG_PRINTLN("GSM SIM800L started.");

  // Set up the FONA to send a +CMTI notification  when an SMS is received
  sim800lSerial->print("AT+CNMI=2,1\r\n");
  DEBUG_PRINTLN("GSM SIM800L is Ready.");
  delay(1000);
  return (simStatus = true);
}  


void sim800L::sim800PowerOn(bool OnOff)
{
  pinMode(_power_on_pin, OUTPUT);
  if (OnOff) digitalWrite(_power_on_pin, HIGH);
  else digitalWrite(_power_on_pin, LOW);
}  

bool sim800L::smsRun() 
{
 if (!simStatus) return false;
  
 if (sim800l.available() )  
   {
     int charCount = 0;
     char* bufPtr = sim800lNotificationBuffer;    //handy buffer pointer

    // Read the notification into fona InBuffer
    do {
      *bufPtr = sim800l.read();
       delay(10);
      } while ((*bufPtr++ != '\n') && (sim800l.available()) && (++charCount < (sizeof(sim800lNotificationBuffer)-1)));
    //Add a terminal NULL to the notification string
    *bufPtr = 0;

    //Scan the notification string for an SMS received notification.
    //  If it's an SMS message, we'll get the slot number in 'slot'
   if (1 == sscanf(sim800lNotificationBuffer, "+CMTI: \"SM\",%d", &_slot)) 
   {
      if (!sim800l.getSMSSender(_slot, _callerIDbuffer, 31)) 
      {
        DEBUG_PRINTLN("Didn't find SMS message in slot!");
      }
      uint16_t smslen;
      if (sim800l.readSMS(_slot, _smsBuffer, 250, &smslen)) 
      {
        smsLen = smslen;smsString = String(_smsBuffer);
        DEBUG_PRINT(smsString);DEBUG_PRINT(" received from:  "); DEBUG_PRINTLN(_callerIDbuffer);
        deleteSMS();
        return true;
      }
      return false;
   }
   return false;
}
return false;  
}

bool sim800L::SendSMS(char *smsmsg)
{
  if (!simStatus) return false;
        DEBUG_PRINT("Sending: ");DEBUG_PRINTLN(String (smsmsg));
        if (!sim800l.sendSMS(_callerIDbuffer, smsmsg)) 
        {
          DEBUG_PRINTLN("Send SMS Failed");
          return false;
        } else 
        {
          DEBUG_PRINTLN("SMS Sent!");
          return true;
        }
}


bool sim800L::deleteSMS()
{
    if (!simStatus) return false;
        if (sim800l.deleteSMS(_slot)) {DEBUG_PRINTLN(F("Deleting SMS in slot OK!")); return true;}
        else 
        {
          DEBUG_PRINT(F("Couldn't delete SMS in slot ")); 
          DEBUG_PRINTLN(_slot);
          sim800l.print(F("AT+CMGD=?\r\n"));
          return false;
        }
}
  
