#ifndef BLYNK_APP_H
#define BLYNK_APP_H

#include <Arduino.h>


class blynk {
  
  private:
          int old_t433ChNumber=0;
          int old_t315ChNumber=0;
          int old_blynkfreqValue=0;
          int old_sevenSeg;
          int old_otaBlynk;
          int old_boot;
          int old_fbonBlynk;
          
  public:
    int connection_loss_count = 0;
    bool blynkEvent = false;
    bool sendToBlynk = false;
    bool sendToBlynkLeds = false;
    int  blynkData=0;
    int  blynkEventID =0;
    int  t433ChNumber,t315ChNumber, blynkfreqValue,sevenSeg,frequency,otaBlynk,bootBlynk,fbonBlynk,smsBlynk;
    bool wifiIsConnected = false;
    bool blynkIsConnected = false;



    blynk(void) ;
    void blynk1(void);
    void SyncAll(void);
    void init();
    void blynkRun();
    void blynkRunTimer();
    bool blynkConnected();
    void blynkConnect();
    bool blynkStatus();
    bool wifiStatus();
    void resetRemoteRC( int _data);
    void resetRemoteVideo( int _data);
    bool Tuner_PLL(int x, int _address, uint _pll);
    void blynkAckLed(bool _data);
    void blynkRCLed(bool _data, int cmd);
    void resetSetupAndScan(bool _data);
    void resetT433Cmd(int cmd );
    void resetT315Cmd(int cmd );
    void wifiRSSI(int _data);
    void zapAutoLocalRC(bool _data);
    void sevenSegValue(int vch);
    void frequencyValue(int freq );
    bool getData();
    void dvrSwitch(bool cmd);
    void notifierDebug(String subject, String body);
    void zapStatus(bool _data);
    void sendToBlynkLed(bool _data);
    bool wifiConnect(void);
    void sendAvRxIndex(int _index);
    void RelaySelect(int rele);
    void sendVersion(String ver); 
    void TerminalPrint (String str);
    void sendPulseRepetetion(int pulse, int repetetion);
    void visualActiveRoom(int id, bool zap);
    void repeatSync(bool repeat);
    void TurnOffLastCh(bool lastAck,int lastSelectedCh, int zapOnOff);
    void sendBoardIndex(int _index);
    void releActiveCh(int rele, int ch);

};
#endif
