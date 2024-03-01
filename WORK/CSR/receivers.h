// receivers.h

#ifndef RECEIVERS_H
#define RECEIVERS_H

/**************************************************VIDEO RC CONTROL ZONE***************************************************************/
void remoteControl(int cmd )
{
       StaticJsonDocument<54> doc4; //Json to send from
       
       if( (!RC_Remote_CSR1) && (!RC_Remote_CSR2)  && (!RC_Remote_CSR3))
        {
         mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);
        }
      else if( RC_Remote_CSR1)
       {
        doc4["RC"] = cmd;
        serializeJson(doc4, Json); // print to client
        client.publish(AWS_IOT_PUBLISH_TOPIC_RC, Json); 
      }

       else if( RC_Remote_CSR2)
      {
        doc4["RC"] = cmd;
        serializeJson(doc4, Json); // print to client
        client.publish(AWS_IOT_PUBLISH_TOPIC_RC_2, Json); 
      } 

       else if( RC_Remote_CSR3)
      {
        doc4["RC"] = cmd;
        serializeJson(doc4, Json); // print to client
        client.publish(AWS_IOT_PUBLISH_TOPIC_RC_3, Json); 
      } 
}

bool Tuner_PLL( int receiver, int _address, uint _pll)
{
  byte MSB = (_pll & 0xFF00) >> 8   ;   //mask LSB, shift 8 bits to the right
  byte LSB = _pll & 0x00FF     ;        //mask MSB, no need to shift

#ifdef CSR2   //4CH 4 Relays Active LOW and 2 I2C Controllers
  switch (receiver)
        {
          case 2:
          case 3:
                Wire1.beginTransmission(_address);
                Wire1.write(MSB );
                Wire1.write(LSB );
                Wire1.write(0xC2 );
                return (Wire1.endTransmission() );                       
          break;

          case 0:
          case 1:
                Wire.beginTransmission(_address);
                Wire.write(MSB );
                Wire.write(LSB );
                Wire.write(0xC2 );
                return (Wire.endTransmission() );                      
            break;                     
        }   
#endif

#ifdef CSR   //4CH 4 Relays Active LOW and 2 I2C Controllers
  switch (receiver)
        {
          case 2:
          case 3:
                Wire1.beginTransmission(_address);
                Wire1.write(MSB );
                Wire1.write(LSB );
                Wire1.write(0xC2 );
                return (Wire1.endTransmission() );                       
          break;
/*
          case 0:
          case 1:
                Wire.beginTransmission(_address);
                Wire.write(MSB );
                Wire.write(LSB );
                Wire.write(0xC2 );
                return (Wire.endTransmission() );                      
            break;   
*/                  
        }   
#endif

#ifdef CSR3   //2CH 2 Relays Active HIGH and MAIN I2C Controllers
          Wire.beginTransmission(_address);
          Wire.write(MSB );
          Wire.write(LSB );
          Wire.write(0xC2 );
          return (Wire.endTransmission() );  
#endif
}
        
bool receiverAvByCh (int Ch, int cmd)
{
  bool ack;
  int PLL_value;
  StaticJsonDocument<54> doc3; //Json to send from

       if (blynkConnected) myBlynk.blynkAckLed(ACK_BAD);
       ack = Tuner_PLL(selected_Rx, av_pll_addr, _pll[Ch]); 
       
       if (blynkConnected) {myBlynk.blynkAckLed( ack); myBlynk.sevenSegValue(Ch);}
       
       recevierFreq =videoCh[Ch].frequency;       
       if (blynkConnected) myBlynk.frequencyValue(recevierFreq );
       lastAck = ack; 
       
    if(V_Remote_CSR1 || V_Remote_CSR2 || V_Remote_CSR2)
        {
        doc3["CMD"] = cmd;
        serializeJson(doc3, Json); // print to client
        doc3["VIDEO"] = Ch;
        serializeJson(doc3, Json); // print to client
            
    if( V_Remote_CSR1)
       {
        client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_1, Json); 
      }

      if( V_Remote_CSR2)
      {
        client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_2, Json); 
      } 

       if( V_Remote_CSR3)
      {
        client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_3, Json); 
      } 
    }
   
   return(lastAck);
}


void receiverAvByFreq ( int Freq)
{
  bool ack=0;
       recevierFreq =Freq;
       if (blynkConnected) myBlynk.blynkAckLed( true); 
        videoCh[recevierCh].frequency = Freq;
       _pll[recevierCh] =( 512 * (Freq + 479.5) ) / 64 ;
       ack = Tuner_PLL(selected_Rx, av_pll_addr, _pll[recevierCh]);
       if (blynkConnected)  { myBlynk.blynkAckLed(ack);myBlynk.frequencyValue(Freq );}
}

void room ( int RC, int AV, int sel , int cmd)
{
     switch (sel)
          {
            case 1:
                receiverAvByCh ( AV , cmd);
            break;

            case 2:
                remoteControl(RC);
            break;

            
            case 3:
                receiverAvByCh ( AV, cmd);
                remoteControl(RC);
            break;
          }
}                            

void AvReceiverSel(int queuData)
 {            
 /*      #ifdef CSR  
                switch (queuData)
                    {
                   case 1:
                                 digitalWrite(I2C_1_2_RELAY, HIGH);  //  
                      break;
                      case 2:
                                 digitalWrite(I2C_1_2_RELAY, LOW);  //  
                      break;

                      case 3:
                                 digitalWrite(I2C_3_4_RELAY, HIGH);  //  
                      break;
                   case 4:
                                 digitalWrite(I2C_3_4_RELAY, LOW);  //  
                      break;                     
                    }  
     #endif
*/


      #ifdef CSR2  
                switch (queuData)
                    {
                      case 1:
                                 digitalWrite(I2C_1_2_RELAY, HIGH);  //  
                      break;
                      case 2:
                                 digitalWrite(I2C_1_2_RELAY, LOW);  //  
                      break;

                       case 3:
                                 digitalWrite(I2C_3_4_RELAY, HIGH);  //  
                      break;
                      case 4:
                                 digitalWrite(I2C_3_4_RELAY, LOW);  //  
                      break;                     
                    }  
     #endif
     
 /*    #ifdef CSR3 
                switch (queuData)
                    {
                   case 1:
                                 digitalWrite(I2C_1_2_RELAY, LOW);  //  
                      break;
                      case 2:
                                 digitalWrite(I2C_1_2_RELAY, HIGH);  //  
                      break;

                      case 3:
                                 digitalWrite(I2C_3_4_RELAY, LOW);  //  
                      break;
                   case 4:
                                 digitalWrite(I2C_3_4_RELAY, HIGH);  //  
                      break;                     
                    }   
     #endif */
}

bool  TCA9548A(uint8_t bus)
{

       #ifdef CSR2
                          Wire1.beginTransmission(0x70);  // TCA9548A address is 0x70
                          Wire1.write(1 << bus);          // send byte to select bus
                          return(Wire1.endTransmission());  
      #else
                          Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
                          Wire.write(1 << bus);          // send byte to select bus
                          return(Wire.endTransmission()); 
      #endif
}


/**************************************************END OF VIDEO RC CONTROL ZONE***************************************************************/


#endif
