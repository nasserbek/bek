// zap.h

#ifndef ZAP_H
#define ZAP_H



void videoChanel(int ch, bool cmd)
{
    zaptime= millis(); zaptimeOff= millis();  //Stop Zap timers to show another room not in Zap list on the fly
 
    if (zapSetup)videoCh[ch].zap=cmd;
    
    if(!zapSetup)
    {
      remoteControlRcCh = ch;
      recevierCh        = ch;
      room ( remoteControlRcCh, recevierCh , Av_Rx , cmd );
    }
     
    if (lastSelectedCh !=0 && !zapOnOff && !zapSetup && lastSelectedCh != ch && (hmi == BLYNK)) myBlynk.TurnOffLastCh( ACK_BAD ,lastSelectedCh,CH_MODE_0);
    lastSelectedCh = ch;
    
    if (hmi == NODE_RED) 
    {
      if(cmd) myBlynk.TurnOffLastCh( lastAck,ch,CH_MODE_4);
      else myBlynk.TurnOffLastCh( lastAck,ch,CH_MODE_0);
    }  
    myBlynk.releActiveCh(selected_Rx, ch);
}

/**************************************************ZAPPING ZONE***************************************************************/

void resetRemoteRC(void)
{
      RC_Remote_CSR1 =false; myBlynk.resetRemoteRC(1);
      RC_Remote_CSR2 =false; myBlynk.resetRemoteRC(2);
      RC_Remote_CSR3 =false;  myBlynk.resetRemoteRC(3);  
}
void resetZapper (void)
{
  zaptime= millis(); 
  zaptimeOff= millis();
  scantime= millis();
  resetRemoteRC();
  previousCh =0;
  catchCh = false;
  stateMachine =SM_CH1_A;
  
}


void nextState( int nextSm)
    {
      stateMachine =nextSm;
      resetRemoteRC();      
    } 

 void automaticOn(int chanel)
    {
        #ifdef CSR3    
          if  (  chanel == R_24 || chanel == R_28 || chanel == R_48 || chanel == R_29 || chanel == R_66 || chanel == R_68) RC_Remote_CSR2 =true; 
          if  (  chanel == R_49 || chanel == R_50 || chanel == R_51 || chanel == R_52) RC_Remote_CSR1 =true; 
        #endif       

        #ifdef CSR2      
          if  ( chanel != R_24 && chanel != R_28 && chanel != R_48 && chanel != R_29   && chanel != R_66 && chanel != R_68) RC_Remote_CSR3 =true;
          if  (  chanel == R_49 || chanel == R_50 || chanel == R_51 || chanel == R_52) RC_Remote_CSR1 =true; 
        #endif      

        #ifdef CSR      
        if        ( chanel == R_24 ||  chanel == R_28 || chanel == R_48 || chanel == R_29 || chanel == R_66 || chanel == R_68) RC_Remote_CSR2 =true; 
        else  if  ( chanel != R_49 && chanel != R_50 && chanel != R_51 && chanel != R_52) RC_Remote_CSR3 =true;
         #endif  
    }    


void automaticOff(int chanel)
    {
        #ifdef CSR3      
          if  ( chanel == R_51  ) RC_Remote_CSR1 =true;
          if  ( chanel == R_49 || chanel == R_50 || chanel == R_24 || chanel == R_25 || chanel == R_26 || chanel == R_28 || chanel == R_66 || chanel == R_68 ) RC_Remote_CSR2 =true;
        #endif       

         
        #ifdef CSR2     
          if  ( chanel == R_51  ) RC_Remote_CSR1 =true;
          else if ( chanel != R_49 && chanel != R_50 && chanel != R_24 && chanel != R_25 && chanel != R_26 && chanel != R_28  && chanel != R_66 && chanel != R_68) RC_Remote_CSR3 =true;
        #endif      

        #ifdef CSR      
          if  ( chanel == R_49 || chanel == R_50 || chanel == R_24 || chanel == R_25 || chanel == R_26 || chanel == R_28 || chanel == R_66 || chanel == R_68) RC_Remote_CSR2 =true;
          else if  ( chanel != R_51  )                  RC_Remote_CSR3 =true;
        #endif  
    } 
        
void turnOn (int ch, int prevCh,  int smb,  int sma)
    {
      
      zaptime= millis();
      zaptimeOff= millis(); 
      scantime= millis();
      catchCh = false; 
      myBlynk.repeatSync(catchCh);
      
      stateMachine =smb;
      
      if (!autoRemoteLocalRc) automaticOn(ch);
      if (prevCh == 0)
        {
          recevierCh=videoCh[ch].id;     
          receiverAvByCh (recevierCh,1);
          myBlynk.TurnOffLastCh( lastAck,recevierCh,CH_MODE_1);
        }
      if (!zapScanOnly) remoteControl(ch);   
     }
     
void turnOff(int ch, int prevCh, int smc )
 {  
          if (!autoRemoteLocalRc) automaticOff(prevCh);
          if (prevCh != 0)
            {
              recevierCh=videoCh[ch].id; 
              receiverAvByCh (recevierCh,0); 
              myBlynk.TurnOffLastCh( lastAck,recevierCh,CH_MODE_1);
              myBlynk.TurnOffLastCh( lastAck,prevCh,CH_MODE_2);
              if (!zapScanOnly) remoteControl(prevCh);
            }
          stateMachine =smc;
 }
 
 void zapping (int ch, int sma, int smb, int smc, int nextSm)
{
  if (videoCh[ch].zap) 
      {
            if (stateMachine == sma || catchCh == true) turnOn(ch,previousCh, smb, sma); 
            if ( ( !zapScanOnly && (stateMachine == smb) &&  (millis() - zaptimeOff > zapTimerOff ) ) || (zapScanOnly && (stateMachine == smb)  )  ) turnOff(ch, previousCh, smc);
            if ( ( !zapScanOnly && (stateMachine == smc) && (millis() - zaptime > zapTimer) ) || ( zapScanOnly && (stateMachine == smc) && (millis() - scantime > scanTimer) ) ) { nextState( nextSm); previousCh = ch;}
      }  
  else nextState(nextSm);                     
}


void zappingAvCh (bool zapCmd, int zapTimer)
{
 if( (Av_Rx !=1) || zapSetup )  //Stop Zapping if RC or Both is selected or Zsetup
  {
      zaptime= millis();
      zaptimeOff= millis(); 
  }
  
 if( zapCmd && (Av_Rx==1) && (!zapSetup) )
  { 
         switch (stateMachine)
          {
            case SM_CH1_A:    //24
            case SM_CH1_B:
            case SM_CH1_C:
                        zapping (1, SM_CH1_A, SM_CH1_B, SM_CH1_C, SM_CH2_A);
            break;

            case SM_CH2_A:    //25 48 67    2 - 7 -18
            case SM_CH2_B:
            case SM_CH2_C:
                        zapping (2, SM_CH2_A, SM_CH2_B, SM_CH2_C, SM_CH3_A);
            break;
            
            case SM_CH3_A:    //26  65   3-16
            case SM_CH3_B:
            case SM_CH3_C:
                        zapping (3, SM_CH3_A, SM_CH3_B, SM_CH3_C, SM_CH4_A);
            break;

            case SM_CH4_A:    //27  50  66  4-9-17  
            case SM_CH4_B:
            case SM_CH4_C:
                        zapping (4, SM_CH4_A, SM_CH4_B, SM_CH4_C, SM_CH5_A);
            break;
                       
            case SM_CH5_A:    //28  52  5-11
            case SM_CH5_B:
            case SM_CH5_C:
                        zapping (5, SM_CH5_A, SM_CH5_B, SM_CH5_C, SM_CH6_A);
            break;
                       
            case SM_CH6_A:    //29 53   6-12
            case SM_CH6_B:
            case SM_CH6_C:
                        zapping (6, SM_CH6_A, SM_CH6_B, SM_CH6_C, SM_CH7_A);
            break;
                       
            case SM_CH7_A:    //48-25-67  7 - 2 - 18
            case SM_CH7_B:
            case SM_CH7_C:
                        zapping (7, SM_CH7_A, SM_CH7_B, SM_CH7_C, SM_CH8_A);
            break;
                       
            case SM_CH8_A:    //49 64  8-15
            case SM_CH8_B:
            case SM_CH8_C:
                        zapping (8, SM_CH8_A, SM_CH8_B, SM_CH8_C, SM_CH9_A);
            break;

            case SM_CH9_A:    //50 27 66  9-4-17
            case SM_CH9_B:
            case SM_CH9_C:
                        zapping (9, SM_CH9_A, SM_CH9_B, SM_CH9_C, SM_CH10_A);
            break;
            
            case SM_CH10_A:   //51 62  10-13
            case SM_CH10_B:
            case SM_CH10_C:
                        zapping (10, SM_CH10_A, SM_CH10_B, SM_CH10_C, SM_CH11_A);
            break; 
             
            case SM_CH11_A:   //52 28  11-5
            case SM_CH11_B:
            case SM_CH11_C:
                        zapping (11, SM_CH11_A, SM_CH11_B, SM_CH11_C, SM_CH12_A);
            break;     

            case SM_CH12_A:   //53 29   12-6
            case SM_CH12_B:
            case SM_CH12_C:
                        zapping (12, SM_CH12_A, SM_CH12_B, SM_CH12_C, SM_CH13_A);
            break;  
            
            case SM_CH13_A:   // 62 51  13-10
            case SM_CH13_B:
            case SM_CH13_C:
                        zapping (13, SM_CH13_A, SM_CH13_B, SM_CH13_C, SM_CH14_A);
            break;  

            case SM_CH14_A:   //63 68  14-19
            case SM_CH14_B:
            case SM_CH14_C:
                        zapping (14, SM_CH14_A, SM_CH14_B, SM_CH14_C, SM_CH15_A);
            break;  
            
            case SM_CH15_A:   //64 49  15-8
            case SM_CH15_B:
            case SM_CH15_C:
                        zapping (15, SM_CH15_A, SM_CH15_B, SM_CH15_C, SM_CH16_A);
            break;                         

            case SM_CH16_A:   //65 26  16-3
            case SM_CH16_B:
            case SM_CH16_C:
                        zapping (16, SM_CH16_A, SM_CH16_B, SM_CH16_C, SM_CH17_A);
            break;                         

            case SM_CH17_A:   //66 27 50  17-4-9
            case SM_CH17_B:
            case SM_CH17_C:
                        zapping (17, SM_CH17_A, SM_CH17_B, SM_CH17_C, SM_CH18_A);
            break;                         


            case SM_CH18_A:   //67-25-48  18 - 2 - 7
            case SM_CH18_B:
            case SM_CH18_C:
                        zapping (18, SM_CH18_A, SM_CH18_B, SM_CH18_C, SM_CH19_A);
            break;                         

            case SM_CH19_A: //68  63  19-14
            case SM_CH19_B:
            case SM_CH19_C:
                        zapping (19, SM_CH19_A, SM_CH19_B, SM_CH19_C, SM_CH20_A);
            break;                         

            case SM_CH20_A:  //20
            case SM_CH20_B:
            case SM_CH20_C:
                        zapping (20, SM_CH20_A, SM_CH20_B, SM_CH20_C, SM_CH1_A);
            break;                         

                                                           
          }
  }
  else if (!zapCmd) {zaptime= millis();stateMachine =SM_CH1_A;}
}


void zapAllOnOff(bool cmd)
{
  if (cmd)
        {
          for (int ch=1;ch<20;ch++) 
            {      
              if(ch != ROUTER  )
                {
                  videoCh[ch].zap=1;
                  myBlynk.TurnOffLastCh( ACK_BAD ,ch,CH_MODE_4); 
                  delay(200);                                 
                }
            }
        }
  else
        {
          for (int ch=1;ch<20;ch++) 
            {      
              if(ch != ROUTER)
                {
                  videoCh[ch].zap=0;
                  myBlynk.TurnOffLastCh( ACK_BAD ,ch,CH_MODE_0);      
                  delay(200);                            
                }
            }
        }
}
/**************************************************END OF ZAPPING ZONE***************************************************************/

#endif
