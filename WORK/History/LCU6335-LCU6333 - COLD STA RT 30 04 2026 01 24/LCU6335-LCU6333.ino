#define DEBUG 0
bool    LockLink    = false;
bool    woke        = false;

bool    EnColdStart = true;
bool    flashing    = false;
bool    OpenOnly    = true;
String  modeStr     = "JUST OPEN ";

#include "Frames.h"
#include "FramesLL.h"
#include "Frames_ColdStart.h"

void gpioMode()
{
  pinMode(DIP1, INPUT_PULLUP);
  pinMode(DIP2, INPUT_PULLUP);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
  delay(50); // let signals stabilize

  bool dip1 = (digitalRead(DIP1) == LOW);
  bool dip2 = (digitalRead(DIP2) == LOW);

  // Create mode value (0–3)
    mode = (dip1 ? 1 : 0) | (dip2 ? 2 : 0);
    
    if(mode == 0) 
    {
        startupMode = OPEN_ONLY;
        state       = INIT_9600;
        modeStr     = "JUST OPEN ";
        OpenOnly    = true;
        LockLink    = false;
        flashing    = false ;
        EnColdStart = false;
    }

    else if(mode == 1)
    {
      state       = CS_pkt2705_1;
      startupMode = COLD_START;
      modeStr     = "COLD START ";
      EnColdStart = true;
      OpenOnly    = false;
      LockLink    = false;
      flashing    = false ;
    }  
        
    else if(mode == 2) 
    {
        startupMode = FLASH_THEN_OPEN;
        state       = INIT_9600;
        modeStr     = "FLASH AND OPEN ";
        flashing    = true ;
        OpenOnly    = false;
        LockLink    = false;
        EnColdStart = false;
    }
 
    else if(mode == 3)
    {
      state       = LL_pktWake;
      startupMode = OPEN_LOCKLINK;
      modeStr     = "LOCK LINK ";
      LockLink    = true;
      OpenOnly    = false;
      flashing    = false ;
      EnColdStart = false;
    }  
    
    Serial.println("");
    Serial.println("Startup Mode: "+ modeStr  );
}



void setup() {
    Serial.begin(Baud_Rate_Monitor);
    delay(1000);
    gpioMode();
}


//MAIN STATE MACHINE LOOP
void loop() {

  switch (state) {
    case INIT_9600:
            FirstWakeSync_BR9600();
    break;
   
    case WAKE_10:
                    DEBUG_PRINTLN("WAKE (ff16 loop)");
                    for (int i = 0; i < 10; i++) {
                       sendFrame(TO_LCU_WAKE, sizeof(TO_LCU_WAKE));
                       
                       led   = !led; 
                       digitalWrite(BLUE_LED, led);
                       
                       delay(150);
                    }
                    state = CHANGE_BAUDRATE_38400;
     break;

    case CHANGE_BAUDRATE_38400:
                    DEBUG_PRINTLN("SWITCH 38400");
                    
                    Change_BR(38400);
                    delay(1000);
                    if(flashing) state = CMD_FLASH_38400;
                    else state = CMD_OPEN_38400;
    break;

    case CMD_OPEN_38400:
                    DEBUG_PRINTLN("SENDING OPEN COMAND");
                    sendFrame(To_LCU_OPEN_CMD, sizeof(To_LCU_OPEN_CMD));

                    state = GET_OPEN_ACK1;
    break; 

    case GET_OPEN_ACK1:
                    openLockAckReceived =getLockAck(7 , 0x10, 0x03 ,lockAckTimeout);
                      if (openLockAckReceived ) 
                       {
                        DEBUG_PRINTLN("1ST OPEN ACK RECEIVED FROM LOCK");
                        openLockAckReceived = false;
                        state = GET_OPEN_ACK2;
                        }
                     else
                       {
                        DEBUG_PRINTLN("1ST OPEN ACK NOT RECEIVED FROM LOCK");
                        state = ERROR_STATE ;
                       }  
    break; 

    case GET_OPEN_ACK2:
                        getLockAck(7,  0x10,0xFF,lockAckTimeout);
                        delay (1000);
                        DEBUG_PRINTLN("");
                        DEBUG_PRINTLN("DOOR OPEN WITH SUCCESS, SENDING OPEN CONFIRMATION TO THE LOCK");
                        sendFrame(To_LCU_ACK_RECEIVED, sizeof(To_LCU_ACK_RECEIVED));
                        state = END_STATE;
    break;

/*********************** COLD START ********************************************/
    case CS_pkt2705_1:
                    DEBUG_PRINTLN("COLD START WAKE (ff16 loop)");
                    Change_BR(9600);
                    delay(3000);
                    idx = 0;
                    CSstart = millis();

  
                while (millis() - CSstart < CSTimeout) {                    
                   if(!woke)
                   {
                     sendFrame(pkt2705_1, sizeof(pkt2705_1));
                     
                     led   = !led; 
                     digitalWrite(BLUE_LED, led);
                     idx = 0;
                     delay(150);
                     ack =getLockAck(10 , 0X06,0x00 ,500);  
                     
                     if(ack) 
                     {
                      woke = true;
                      state = CS_pkt2727_1;
                      DEBUG_PRINTLN("");
                      DEBUG_PRINTLN("COLD START WOKE ok");
                      return;
                     }
                   }
                }
                
               if(!woke)state = state = ERROR_STATE ;
  
    break;

    case CS_pkt2727_1:
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("COLD START WOKE SUCCEFULLY SENDIND pkt2727_1 ");
            delay(200);
            sendFrame(pkt2727_1, sizeof(pkt2727_1));
            ack =waitACK(200);
            if(ack)  state = CS_pkt2749_1;
             else  
            {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED pkt2727_1 ");  
              state = ERROR_STATE ;
            }
    break;

    case CS_pkt2749_1:
            DEBUG_PRINTLN("ACK RECIEVED FROM pkt2727_1 SWITCHING TO 57600 TO START COLD START FLASHING");
            Change_BR(57600);
            CSfirmwareUpload();  
    break;  
     
    case CS_pkt2799_1:
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("COLD START FLASH DONE SUCCEFULLY SENDIND pkt2799_1 ");
            delay(200);
            sendFrame(pkt2799_1, sizeof(pkt2799_1));
            ack =waitACK(200);
            if(ack)  state = CS_pkt3135_1;
             else  
            {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED pkt2799_1 ");  
              state = ERROR_STATE ;
            }
    break;

    case CS_pkt3135_1:
            delay(3000);
            DEBUG_PRINTLN("SWITCHING TO 38400 END OF FLASHING");
            Change_BR(38400);
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt3135_1 AT 38400 ");
            delay(200);
            sendFrame(pkt3135_1, sizeof(pkt3135_1));

            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt3401_1 AT 38400 ");
            delay(2000);
            sendFrame(pkt3401_1, sizeof(pkt3401_1));

            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt3771_1 AT 38400 ");
            delay(2000);
            sendFrame(pkt3771_1, sizeof(pkt3771_1));

            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt4079_1 AT 38400 ");
            delay(2000);
            sendFrame(pkt4079_1, sizeof(pkt4079_1));

            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt4351_1 AT 38400 ");
            delay(2000);
            sendFrame(pkt4351_1, sizeof(pkt4351_1));

            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt4617_1 AT 38400 ");
            delay(2000);
            sendFrame(pkt4617_1, sizeof(pkt4617_1));

            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt4883_1 AT 38400 ");
            delay(2000);
            sendFrame(pkt4883_1, sizeof(pkt4883_1));

            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt5149_1 AT 38400 ");
            delay(3000);
            sendFrame(pkt5149_1, sizeof(pkt5149_1));
            
            delay(150);
            ack =getLockAck(7 ,0x10,0x03 ,lockAckTimeout);  
           
             if(ack) state = CS_pkt5415_1;
             else  
              {
                DEBUG_PRINTLN("");
                DEBUG_PRINTLN("NO ACK RECEIVED pkt5149_1 ");  
                state = ERROR_STATE ;
              }
            
    break;

    case CS_pkt5415_1:
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("SENDIND pkt5415_1 AT 38400 ");
            delay(2000);
            sendFrame(pkt5415_1, sizeof(pkt5415_1));
            
            delay(150);
            ack =getLockAck(54 ,0x10, 0x03 ,lockAckTimeout);  
           
            if(ack) 
            {
              state = END_STATE;
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("COLD START DONE SUCCESSIFULLY, OPENING DOOR..... ");
              delay(3000);
              state = CMD_OPEN_38400;
            }
             else  
              {
                DEBUG_PRINTLN("");
                DEBUG_PRINTLN("NO ACK RECEIVED pkt5415_1 ");  
                state = ERROR_STATE ;
              }
    break;
/*********************** END OF COLD START  ********************************************/


    
/*********************** FLASHING ********************************************/
    case CMD_FLASH_38400: // Send 1002000060ffff10106ee24f1e18860f3b3ef1fd3c16476a0caad5fcc91003 and wait for 1002000c0c1003 at 38400
        preUpload_cmd1_38400();
    break;

    case GET_FLASH_ACK1:   // SetBaudRate 9600  send ff16 and wait for  0600343020202f080000
       FirstWakeSync_BR9600();
    break; 

    case GET_FLASH_ACK2: //send  02010000f90ef8and wait for 06 then SetBaudRate 57600
        preUpload_cmd2_9600_Change_BR_57600();
     break; 
       
    case FIRMWARE_UPLOAD_57600:
        firmwareUpload();  
    break;

    case END_OF_UPLOAD:
       uploadDone();
    break; 

    case TERMINATE_UPLOAD_SUCCESS:
        terminateUpload();
    break;    

/*********************************************** START OF LOCK LINK *****************************************************/
    case LL_pktWake:
                    DEBUG_PRINTLN("WAKE (ff16 loop)");
                    Change_BR(9600);
                    delay(3000);
                    woke = false;
                    idx = 0;
                    
                    while(!woke)
                   {
                     sendFrame(pktWake, sizeof(pktWake));
                     
                     led   = !led; 
                     digitalWrite(BLUE_LED, led);
                     idx = 0;
                     delay(150);
                     ack =getLockAck(10 ,0, 0x20 ,lockAckTimeout);
                     
                     if(ack) woke = true;
                   }
                    if(!LockLink)state = CHANGE_BAUDRATE_38400;
                    else state = LL_pkt3642;
     break;

      case LL_pkt3642:
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("LL WOKE SEND pkt3642 ");
            delay(3000);
            sendFrame(pkt3642, sizeof(pkt3642));
            ack =getLockAck(39 , 0,  0x20 ,lockAckTimeout);
            //delay(50);
            if(ack)  state = LL_pktff06;
             else  
            {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3642 ");  
              state = ERROR_STATE ;
            }
      break;


      case LL_pktff06:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FORpkt3642, SENDING pktff06 ff06 ");
          //  delay(500);
            sendFrame(pktff06, sizeof(pktff06));  
          //  delay(50);
            state = LL_pkt3656;
            
      break;

      case LL_pkt3656:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("LL WOKE SEND pkt3656 ");
            sendFrame(pkt3656, sizeof(pkt3656));  
            delay(50);
            ack =getLockAck(1 ,0, 0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3662;
             else  
           {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3656 ");  
              state = ERROR_STATE ;
            }
   
      break;
      
      case LL_pkt3662:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3656, SEND pkt3662 ");
            sendFrame(pkt3662, sizeof(pkt3662));  
            delay(200);
            ack =getLockAck(8, 0, 0xD7 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3670;
             else  
           {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3662 ");  
              state = ERROR_STATE ;
            }
     
      break;
      
      case LL_pkt3670:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3662 ,  SEND pkt3670 ");
            sendFrame(pktff06, sizeof(pktff06));  //FF06
            delay(500);
            sendFrame(pkt3670, sizeof(pkt3670));  
            delay(200);
            ack =getLockAck(8, 0, 0xB2 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3678;
             else  
          {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3670 ");  
              state = ERROR_STATE ;
            }         
      break;
      
      case LL_pkt3678:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3670, SEND pkt3678 ");
            sendFrame(pktff06, sizeof(pktff06));  //FF06
            delay(500);
            sendFrame(pkt3678, sizeof(pkt3678));  
            delay(200);
            ack =getLockAck(1,  0,0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3684;
             else  
            {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3678 ");  
              state = ERROR_STATE ;
            }   
      break;
      
      case LL_pkt3684:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3678, SEND pkt3684 ");
            sendFrame(pkt3684, sizeof(pkt3684));  
            ack =getLockAck(1, 0, 0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3694;
             else  
           {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3684 ");  
              state = ERROR_STATE ;
            }      
      break;
      
      case LL_pkt3694:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3684 SEND pkt3694 ");
            sendFrame(pkt3694, sizeof(pkt3694));  
            ack =getLockAck(1, 0, 0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3704;
             else  
           {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3694 ");  
              state = ERROR_STATE ;
            }        
      break;
      
      case LL_pkt3704:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3694 ,SEND pkt3704 ");
            sendFrame(pkt3704, sizeof(pkt3704));  
            ack =getLockAck(1, 0, 0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3708;
             else  
           {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3704 ");  
              state = ERROR_STATE ;
            }          
      break;
      
      case LL_pkt3708:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3704 SEND pkt3708 ");
            sendFrame(pkt3708, sizeof(pkt3708));  
            ack =getLockAck(1, 0, 0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state = LL_pkt3714;
             else  
           {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3708 ");  
              state = ERROR_STATE ;
            }         
      break;
      
      case LL_pkt3714:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3708 SEND pktpkt3714 ");
            sendFrame(pkt3714, sizeof(pkt3714));  
            ack =getLockAck(1, 0, 0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state =LL_pkt3722 ;
             else  
            {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3714 ");  
              state = ERROR_STATE ;
            }         
      break;
      
      case LL_pkt3722:
      DEBUG_PRINTLN("");
            DEBUG_PRINTLN("ACK RECEIVED FOR pkt3714 SEND pkt3714 ");
            sendFrame(pkt3722, sizeof(pkt3722));  
            ack =getLockAck(1,  0,0x06 ,lockAckTimeout);
            delay(200);
            if(ack)  state = END_STATE;
            else  
            {
              DEBUG_PRINTLN("");
              DEBUG_PRINTLN("NO ACK RECEIVED FOR pkt3722 ");  
              state = ERROR_STATE ;
            }
            
      break;
/*********************************************** END OF LOCK LINK *****************************************************/
    
    case END_STATE:
        endState();
    break;
    
    case ERROR_STATE:
        errorState();
    break;      
  }
  prevState = state;

}
void preUpload_cmd1_38400()
{
  DEBUG_PRINTLN("SENDING FLASH COMAND 1 AT 38400");
  sendFrame(To_LCU_FLASH_CMD, sizeof(To_LCU_FLASH_CMD));
 // delay(200);
  ack =getLockAck(7 ,0, 0x03 ,lockAckTimeout);
   if (ack ) 
    {
      DEBUG_PRINTLN("FLASH ACK RECEIVED FROM LOCK, SWITCH TO 9600");
      flashing = true;
      state = GET_FLASH_ACK1;
   }

  else
   {
    DEBUG_PRINTLN("FLASH CMD1 ACK NOT RECEIVED FROM LOCK");
  //  digitalWrite(BLUE_LED, LOW);
    state = ERROR_STATE ;
   }  
}


void FirstWakeSync_BR9600()
{
    Change_BR(9600);
    delay(1000);
    DEBUG_PRINTLN("SENDING ONLY ONE WAKE (ff16 loop)");
    sendFrame(TO_LCU_WAKE, sizeof(TO_LCU_WAKE));

    ack =getLockAck(10 ,0, 0x00 ,lockAckTimeout);
    if(ack) 
     {
      DEBUG_PRINTLN("SENDING 2ND FLASH CMD AT 9600");
      if(flashing)state = GET_FLASH_ACK2;
      else state = CHANGE_BAUDRATE_38400 ;
    }

  else
   {
    DEBUG_PRINTLN("FLASH WAKE ACK NOT RECEIVED FROM LOCK");
    state = WAKE_10;
   }      
 
}


void preUpload_cmd2_9600_Change_BR_57600()
{
    LockSerial.write(To_LCU_FLASH_CMD2, sizeof(To_LCU_FLASH_CMD2));
//    delay(200);
    ack =waitACK(200);
     if (ack ) 
     {  
      DEBUG_PRINTLN("ACK RECIEVED SWITCHING TO 57600 TO START FLASHING");
      Change_BR(57600);
      state = FIRMWARE_UPLOAD_57600;
     }
 else
   {
    DEBUG_PRINTLN("FLASH 2ND ACK NOT RECEIVED FROM LOCK");
   // digitalWrite(BLUE_LED, LOW);
    state = ERROR_STATE ;
  //   state = GET_FLASH_ACK3_06;
   }  
}
                       

void firmwareUpload()
{
      DEBUG_PRINTLN("STREAM FLASHING .......");
              led   = false;
              for (int i = 0; i < NR_OF_FRAMES; i++) 
                {
                  offset = i * FRAME_SIZE;
                  sendFrame(&frames[offset], FRAME_SIZE);
                  delay(50); // IMPORTANT: matches your trace spacing
                  ack =waitACK(200); 
                  
                  if (!ack  ) 
                  {
                      DEBUG_PRINTLN(" Frame Nr: " + String (i) + " failed, no ACK received");
              //        digitalWrite(BLUE_LED, LOW);
                      state = ERROR_STATE ;
                      return;
                  }    
        
                  if(i == 0)
                  {
                      sendFrame(pkt489_1, sizeof(pkt489_1)); //pkt489_1[22]
                      delay(50); // IMPORTANT: matches your trace spacing
                      ack =waitACK(200);
                      if (!ack  ) 
                      {
                      DEBUG_PRINTLN(" Frame pkt489_1 at index " + String (i) + " failed, no ACK received");
                //      digitalWrite(BLUE_LED, LOW);
                      state = ERROR_STATE ;
                      return;
                      }
                  }
        
                  
                  if(i == 512)
                  {
                      sendFrame(pkt6867_1, sizeof(pkt6867_1)); //pkt6867_1[22]
                      delay(50); // IMPORTANT: matches your trace spacing
                      ack =waitACK(200); 
                      if (!ack  ) 
                      {
                      DEBUG_PRINTLN(" Frame ppkt6867_1kt489_1 at index " + String (i) + " failed, no ACK received");
                //      digitalWrite(BLUE_LED, LOW);
                      state = ERROR_STATE ;
                      return;
                      }
                  }
                   led   = !led; 
                   digitalWrite(BLUE_LED, led);
                }
          state = END_OF_UPLOAD;                
}
  
void uploadDone()
{
            DEBUG_PRINTLN("SENDING FLASH END CMD AT 57600");
            delay(200);
            sendFrame(To_LCU_END_OF_FILE, sizeof(To_LCU_END_OF_FILE));  
            ack =waitACK(200);
            if (ack ) 
             {
              DEBUG_PRINTLN("SWITCHING TO 38400 END OF FLASHING");
              Change_BR(38400);
              state = TERMINATE_UPLOAD_SUCCESS;
            }

           else
             {
              DEBUG_PRINTLN("FLASH END ACK NOT RECEIVED FROM LOCK");
           //   digitalWrite(BLUE_LED, LOW);
              state = ERROR_STATE ;
             }    
}


void terminateUpload()
{
    DEBUG_PRINTLN("CLOSING UPLOAD SESSION");
    delay(3000);
    sendFrame(To_LCU_CLOSE_SESSION, sizeof(To_LCU_CLOSE_SESSION));
    delay(500);
    ack =getLockAck(7 , 0,0x03 ,lockAckTimeout);
    if(ack) 
     {
      DEBUG_PRINTLN("CLOSING UPLOAD SESSION SUCCES, OPENING DOOR");
      state = CMD_OPEN_38400;
    }

    else
     {
      DEBUG_PRINTLN("CLOSING UPLOAD SESSION FAILED ACK NOT RECEIVED");
       state = ERROR_STATE ;
     }      
  
}


void endState()
{
          delay(500);
          digitalWrite(2, LOW);
          delay(500);
          digitalWrite(2, HIGH);
          
          delay(400);
          digitalWrite(2, LOW);
          delay(400);
          digitalWrite(2, HIGH);
          
          delay(300);
          digitalWrite(2, LOW);
          delay(300);
          digitalWrite(2, HIGH); 
            
          delay(200);
          digitalWrite(2, LOW);
          delay(200);
          digitalWrite(2, HIGH);                 

          delay(100);
          digitalWrite(2, LOW);
          delay(100);
          digitalWrite(2, HIGH);
}


void errorState()
{
          flashing = false;
          delay(3000);
          digitalWrite(2, LOW);
          delay(3000);
          digitalWrite(2, HIGH);
}

void CSfirmwareUpload()
{
      DEBUG_PRINTLN("COLD START STREAM FLASHING .......");
                  led   = false;
                  digitalWrite(BLUE_LED, led);
                  sendFrame(pkt2749_1, sizeof(pkt2749_1));
                  delay(50); // IMPORTANT: matches your trace spacing
                  ack =waitACK(200); 
                  
                  if (!ack  ) 
                  {
                      DEBUG_PRINTLN(" pkt2749_1 failed, no ACK received");
                      state = ERROR_STATE ;
                      return;
                  }    
                  
                  
                  led   = !led; 
                  digitalWrite(BLUE_LED, led);
                  sendFrame(pkt2761_1, sizeof(pkt2761_1)); //pkt2761_1[22]
                  delay(50); // IMPORTANT: matches your trace spacing
                  ack =waitACK(200);
                  if (!ack  ) 
                  {
                  DEBUG_PRINTLN(" Frame pkt2761_1 failed, no ACK received");
                  state = ERROR_STATE ;
                  return;
                  }

                  led   = !led; 
                  digitalWrite(BLUE_LED, led);       
                  sendFrame(pkt2767_1, sizeof(pkt2767_1));
                  delay(50); // IMPORTANT: matches your trace spacing
                  ack =waitACK(200); 
                  
                  if (!ack  ) 
                  {
                      DEBUG_PRINTLN(" pkt2767_1 failed, no ACK received");
                      state = ERROR_STATE ;
                      return;
                  }                      
                  
                  
                  led   = !led; 
                  digitalWrite(BLUE_LED, led);
                  sendFrame(pkt2773_1, sizeof(pkt2773_1));
                  delay(50); // IMPORTANT: matches your trace spacing
                  ack =waitACK(200); 
                  
                  if (!ack  ) 
                  {
                      DEBUG_PRINTLN(" pkt2773_1 failed, no ACK received");
                      state = ERROR_STATE ;
                      return;
                  }                      
                  
                  
                  led   = !led; 
                  digitalWrite(BLUE_LED, led);
                  sendFrame(pkt2781_1, sizeof(pkt2781_1));
                  delay(50); // IMPORTANT: matches your trace spacing
                  ack =waitACK(200); 
                  
                  if (!ack  ) 
                  {
                      DEBUG_PRINTLN(" pkt2781_1 failed, no ACK received");
                      state = ERROR_STATE ;
                      return;
                  }                      

                  
                  led   = !led; 
                  digitalWrite(BLUE_LED, led);
                  sendFrame(pkt2789_1, sizeof(pkt2789_1));
                  delay(50); // IMPORTANT: matches your trace spacing
                  ack =waitACK(200); 
                  
                  if (!ack  ) 
                  {
                      DEBUG_PRINTLN(" pkt2789_1 failed, no ACK received");
                      state = ERROR_STATE ;
                      return;
                  }                      


          state = CS_pkt2799_1;                
}
