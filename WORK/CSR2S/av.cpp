#include "av.h"
#include "headers.h"

#include <Wire.h>

reciever::reciever(void) 
{
  this->_scl_pin = I2C_SCL;
  this->_sda_pin = I2C_SDA;
  this->_analog_pin =AV_OUTPUT_AN;

 }

void reciever::init_I2C() 
{
    Wire.begin(_sda_pin,_scl_pin); 
    
}


bool reciever::Tuner_PLL( int _address, uint _pll)
{
byte MSB = (_pll & 0xFF00) >> 8   ;   //mask LSB, shift 8 bits to the right
byte LSB = _pll & 0x00FF     ;        //mask MSB, no need to shift
bool ack= true;

        Wire.beginTransmission(_address);
        Wire.write(MSB );
        Wire.write(LSB );
        Wire.write(0xC2 );
        ack= Wire.endTransmission();
  

 return (ack);
}

void reciever::sendCmd(int _address , byte _byte) 
{
 
      Wire.beginTransmission(_address);
      Wire.write(_byte );
      Wire.endTransmission();
   
}
