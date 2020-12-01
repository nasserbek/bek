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
      // Sim800 Set-up modem reset, enable, power pins
     pinMode(MODEM_PWKEY, OUTPUT);
     pinMode(MODEM_RST, OUTPUT);
     pinMode(MODEM_POWER_ON, OUTPUT);
     
     digitalWrite(MODEM_PWKEY, LOW);
     digitalWrite(MODEM_POWER_ON, HIGH);
     digitalWrite(MODEM_RST, LOW);
     delay(2000);
     digitalWrite(MODEM_RST, HIGH);
     delay(1000);
     
  
    DEBUG_PRINTLN("ESP32 with GSM SIM800L Initializing.......");
    long timeout = millis();
    long smsRebegin = millis();
    sim800lSerial->begin(4800, SERIAL_8N1, _rx_pin , _tx_pin);   // Make it slow so its easy to read!
    bool smsBeginStatus = sim800l.begin(*sim800lSerial);
    while (!smsBeginStatus)
      {
        if (millis() - smsRebegin > 5000 ) 
          {
            sim800lSerial->begin(4800, SERIAL_8N1, _rx_pin , _tx_pin);   // Make it slow so its easy to read!
            smsBeginStatus = sim800l.begin(*sim800lSerial);
            smsRebegin = millis();
            DEBUG_PRINTLN("Sms Re-Begin");
          }
        if (millis() - timeout > 20000){DEBUG_PRINTLN("Couldn't find GSM SIM800L"); return (simStatus = false); }
      }
 
  DEBUG_PRINTLN("Set up the FONA SIM800L to send a +CMTI notification when an SMS is received.");
  sim800lSerial->print("AT+CNMI=2,1\r\n");
  DEBUG_PRINTLN("GSM SIM800L is Ready.");
  delay(2000);
  return (simStatus = true);
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
  
        long timeout = millis();
        long smsResend = millis();
        DEBUG_PRINT("Sending: ");DEBUG_PRINTLN(String (smsmsg));
        bool smsSendStatus = sim800l.sendSMS(_callerIDbuffer, smsmsg);
        while (!smsSendStatus) 
        {
         if (millis() - smsResend > 10000 ) 
          {
            smsSendStatus = sim800l.sendSMS(_callerIDbuffer, smsmsg) ;
            smsResend = millis();
            DEBUG_PRINTLN("Re-Sending Sms...");
          }
         if (millis() - timeout > 30000){DEBUG_PRINTLN("Failed to send Sms"); return smsSendStatus; }
        }
       
        DEBUG_PRINTLN("SMS Sent!");
        return smsSendStatus;
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

void sim800L::sim800PowerOn(bool OnOff)
{
  pinMode(_power_on_pin, OUTPUT);
  if (OnOff) digitalWrite(_power_on_pin, HIGH);
  else digitalWrite(_power_on_pin, LOW);
}  

  
