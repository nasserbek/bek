#ifndef AV_H
#define AV_H


#include <Arduino.h>

class reciever {
  
  private:
    byte _scl_pin,_sda_pin, _analog_pin, _scl_pin_1,_sda_pin_1, _analog_pin_1, _pll_Lsb,_pll_Msb;
    
  public:
 
    reciever(void) ;
    void init_I2C();
    void sendCmd(int Rx, int _address , byte _byte);
    bool Tuner_PLL(int x, int _address, uint _pll);
};


#endif
