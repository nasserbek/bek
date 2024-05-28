// receivers.h

#ifndef RECEIVERS_H
#define RECEIVERS_H



extern int  Av_Rx;
extern void AvReceiverSel(int queuData);
extern void videoChanel(int ch, bool cmd);


bool  TCA9548A(uint8_t bus)
{

    Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
    Wire.write(1 << bus);          // send byte to select bus
    return(Wire.endTransmission()); 
}

/**************************************************VIDEO RC CONTROL ZONE***************************************************************/
void remoteControl(int cmd )
{
      if( (!RC_Remote_CSR1) && (!RC_Remote_CSR2)  && (!RC_Remote_CSR3)) mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);  
        
      else if( RC_Remote_CSR1)
       {

        if (blynkConnected) apiSend(ESP1, "V1", cmd); 
        else 
          {
            StaticJsonDocument<54> doc4; //Json to send from
            doc4["RC"] = cmd;
            serializeJson(doc4, Json); // print to client
            client.publish(AWS_IOT_PUBLISH_TOPIC_RC, Json); 
          }
        }

       else if( RC_Remote_CSR2)
       {
        if (blynkConnected) apiSend(ESP2, "V1", cmd);
        else
            {
              StaticJsonDocument<54> doc4; //Json to send from
              doc4["RC"] = cmd;
              serializeJson(doc4, Json); // print to client
              client.publish(AWS_IOT_PUBLISH_TOPIC_RC_2, Json); 
            } 
       }
       
      else if( RC_Remote_CSR3)
      {
        if (blynkConnected) apiSend(ESP3, "V1", cmd);
          else 
          {
            StaticJsonDocument<54> doc4; //Json to send from
            doc4["RC"] = cmd;
            serializeJson(doc4, Json); // print to client
            client.publish(AWS_IOT_PUBLISH_TOPIC_RC_3, Json); 
          }
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

#ifdef CSR   
   Wire.beginTransmission(_address);
   Wire.write(MSB );
   Wire.write(LSB );
   Wire.write(0xC2 );
   return (Wire.endTransmission() ); 
#endif

#ifdef CSR3    
          Wire.beginTransmission(_address);
          Wire.write(MSB );
          Wire.write(LSB );
          Wire.write(0xC2 );
          return (Wire.endTransmission() );  
#endif

#ifdef TEST   
          return false;  
#endif
}
        
bool receiverAvByCh (int Ch, int cmd)
{
  bool ack;
  int PLL_value;
       if (blynkConnected) myBlynk.blynkAckLed(ACK_BAD);
       ack = Tuner_PLL(selected_Rx, av_pll_addr, _pll[Ch]); 
      
       if (blynkConnected) {myBlynk.blynkAckLed( ack); myBlynk.VideoActiveCh(Ch);}
       
       recevierFreq =videoCh[Ch].frequency;       
       if (blynkConnected) myBlynk.frequencyValue(recevierFreq );
       lastAck = ack; 
       
    if(V_Remote_CSR1 || V_Remote_CSR2 || V_Remote_CSR2)
      {
        StaticJsonDocument<54> doc3; //Json to send from
        doc3["CMD"] = cmd;
        serializeJson(doc3, Json); // print to client
        doc3["VIDEO"] = Ch;
        serializeJson(doc3, Json); // print to client
             
          if( V_Remote_CSR1)
           {
             if (blynkConnected) apiSend(ESP1, "V2", Ch);
              else client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_1, Json); 
           }
    
          if( V_Remote_CSR2)
          {
            if (blynkConnected) apiSend(ESP2, "V2", Ch);
            else client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_2, Json); 
            
          } 
    
           if( V_Remote_CSR3)
          {
            if (blynkConnected) apiSend(ESP3, "V2", Ch);
            else client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_3, Json); 
            
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
     #ifdef CSR  
          TCA9548A(queuData-1);
     #endif

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
     
     #ifdef CSR3 
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
     #endif 
}



void  dvrOnOff (bool onOff)
{
   myBlynk.dvrSwitch(onOff);
   
   if (onOff) 
   {
    digitalWrite(AV_RX_DVR_PIN_2, LOW); 
   
    #ifdef CSR      
           
           selected_Rx = 3;  //H3
           delay (1000);
           AvReceiverSel(4);  
           delay (1000);
           if(DvrChOn) {Av_Rx = BOTH; videoChanel(R_29, ON);}
           else {Av_Rx = SOLO_VIDEO; videoChanel(R_29, ON);}
           myBlynk.RelaySelect(2);  //RX4 SHOWS RX2 N HMI
           delay (1000);   
            
           selected_Rx = 2;  //CH2
           AvReceiverSel(3);  
           delay (1000);
           if(DvrChOn) {Av_Rx = BOTH; videoChanel(R_49, ON);}
           else {Av_Rx = SOLO_VIDEO; videoChanel(R_49, ON);}
           myBlynk.RelaySelect(3);   //RX3 SHOWS RX3 IN HMI

    #endif
  
    #ifdef CSR2      
           selected_Rx = 0;
           delay (1000);
           AvReceiverSel(1);  
           delay (1000);
           if(DvrChOn) {Av_Rx = BOTH; videoChanel(R_25, ON);}
           else {Av_Rx = SOLO_VIDEO; videoChanel(R_25, ON);}
           myBlynk.RelaySelect(1);
           delay (1000); 
           
           selected_Rx = 1;
           AvReceiverSel(2);  
           delay (1000);
           if(DvrChOn) {Av_Rx = BOTH; videoChanel(R_26, ON);}
           else {Av_Rx = SOLO_VIDEO; videoChanel(R_26, ON);}
           myBlynk.RelaySelect(2);
           delay (1000); 

           selected_Rx = 2;
           AvReceiverSel(3);  
           delay (1000);
           if(DvrChOn) {Av_Rx = BOTH; videoChanel(R_27, ON);}
           else {Av_Rx = SOLO_VIDEO; videoChanel(R_27, ON);}
           myBlynk.RelaySelect(3);
           delay (1000); 
           
           selected_Rx = 3;
           AvReceiverSel(4);  
           delay (1000);
           if(!DvrChOn) {Av_Rx = BOTH; videoChanel(R_28, ON);}
           else {Av_Rx = SOLO_VIDEO; videoChanel(R_28, ON);}
           myBlynk.RelaySelect(4);
    #endif
    
    #ifdef CSR3      
           selected_Rx = 0; //CH4
           delay (1000);
           AvReceiverSel(1);  
           delay (1000);
//           if(DvrChOn) {Av_Rx = BOTH; videoChanel(R_25, ON);}  //CH4 ZAP
//           else {Av_Rx = SOLO_VIDEO; videoChanel(R_25, ON);}
           Av_Rx = SOLO_VIDEO; videoChanel(R_25, ON);
           myBlynk.RelaySelect(4);  //RX1 SHOWS RX2 IN HMI
           delay (1000); 
           
           selected_Rx = 1;   //CH3
           AvReceiverSel(2);  
           delay (1000);
           if(DvrChOn) {Av_Rx = BOTH; videoChanel(R_51, ON);}
           else {Av_Rx = SOLO_VIDEO; videoChanel(R_51, ON);}
           myBlynk.RelaySelect(3);  //RX2 SHOWS RX3 IN HMI
    #endif
    DvrChOn = true;
    Av_Rx = SOLO_VIDEO;
      
   }
   
   else 
   {
      digitalWrite(AV_RX_DVR_PIN_2, HIGH); 
      DvrChOn = false;
   }
   
}

/**************************************************END OF VIDEO RC CONTROL ZONE***************************************************************/


#endif
