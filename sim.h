#ifndef SIM_H
#define SIM_H

#include <Arduino.h>


class SIM800L {
  
  private:
    byte _rx_pin,_tx_pin, _power_on_pin, _pwkey,_command;
    String _SmsReceived;
    int _smsLen=0;
    char _callerIDbuffer[32] = "+33699263544";  //we'll store the SMS sender number in here
    String _smsString="";

  public:
    String smsString="";
    int     smsLen=0;
    
    bool simStatus=true;
    SIM800L(void) ;
    bool init();
    bool smsRun(void);
    bool SendSMS(char *smsmsg);
    bool deleteSMS();
    void sim800PowerOn(bool OnOff);
};

#endif
