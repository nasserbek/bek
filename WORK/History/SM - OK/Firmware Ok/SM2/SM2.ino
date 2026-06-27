#include "Frames.h"




void setup() {
  // put your setup code here, to run once:
    Serial.begin(Baud_Rate);
    pinMode(2, OUTPUT);
    digitalWrite(BLUE_LED, LOW);

    delay(500);
    DEBUG_PRINTLN("READY TO START");
    flashing = false;
}



//MAIN STATE MACHINE LOOP
void loop() {
  #if FLASH
      flashing = true ;
  #endif 
   
  switch (state) {

    case INIT_9600:
    preUpload1_Change_BR_9600_Send_Wake();
//                    DEBUG_PRINTLN("INIT 9600");
//                    
//                    Change_BR(9600);
//                    
//                    state = WAKE_9600;
//           state = GET_FLASH_ACK1;
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
              
                //    digitalWrite(BLUE_LED, HIGH);
                    if(flashing) state = CMD_FLASH_38400;
                    else state = CMD_OPEN_38400;
    break;

    case CMD_OPEN_38400:
                    DEBUG_PRINTLN("SENDING OPEN COMAND");
                    sendFrame(To_LCU_OPEN_CMD, sizeof(To_LCU_OPEN_CMD));
                //    delay(200);
                    state = GET_OPEN_ACK1;
             //       digitalWrite(BLUE_LED, LOW);
    break; 

    case GET_OPEN_ACK1:
                    openLockAckReceived =getLockAck(7 , 0x03 ,lockAckTimeout);
                      if (openLockAckReceived ) 
                       {
                        DEBUG_PRINTLN("1ST OPEN ACK RECEIVED FROM LOCK");
                        openLockAckReceived = false;
                //        delay(200);
                        state = GET_OPEN_ACK2;
               //         digitalWrite(BLUE_LED, HIGH);
                        }
                     else
                       {
                        DEBUG_PRINTLN("1ST OPEN ACK NOT RECEIVED FROM LOCK");
                   //     digitalWrite(BLUE_LED, LOW);
                        #if OPEN_FLASH
                            flashing = true  ;
                            state = INIT_9600 ;
                        #else
                            state = ERROR_STATE ;
                        #endif              
                       }  
    break; 

    case GET_OPEN_ACK2:
                       openLockAckReceived =getLockAck(39 , 0x03 ,lockAckTimeout);
                        if (openLockAckReceived ) 
                         {
                          DEBUG_PRINTLN("2ND OPEN ACK RECEIVED FROM LOCK");
                   //       delay(200);
                          state = CMD_OPEN_CONF;
                  //        digitalWrite(BLUE_LED, HIGH);
                          }
                       else
                         {
                          DEBUG_PRINTLN("2ND OPEN ACK NOT RECEIVED FROM LOCK");
                   //       digitalWrite(BLUE_LED, LOW);
                          #if OPEN_FLASH
                              flashing = true  ;
                              state = INIT_9600 ;
                          #else
                              state = ERROR_STATE ;
                          #endif 
                         }  
    break;

    case CMD_OPEN_CONF:
                        DEBUG_PRINTLN("SENDING OPEN CONFIRMATION TO THE LOCK");
                        sendFrame(To_LCU_ACK_RECEIVED, sizeof(To_LCU_ACK_RECEIVED));
                        openLockAckReceived = false;
              //          delay(200);
                        state = END_STATE;
                   //     digitalWrite(BLUE_LED, HIGH);
     break;

/*********************** FLASHING ********************************************/
    case CMD_FLASH_38400: // Send 1002000060ffff10106ee24f1e18860f3b3ef1fd3c16476a0caad5fcc91003 and wait for 1002000c0c1003 at 38400
        preUpload_cmd1_38400();
    break;

    case GET_FLASH_ACK1:   // SetBaudRate 9600  send ff16 and wait for  0600343020202f080000
       preUpload1_Change_BR_9600_Send_Wake();
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
  ack =getLockAck(7 , 0x03 ,lockAckTimeout);
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


void preUpload1_Change_BR_9600_Send_Wake()
{
    Change_BR(9600);
    
    DEBUG_PRINTLN("SENDING ONLY ONE WAKE (ff16 loop)");
    sendFrame(TO_LCU_WAKE, sizeof(TO_LCU_WAKE));
//    delay(200);
    ack =getLockAck(10 , 0x00 ,lockAckTimeout);
    if(ack) 
     {
      DEBUG_PRINTLN("SENDING 2ND FLASH CMD AT 9600");
      if(flashing)state = GET_FLASH_ACK2;
      else state = CHANGE_BAUDRATE_38400 ;
    }

  else
   {
    DEBUG_PRINTLN("FLASH WAKE ACK NOT RECEIVED FROM LOCK");
  //  digitalWrite(BLUE_LED, LOW);
    state = WAKE_10;
  //   state = GET_FLASH_ACK3_06;
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
    delay(2500);
    sendFrame(To_LCU_CLOSE_SESSION, sizeof(To_LCU_CLOSE_SESSION));
    delay(500);
    ack =getLockAck(7 , 0x03 ,lockAckTimeout);
    if(ack) 
     {
      DEBUG_PRINTLN("CLOSING UPLOAD SESSION SUCCES");
      state = END_STATE;
    }

    else
     {
      DEBUG_PRINTLN("CLOSING UPLOAD SESSION FAILED ACK NOT RECEIVED");
       state = ERROR_STATE ;
     }      
  
}


void endState()
{
          flashing = false;
          delay(500);
          digitalWrite(2, LOW);
          delay(500);
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
