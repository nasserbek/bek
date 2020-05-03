#ifndef BLYNK_APP_H
#define BLYNK_APP_H

#include <Arduino.h>


class BLYNK {
  
  private:
          int old_t433ChNumber=0;
          int old_blynkfreqValue=0;
          int old_sevenSeg;
          int old_otaBlynk;
          int old_boot;
          int old_fbonBlynk;
          
  public:
    bool blynkEvent = false;
    int  blynkData=0;
    int  blynkEventID =0;
    int  t433ChNumber, blynkfreqValue,sevenSeg,otaBlynk,bootBlynk,fbonBlynk,smsBlynk;

    BLYNK(void) ;
    void init();
    void blynkRun();
    
    void sendAlive( int _data);
    void blynkAckLed(bool _data);
    void blynkRCLed(bool _data);
    void resetT433Cmd(int cmd );
    void blynkFirebaseLed(bool _data);
    void blynkSmsLed(bool _data);
    void sevenSegValue(int freq );
    bool getData();
    void sendRsss(int _rsss);
    void notifierDebug(String subject, String body);

};
#endif
