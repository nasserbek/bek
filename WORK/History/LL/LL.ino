//#include <SoftwareSerial.h>
#define DEBUG
#ifdef DEBUG
 #define DEBUG_PRINT(x)    Serial.print (x)
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_WRITE(x)    Serial.write (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_WRITE(x)  
#endif
const char To_LL_1 [7]           = {0x02,0x01,0x05,0x05,0xf0,0x01,0x04};
const char To_LL_2_Sync [2]      = {0xff,0x16};
const char To_LL_3_change_BR [8] = {0xff,0x02,0x01,0x00,0x00,0xf9,0x0c,0xfa};
const char To_LL_4 [8]           = {0xff,0x02,0x01,0xa0,0x99,0xfe,0x01,0xc7};
const char To_LL_Ack [2]         = {0xff,0x06};
const char To_LL_5 [8]           = {0xff,0x02,0x01,0x00,0x75,0xfc,0x01,0x8d};
const char To_LL_6 [8]           = {0xff,0x02,0x01,0x00,0x75,0x00,0xc7,0xc3};
const char To_LL_7 [9]           = {0xff,0x02,0x02,0x7c,0x90,0x00,0xc4,0x1e,0x10};
const char To_LL_8 [9]           = {0xff,0x02,0x02,0x7c,0x92,0x00,0x86,0x57,0x13};
const char To_LL_9 [8]           = {0xff,0x02,0x01,0x7f,0xf9,0x00,0x02,0x85};
const char To_LL_10 [8]          = {0xff,0x02,0x01,0xa0,0x02,0xfe,0x01,0x5e};
const char To_LL_11 [8]          = {0xff,0x02,0x01,0xa0,0x02,0xfd,0x82,0xde};
const char To_LL_12 [14]         = {0xff,0x02,0x07,0x02,0x33,0x00,0x01,0x07,0x16,0x02,0x25,0x02,0x1c,0x61};
const char To_LL_13 [9]          = {0xff,0x02,0x02,0x01,0x62,0x00,0x51,0xce,0x7c};
const char To_LL_14 [8]          = {0xff,0x02,0x01,0x7f,0xf9,0x00,0x02,0x85};
const char To_LL_15 [11]         = {0xff,0x02,0x04,0x7f,0xfa,0x00,0x00,0x80,0x00,0x00,0x03};
const char To_LL_16 [7]          = {0xff,0x02,0x00,0x00,0x00,0x01,0xff};


HardwareSerial LockLink_Serial(2);  // UART2
//SoftwareSerial LockLink_Serial(2,3);
char i,j;
int Baud_Rate_1 = 9600;   // initial baud rate - default of the module
int Baud_Rate_2 = 19200;   // initial baud rate - default of the module
bool br=false;

void setup()
{
    Serial.begin(Baud_Rate_1);
    
    LockLink_Serial.begin(Baud_Rate_1);
    pinMode(2, OUTPUT);
//    pinMode(2, INPUT);
//    pinMode(13, OUTPUT);
    digitalWrite(2, HIGH);
    
    delay (3000);
    DEBUG_PRINTLN("READY TO START");
    DEBUG_PRINTLN("Sending a Sync command to LL 0x02,0x01,0x05,0x05,0xf0,0x01,0x04");
    for (j=0;j<6;j++)
      { 
       for (i=0;i<7;i++){ LockLink_Serial.write(To_LL_1 [i]);}
        delay(150);
      }
    digitalWrite(2, LOW);

    DEBUG_PRINTLN("Waiting for Ack from LL by sending sync 0xff,0x16");
//    while (br==false)
//    {
     for (i=0;i<10;i++){ LockLink_Serial.write(To_LL_2_Sync [i]);} 
     delay(150);
  //   if ( From_LL_Ack() ) {br=true;break;}
//    }
    digitalWrite(2, HIGH);
    
    br =false;
    
    DEBUG_PRINTLN("Received Ack from LL, changing baud rate to 19200  sending 0xff,0x02,0x01,0x00,0x00,0xf9,0x0c,0xfa");
    for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_3_change_BR [i]);}  
    delay(150);
    digitalWrite(2, LOW);
    
    Change_BR();
    delay(150);       
    digitalWrite(2, HIGH);

    DEBUG_PRINTLN("Sending 0xff,0x02,0x01,0xa0,0x99,0xfe,0x01,0xc7");
    for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_4 [i]);}  
    delay(150);
   digitalWrite(2, LOW);

   DEBUG_PRINTLN("Received answer from LL , Sending Ack to LL 0xff,0x06");
   for (i=0;i<2;i++){ LockLink_Serial.write(To_LL_Ack [i]);} 
   delay(150);
   digitalWrite(2, HIGH);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x01,0x00,0x75,0xfc,0x01,0x8d");
   for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_5[i]);}  
   delay(150);
   digitalWrite(2, LOW);
   
   DEBUG_PRINTLN("Received answer from LL , Sending Ack to LL 0xff,0x06");
   for (i=0;i<2;i++){ LockLink_Serial.write(To_LL_Ack [i]);} 
   delay(150);
   digitalWrite(2, HIGH);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x01,0x00,0x75,0x00,0xc7,0xc3");
   for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_6[i]);}
   delay(150);

   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x02,0x7c,0x90,0x00,0xc4,0x1e,0x10");
   for (i=0;i<9;i++){ LockLink_Serial.write(To_LL_7[i]);}
   delay(150);
   digitalWrite(2, LOW);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x02,0x7c,0x92,0x00,0x86,0x57,0x13");
   for (i=0;i<9;i++){ LockLink_Serial.write(To_LL_8[i]);}
   delay(150);
   digitalWrite(2, HIGH);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x01,0x7f,0xf9,0x00,0x02,0x85");
   for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_9[i]);}  
   delay(150);
   digitalWrite(2, LOW);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x01,0xa0,0x02,0xfe,0x01,0x5e");
   for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_10[i]);}  
   delay(150);
   digitalWrite(2, HIGH);
   
   DEBUG_PRINTLN("Received answer from LL , Sending Ack to LL 0xff,0x06");
   for (i=0;i<2;i++){ LockLink_Serial.write(To_LL_Ack [i]);} 
   delay(150);
   digitalWrite(2, LOW);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x01,0xa0,0x02,0xfd,0x82,0xde");
   for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_11[i]);}  
   delay(150);
   digitalWrite(2, HIGH); 
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x07,0x02,0x33,0x00,0x01,0x07,0x16,0x02,0x25,0x02,0x1c,0x61");
   for (i=0;i<14;i++){ LockLink_Serial.write(To_LL_12[i]);}  
   delay(150);
   digitalWrite(2, LOW);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x02,0x01,0x62,0x00,0x51,0xce,0x7c");
   for (i=0;i<9;i++){ LockLink_Serial.write(To_LL_13[i]);}   
   delay(150);
   digitalWrite(2, HIGH);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x01,0x7f,0xf9,0x00,0x02,0x85");
   for (i=0;i<8;i++){ LockLink_Serial.write(To_LL_14[i]);}   
   delay(150);
   digitalWrite(2, LOW);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x04,0x7f,0xfa,0x00,0x00,0x80,0x00,0x00,0x03");
   for (i=0;i<11;i++){ LockLink_Serial.write(To_LL_15[i]);}   
   delay(150);  
   digitalWrite(2, HIGH);
   
   DEBUG_PRINTLN("Sending 0xff,0x02,0x00,0x00,0x00,0x01,0xff");
   for (i=0;i<7;i++){ LockLink_Serial.write(To_LL_16[i]);}  
   delay(150);   
   digitalWrite(2, LOW);
   
   DEBUG_PRINTLN("OPEN THE DOOR");
}



void loop()
{ 
}

bool From_LL_Ack(void)
{
bool ack= false;
       if (LockLink_Serial.available())
          {
          if (LockLink_Serial.read()==0x06)ack=true;
          }
return ack;         
}


void Change_BR(void)
{
              LockLink_Serial.end();
              LockLink_Serial.flush();
              LockLink_Serial.begin(Baud_Rate_2);
}
