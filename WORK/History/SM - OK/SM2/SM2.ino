#include "Frames.h"

#define ESP_NO 0
void gpioMode()
{
  pinMode(DIP1, INPUT_PULLUP);
//  pinMode(DIP2, INPUT_PULLUP);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);
  delay(50); // let signals stabilize

  bool dip1 = (digitalRead(DIP1) == LOW);
 // bool dip2 = (digitalRead(DIP2) == LOW);

  // Create mode value (0–3)
 // mode = (dip1 ? 1 : 0) ;//| (dip2 ? 2 : 0);
  
  #if ESP_NO
    mode = (dip1 ? 1 : 0) ;//| (dip2 ? 2 : 0);
  #else
    mode = (dip1 ? 0 : 1) ;//| (dip2 ? 2 : 0);
  #endif
  
  if(mode == 1) 
    {
        startupMode = FLASH_OPEN;
        flashing = true ;
    }
    String modeStr = "JUST OPEN ";
    if(startupMode == FLASH_OPEN) modeStr = "FLASH THEN OPEN ";
    Serial.println("");
    Serial.println("Startup Mode: "+ modeStr  );
  
}

void setup() {
    Serial.begin(Baud_Rate);
    delay(1000);
    gpioMode();
}



//MAIN STATE MACHINE LOOP
void loop() {

  switch (state) {
    case INIT_9600:
            preUpload1_Change_BR_9600_Send_Wake();
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
                    openLockAckReceived =getLockAck(7 , 0x03 ,lockAckTimeout);
                      if (openLockAckReceived ) 
                       {
                        DEBUG_PRINTLN("1ST OPEN ACK RECEIVED FROM LOCK");
                        openLockAckReceived = false;
                        state = GET_OPEN_ACK2;
                        }
                     else
                       {
                        DEBUG_PRINTLN("1ST OPEN ACK NOT RECEIVED FROM LOCK");
                        if( flashing ) state = INIT_9600 ;
                        else state = ERROR_STATE ;
                       }  
    break; 

    case GET_OPEN_ACK2:
                        getLockAck(7, 0xFF,lockAckTimeout);
                        delay (1000);
                        DEBUG_PRINTLN("");
                        DEBUG_PRINTLN("DOOR OPEN WITH SUCCESS, SENDING OPEN CONFIRMATION TO THE LOCK");
                        sendFrame(To_LCU_ACK_RECEIVED, sizeof(To_LCU_ACK_RECEIVED));
                        state = END_STATE;
    break;

    case CMD_OPEN_CONF:
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
    delay(1000);
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
    delay(3000);
    sendFrame(To_LCU_CLOSE_SESSION, sizeof(To_LCU_CLOSE_SESSION));
    delay(500);
    ack =getLockAck(7 , 0x03 ,lockAckTimeout);
    if(ack) 
     {
      DEBUG_PRINTLN("CLOSING UPLOAD SESSION SUCCES");
      
        if( startupMode == FLASH_OPEN) state = CMD_OPEN_38400;
        else state = END_STATE;
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
