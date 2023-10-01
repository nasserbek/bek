#include "av.h"
#include "headers.h"

#include <Wire.h>

reciever::reciever(void) 
{
  this->_scl_pin = I2C_SCL;
  this->_sda_pin = I2C_SDA;
  this->_analog_pin =AV_OUTPUT_AN;

  this->_scl_pin_1 = I2C_SCL_1;
  this->_sda_pin_1 = I2C_SDA_1;
  this->_analog_pin_1 =AV_OUTPUT_AN_1;
}

void reciever::init_I2C() 
{
    Wire.begin(_sda_pin,_scl_pin); 
    Wire1.begin(_sda_pin_1,_scl_pin_1); 
}


bool reciever::Tuner_PLL(int Rx, int _address, uint _pll)
{
byte MSB = (_pll & 0xFF00) >> 8   ;   //mask LSB, shift 8 bits to the right
byte LSB = _pll & 0x00FF     ;        //mask MSB, no need to shift
bool ack= true;
    if (Rx == 1)
    {
        Wire.beginTransmission(_address);
        Wire.write(MSB );
        Wire.write(LSB );
        Wire.write(0xC2 );
        ack= Wire.endTransmission();
    }

    if (Rx == 2)
    {
        Wire1.beginTransmission(_address);
        Wire1.write(MSB );
        Wire1.write(LSB );
        Wire1.write(0xC2 );
        ack= Wire1.endTransmission();
    }

 return (ack);
}

void reciever::sendCmd(int Rx ,int _address , byte _byte) 
{
  if (Rx == 1)
    {
      Wire.beginTransmission(_address);
      Wire.write(_byte );
      Wire.endTransmission();
    }

   if (Rx == 2)
    {
      Wire1.beginTransmission(_address);
      Wire1.write(_byte );
      Wire1.endTransmission();
    }   
}
