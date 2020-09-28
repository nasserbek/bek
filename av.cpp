#include "av.h"
#include "headers.h"

#include <Wire.h>

reciever::reciever(void) 
{
  this->_scl_pin = I2C_SCL;
  this->_sda_pin = I2C_SDA;
  this->_analog_pin =AV_OUTPUT_AN;

  pinMode(BLU_LED, OUTPUT);
  pinMode(RC_PWR_PIN, OUTPUT);
  digitalWrite(RC_PWR_PIN, LOW);
}

void reciever::init() 
{
    Wire.begin(_sda_pin,_scl_pin); 
    ConfigureInputs (PCF8574_STATUS_ADDRESS);
    Tuner_PLL(PLL_ADDRESS, ( 512 * ( 1000000 * (1080 + 479.5) ) ));
}


bool reciever::Tuner_PLL(int _address, uint _pll)
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

void reciever::ConfigureInputs (int _address) 
{
   Wire.beginTransmission(_address);
   sendCmd(_address , 0xFF);
   Wire.endTransmission();
}

byte reciever::pcf8574StatusRead(int _address) 
{
  Wire.beginTransmission(_address);
  Wire.requestFrom(_address, 1);
  int Data_In = Wire.read();
  Wire.endTransmission();
  return Data_In;

}

void reciever::sendCmd(int _address , byte _byte) 
{
  Wire.beginTransmission(_address);
  Wire.write(_byte );
  Wire.endTransmission();
}

int  reciever::Read_Analog_Av_Output(byte _analog_pin)
{
  return (analogRead(_analog_pin));
}

void  reciever::bluLed(bool onOff)
{
   if (onOff) digitalWrite(BLU_LED, HIGH); 
   else digitalWrite(BLU_LED, LOW); 
}

void  reciever::rcPower(bool onOff)
{
   if (onOff) digitalWrite(RC_PWR_PIN, HIGH); 
   else digitalWrite(RC_PWR_PIN, LOW); 
}
