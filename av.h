#ifndef AV_H
#define AV_H


#include <Arduino.h>

class reciever {
  
  private:
    byte _scl_pin,_sda_pin, _analog_pin, _pll_Lsb,_pll_Msb;
    
  public:
 
    reciever(void) ;
    void init();
    byte pcf8574StatusRead(int _address);
    void sendCmd(int _address , byte _byte);
    void ConfigureInputs(int _address);
    int  Read_Analog_Av_Output(byte _analog_pin);
    bool Tuner_PLL(int _address, uint _pll);
    void bluLed(bool onOff);
    void rcPower(bool onOff);
};


#endif
