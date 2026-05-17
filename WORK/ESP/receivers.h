// receivers.h

#ifndef RECEIVERS_H
#define RECEIVERS_H



extern int  Av_Rx;
extern void AvReceiverSel(int queuData);
extern void videoChanel(int ch, bool cmd);
extern bool PowerOnTune;

bool  TCA9548A(uint8_t bus)
{

    Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
    Wire.write(1 << bus);          // send byte to select bus
    return(Wire.endTransmission()); 
}

/**************************************************VIDEO RC CONTROL ZONE***************************************************************/
void remoteControl(int cmd )
{
      if( (!RC_Remote_ESP1) && (!RC_Remote_ESP2)  && (!RC_Remote_ESP3)) mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);  
        
      else if( RC_Remote_ESP1)
       {
        if (blynkConnected) apiSend("ESP1", "V1", cmd); 
        else 
          {
            StaticJsonDocument<54> doc4; //Json to send from
            doc4["RC"] = cmd;
            serializeJson(doc4, Json); // print to client
            client.publish(AWS_IOT_PUBLISH_TOPIC_RC, Json); 
          }
        }

       else if( RC_Remote_ESP2)
       {
        if (blynkConnected) apiSend("ESP2", "V1", cmd);
        else
            {
              StaticJsonDocument<54> doc4; //Json to send from
              doc4["RC"] = cmd;
              serializeJson(doc4, Json); // print to client
              client.publish(AWS_IOT_PUBLISH_TOPIC_RC_2, Json); 
            } 
       }
       
      else if( RC_Remote_ESP3)
      {
        if (blynkConnected) apiSend("ESP3", "V1", cmd);
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
   Wire.beginTransmission(_address);
   Wire.write(MSB );
   Wire.write(LSB );
   Wire.write(0xC2 );
   if(ActiveBoard == ESP0 ) return false;  
   else return (Wire.endTransmission() ); 
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
       
    if(V_Remote_ESP1 || V_Remote_ESP2 || V_Remote_ESP3)
      {
        StaticJsonDocument<54> doc3; //Json to send from
        doc3["CMD"] = cmd;
        serializeJson(doc3, Json); // print to client
        doc3["VIDEO"] = Ch;
        serializeJson(doc3, Json); // print to client
             
          if( V_Remote_ESP1)
           {
             if (blynkConnected) apiSend("ESP1", "V2", Ch);
              else client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_1, Json); 
           }
    
          if( V_Remote_ESP2)
          {
            if (blynkConnected) apiSend("ESP2", "V2", Ch);
            else client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO_2, Json); 
            
          } 
    
           if( V_Remote_ESP3)
          {
            if (blynkConnected) apiSend("ESP3", "V2", Ch);
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
     TCA9548A(queuData-1);
}


void PowerOnTuning(void)
{
        selected_Rx = TCA9548A_CH4;  //CH4
        AvReceiverSel(selected_Rx+1);  
        delay (1000);
        Av_Rx = SOLO_VIDEO; 
        videoChanel(CH_1, ON);
        myBlynk.RelaySelect(selected_Rx+1);
        delay (1000);
        
        selected_Rx = TCA9548A_CH3;  //CH3
        AvReceiverSel(selected_Rx+1);  
        delay (1000);
        Av_Rx = SOLO_VIDEO; 
        videoChanel(CH_1, ON);
        myBlynk.RelaySelect(selected_Rx+1);
        delay (1000);
        
        selected_Rx = TCA9548A_CH2;  //CH2
        AvReceiverSel(selected_Rx+1);  
        delay (1000);
        Av_Rx = SOLO_VIDEO; 
        videoChanel(CH_1, ON);
        myBlynk.RelaySelect(selected_Rx+1);
        delay (1000);

        selected_Rx = TCA9548A_CH1;  //CH1
        AvReceiverSel(selected_Rx+1);  
        delay (1000);
        Av_Rx = SOLO_VIDEO; 
        videoChanel(CH_1, ON);
        myBlynk.RelaySelect(selected_Rx+1);
       
}      


void  dvrOnOff (bool powerOn)
{
   myBlynk.dvrSwitch(powerOn);
   
   if (powerOn) 
   {
    digitalWrite(AV_RX_DVR_PIN_2, LOW); 
    if(PowerOnTune)PowerOnTuning();    
    DvrChOn = true;
    stateDVR = DVR_ON;
    Av_Rx = SOLO_VIDEO;
   }
   else 
   {
      digitalWrite(AV_RX_DVR_PIN_2, HIGH); 
      DvrChOn = false;
      stateDVR = DVR_OFF;
   }
}



/**************************************************END OF VIDEO RC CONTROL ZONE***************************************************************/


#endif
