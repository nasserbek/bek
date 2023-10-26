/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-i2c-communication-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Wire.h>


#define I2C_SDA 33
#define I2C_SCL 32



TwoWire I2CBME = TwoWire(0);


unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));

  I2CBME.begin(I2C_SDA, I2C_SCL, 100000);
 
  I2CBME.beginTransmission(0x70);  // TCA9548A address is 0x70
  I2CBME.write(1 << 1);          // send byte to select bus
  
  
  bool ack = true;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  ack =  I2CBME.endTransmission();;  
  
  Serial.println(ack ? F("NotACK") : F("ACK"));

  delayTime = 1000;

  Serial.println();
}

void loop() { 
  
}

