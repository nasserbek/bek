#include "main.h"
#include <ESP32Ping.h>

#define VERSION_ID "Booting V1.2 11 10 2020 21.00"

#ifdef BEK
    #define NOTIFIER_ID "BEK : \n "
#else
    #define NOTIFIER_ID "BEK2 : \n "
#endif


#define LIVE_TIMER_ON   3000
#define LIVE_TIMER_OFF  3000
#define NETGEER_RESET_TIMER 36000000  // 10 HOURS
#define WIFI_SURVILANCE_TIMER 300000  // 5 MIN
#define PING_GOOLE_TIMER 300000  // 5 MIN
#define WIFI_IDE_TIMER 600000  //10 MIN

long timeout1, NetgeerResetTimer, wifiSurvilanceTimer, internetSurvilanceTimer, liveTimerOn,liveTimerOff,wifiIDETimer;
bool netgeerReset = false;
bool pingGoogle= false;
bool liveBit = false;
IPAddress ip (192, 168, 0, 1); // The remote ip to ping
bool aliveTimout = false;
bool RCsent= false;
int stateMachine =0;
bool wifiIde = true;
int repetionRC = 5;
int pulseRC = 320;
 reciever av;
 fireBase fb;
 sim800L sim; 
 otaUpload ota; 
 ntpServerUtil util;
 blynk myBlynk;


void setup() 
{
   pinMode(NETGEER_PIN, OUTPUT);
   digitalWrite(NETGEER_PIN, LOW);
   
   av.bluLed(ON);
   Serial.begin(115200);
   EEPROM.begin(EEPROM_SIZE);
   EEPROM.write(EEPROM_WIFI_ADD, 1); EEPROM.commit();wifiOn=true;
   EEPROM.write(EEPROM_FB_ADD, 0); EEPROM.commit();fireBaseOn=false;
   EEPROM.write(EEPROM_BLYNK_ADD, 1); EEPROM.commit();blynkOn=true;
   EEPROM.write(EEPROM_SMS_ADD, 1); EEPROM.commit();smsOn=true;
   
   initWDG(SEC_60,EN);
   av.init();
   
   sim800Available = smsOn && sim.init();
   if (sim800Available) {EEPROM.write(EEPROM_SIM800_ADD, 1); EEPROM.commit();}
   else {EEPROM.write(EEPROM_SIM800_ADD, 0); EEPROM.commit();}


     wifiAvailable= wifiOn && fb.wifiConnect();
     if (wifiAvailable) 
      { 
       byte gitHub = EEPROM.read(EEPROM_GITHUB_ADD);
       if (gitHub) {
        getDateTimeNTP(gitHub); 
        sendToHMI(util.dateAndTimeChar, "Version : ", String(util.dateAndTimeChar),FB_NOTIFIER,String(util.dateAndTimeChar));
        }
       if (blynkOn){myBlynk.init();myBlynk.frequencyValue(1080 );myBlynk.sevenSegValue(1 );getSettingsFromEeprom();myBlynk.notifierDebug(NOTIFIER_ID, VERSION_ID);}
       if (fireBaseOn) fb.init();
      }
     else  
     {
      if(sim800Available)
        {
          EEPROM.write(EEPROM_WIFI_ADD, 0); EEPROM.commit();wifiOn=false;
          EEPROM.write(EEPROM_FB_ADD, 0); EEPROM.commit();smsOn=false;
          EEPROM.write(EEPROM_BLYNK_ADD, 0); EEPROM.commit();blynkOn=false;

        }
      sendToHMI("Wifi failed to connect or turned off", "Wifi activation: ", "Wifi failed to connect or turned off",FB_NOTIFIER, "Wifi failed to connect or turned off" );

     }
    mySwitch.enableTransmit(RC_TX_PIN);
 //   mySwitch.setProtocol(1); 
    mySwitch.setPulseLength(pulseRC);
    repetionRC =EEPROM.read(RC_REPETION_ADD);
    if (repetionRC < 2) repetionRC = 2;
    DEBUG_PRINT("RC Repetion is : ");DEBUG_PRINTLN(repetionRC);
    mySwitch.setRepeatTransmit(repetionRC);
 //   av.rcPower(ON);  //RC Vcc Pin 2 to be removed

    
    av.bluLed(OFF);
    NetgeerResetTimer       = millis();
    wifiSurvilanceTimer     = millis();
    internetSurvilanceTimer = millis();
    liveTimerOff            = millis();
    liveTimerOn             = millis();
    wifiIDETimer            = millis();
    
    otaIdeSetup();
}


void loop(void) 
{
       resetWdg();    //reset timer (feed watchdog) 
       liveCtrl();
       processCommands();
       netgeerCtrl();
       
       while (!wifiIde) 
       {
        enableWDG(false);
        if (  millis() - wifiIDETimer > WIFI_IDE_TIMER )
        {
           wifiIde = true;
           resetWdg();
           enableWDG(true);
           wifiIDETimer = millis();
           ESP.restart();
        }
        else ArduinoOTA.handle();
       }
}



void processCommands(void)
{
        if(blynkOn) 
        {
          myBlynk.blynkRun();
          if(blynkEvent = myBlynk.getData () )  processBlynk(); 
          if (zapOnOff ) zappingAvCh (zapOnOff, zapTimer , zapCh1, zapCh2, zapCh3,zapCh4, zapCh5, zapCh6, zapCh7, zapCh8);
        }

        if (fireBaseOn)
        {
        if(fbEvent = fb.firebaseRun())          processFirebase();
        }
        
        if(sim800Available) 
        {
          if( smsEvent =sim.smsRun()) processSms();
        }

       
        avOutput = av.Read_Analog_Av_Output(AV_OUTPUT_AN);    
}



void processBlynk(void)
{
        switch (myBlynk.blynkEventID)
          {
            case FB_AV_7SEG_ID:
                recevierCh=myBlynk.blynkData;
                DEBUG_PRINT("FB_AV_7SEG: ");DEBUG_PRINTLN(myBlynk.blynkData);
                if (recevierCh >= 1 && recevierCh <= 9) receiverAvByCh (recevierCh);
            break;
            case FB_FREQ_ID:
              recevierFreq=myBlynk.blynkData;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(myBlynk.blynkData);
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_T433_CH_NR_ID:
              remoteControlRcCh=myBlynk.blynkData;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(myBlynk.blynkData);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 15) {remoteControl(remoteControlRcCh );RCsent = false;}
            break;
            case FB_T315_CH_NR_ID:
              remoteControlRcCh=myBlynk.blynkData;
              DEBUG_PRINT("FB_T315_CH_NR: ");DEBUG_PRINTLN( (myBlynk.blynkData) -15);
              if (remoteControlRcCh >= 16 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );RCsent = false;}
            break;
 
            case FB_RESET_ID:
              rebootCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(myBlynk.blynkData);
              rebootSw();
            break;
            case FB_OTA_ID:
              otaCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(myBlynk.blynkData);
              fb.endTheOpenStream();
              otaGsm ();
            break;
            case FB_SMS_ON_ID:
              smsOnOffCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_SMS_ON: ");DEBUG_PRINTLN(myBlynk.blynkData);
              smsActivation(smsOnOffCmd);
              myBlynk.blynkSmsLed(sim800Available);
            break;
            case FB_VERSION_ID:
              verCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_VERSION: ");DEBUG_PRINTLN(myBlynk.blynkData);
              sendVersion();
            break;
            case FB_FB_OFF_ID:
              firebaseOnOffCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_FB_OFF: ");DEBUG_PRINTLN(myBlynk.blynkData);
              firebaseOnActivation(firebaseOnOffCmd);
              myBlynk.blynkFirebaseLed(fireBaseOn);
            break;
            case FB_BLYNK_ON_OFF_ID:
              blynkOnOffCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_BLYNK_ON_OFF: ");DEBUG_PRINTLN(myBlynk.blynkData);
              blynkActivation(0);
            break;
            case FB_WIFI_OFF_ID:
              wifiOnOffCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_WIFI_OFF: ");DEBUG_PRINTLN(myBlynk.blynkData);
              wifiActivation(0);
            break;
            case FB_SETTINGS_ID :
              getSettingsFromEeprom();
              sendToHMI(smsSettings, "Setting ", smsSettings ,FB_NOTIFIER,smsSettings);
            break;

            case FB_ZAP_ID:
              zapOnOff=myBlynk.blynkData;
              DEBUG_PRINT("ZAP IS : ");
              DEBUG_PRINTLN(zapOnOff ? F("On") : F("Off"));
              myBlynk.zapLed(zapOnOff);
            break;

            case FB_ZAP_TIMER_ID :
              zapTimer=myBlynk.blynkData;
            break;

            case FB_ZAP_CHANNEL_ID1 :
              zapCh1=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID2 :
              zapCh2=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID3 :
              zapCh3=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID4 :
              zapCh4=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID5 :
              zapCh5=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID6 :
              zapCh6=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID7 :
              zapCh7=myBlynk.blynkData;
            break;

             case FB_ZAP_CHANNEL_ID8 :
              zapCh8=myBlynk.blynkData;
            break;
             case FB_NETGEER_ID  :
             myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset from Blynk");
              ResetNetgeer();
            break;

            case FB_WIFI_IDE_ID:
                wifiIde = false;
                wifiIDETimer = millis();
            break;

            case FB_AV_CH_PLUS_ID:
                recevierCh += 1;
                if (recevierCh >= 1 && recevierCh <= 9) receiverAvByCh (recevierCh);
            break;
            case FB_AV_CH_MINUS_ID:
                recevierCh -= 1;
                if (recevierCh >= 1 && recevierCh <= 9) receiverAvByCh (recevierCh);
            break;
            case FB_AV_FR_PLUS_ID:
              recevierFreq += 1;
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_AV_FR_MINUS_ID:
              recevierFreq -= 1;
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_RC_REPETION_ID:
             repetionRC=myBlynk.blynkData;
             EEPROM.write(RC_REPETION_ADD, repetionRC); EEPROM.commit();
             mySwitch.setRepeatTransmit(repetionRC);
            break;

             case FB_RC_PULSE_ID:
             pulseRC=myBlynk.blynkData;
        //     EEPROM.write(RC_REPETION_ADD, repetionRC); EEPROM.commit();
             mySwitch.setPulseLength(pulseRC);
            break;
    }  
}


void remoteControl(int cmd )
{
 if(RCsent == false);
 {
     if (blynkOn)    
      {
        if (cmd >= 1 && cmd <= 15) {myBlynk.blynkRCLed(1); }
        if (cmd >= 16 && cmd <= 30) {myBlynk.blynkRCLed315(1);}
      }

     if (fireBaseOn) fb.SendString (FB_RC_LED, "1" );
 //    av.rcPower(ON);  //RC Vcc Pin 2
  //   delay(500);
     mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);
     DEBUG_PRINT("ch433:");DEBUG_PRINTLN(cmd);
     //delay(500);
  //   av.rcPower(OFF);
     if (blynkOn)    
      {
        if (cmd >= 1 && cmd <= 15) {myBlynk.blynkRCLed(0);myBlynk.resetT433Cmd(cmd);}
        if (cmd >= 16 && cmd <= 30) {myBlynk.blynkRCLed315(0);myBlynk.resetT315Cmd(cmd);}
      }
     if (fireBaseOn) {fb.SendString (FB_RC_LED, "0" );fb.SendString (FB_AV_OUTPUT, String(avOutput) );}
     RCsent = true;
 }
}


void zappingAvCh (bool zapCmd, int zapTimer, bool ch1, bool ch2, bool ch3,bool ch4, bool ch5, bool ch6, bool ch7, bool ch8)
{
         switch (stateMachine)
          {
            case 0:
            case 1:
                if (ch1 ) 
                  {
                    if (stateMachine == 0) {timeout1= millis();stateMachine =1;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (1);stateMachine =2;}
                  }
                else stateMachine =2;
            break;

            case 2:
            case 3:
                if (ch2 ) 
                  {
                    if (stateMachine == 2) {timeout1= millis();stateMachine =3;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (2);stateMachine =4;}
                  }
                else stateMachine =4;
            break;
            
            case 4:
            case 5:
                if (ch3 ) 
                  {
                    if (stateMachine == 4) {timeout1= millis();stateMachine =5;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (3);stateMachine =6;}
                  }
                else stateMachine =6;
            break;

            case 6:
            case 7:
                if (ch4 ) 
                  {
                    if (stateMachine == 6) {timeout1= millis();stateMachine =7;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (4);stateMachine =8;}
                  }
                else stateMachine =8;
            break;
                       
            case 8:
            case 9:
                if (ch5 ) 
                  {
                    if (stateMachine == 8) {timeout1= millis();stateMachine =9;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (5);stateMachine =10;}
                  }
                else stateMachine =10;
            break;
                       
            case 10:
            case 11:
                if (ch6 ) 
                  {
                    if (stateMachine == 10) {timeout1= millis();stateMachine =11;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (6);stateMachine =12;}
                  }
                else stateMachine =12;
            break;
                       
            case 12:
            case 13:
                if (ch7 ) 
                  {
                    if (stateMachine == 12) {timeout1= millis();stateMachine =13;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (7);stateMachine =14;}
                  }
                else stateMachine =14;
            break;
                       
            case 14:
            case 15:
                if (ch8) 
                  {
                    if (stateMachine == 14) {timeout1= millis();stateMachine =15;}
                    if (millis() - timeout1 > zapTimer) {receiverAvByCh (8);stateMachine =0;}
                  }
                else stateMachine =0;
            break;
          }

}

        
void receiverAvByCh (int Ch)
{
  bool ack;
  int PLL_value;
      myBlynk.blynkAckLed(true);
       if (Ch != 9) {ack = av.Tuner_PLL(av_pll_addr, PLL[Ch]);}
       else 
        {
          PLL_value =( 512 * ( 1000000 * (990 + 479.5) ) ) / (16*4000000) ;
          ack = av.Tuner_PLL(av_pll_addr, PLL_value);
        }
       if (fireBaseOn) fb.SendString (FB_ACK_LED, String(ack) ); 
       if (blynkOn) myBlynk.blynkAckLed(ack);
       myBlynk.sevenSegValue(Ch );
        switch (Ch)
          {
            case 1:
                myBlynk.frequencyValue(1080 );recevierFreq =1080;
            break;

            case 2:
                myBlynk.frequencyValue(1120 );recevierFreq =1120;
            break;

            
            case 3:
                myBlynk.frequencyValue(1160 );recevierFreq =1160;
            break;


            case 4:
                myBlynk.frequencyValue(1200 );recevierFreq =1200;
            break;

                       
            case 5:
                myBlynk.frequencyValue(1240 );recevierFreq =1240;
            break;

                       
            case 6:
                myBlynk.frequencyValue(1280 );recevierFreq =1280;
            break;

                       
            case 7:
                myBlynk.frequencyValue(1320 );recevierFreq =1320;
            break;

                       
            case 8:
                myBlynk.frequencyValue(1360 );recevierFreq =1360;
            break;

          }
       DEBUG_PRINT("Received freq channel:");DEBUG_PRINTLN(Ch);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}



void receiverAvByFreq (int Freq)
{
  bool ack=0;
         recevierFreq =Freq;
        myBlynk.blynkAckLed(true);
       int PLL_value =( 512 * ( 1000000 * (Freq + 479.5) ) ) / (16*4000000) ;
       ack = av.Tuner_PLL(av_pll_addr, PLL_value);
       if (fireBaseOn) {fb.SendString (FB_ACK_LED, String(ack) ); fb.SendString (FB_AV_OUTPUT, String(avOutput) );}
       if (blynkOn)  { myBlynk.blynkAckLed(ack);/*myBlynk.sendRsss(avOutput);*/}
       myBlynk.frequencyValue(Freq );
       DEBUG_PRINT("Received manual_freq:");DEBUG_PRINTLN(manual_freq);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}


void getDateTimeNTP(bool ver)
{
   if (ver)util.printLocalTime(true); 
   else util.sendDateTime(false);
}    



void IRAM_ATTR resetModule() 
{
EEPROM.write(EEPROM_ERR_ADD, WDG_ERR); EEPROM.commit();
DEBUG_PRINTLN("Watch Dog Timer Timout, rebooting....");
ESP.restart();
}

void initWDG(int wdtTimeout,bool _enable) 
{
   _timer = timerBegin(0, 80, true);                                    //timer 0, div 80
   timerAttachInterrupt(_timer, &resetModule, true);                  //attach callback
   timerAlarmWrite(_timer, wdtTimeout * 1000, false);              //set time in us
   enableWDG(_enable);                                            //enable interrupt
}

void enableWDG(bool _enable)
  {
   if (_enable) timerAlarmEnable(_timer);                          //enable interrupt
   else timerAlarmDisable(_timer);                                //Disable interrupt
   resetWdg();                                        //reset timer (feed watchdog)  
  }
  
void resetWdg(void)
  {
   timerWrite(_timer, 0);                                        //reset timer (feed watchdog)  
  }

void  getSettingsFromEeprom(void)
{
 
   smsOn = EEPROM.read(EEPROM_SMS_ADD);DEBUG_PRINT("Sms is:");DEBUG_PRINTLN(smsOn ? F("On") : F("Off"));
   if (smsOn) {smsSettings [48] ='O';smsSettings [49] ='n';smsSettings [50] =' '; }
   else {smsSettings [48] ='O';smsSettings [49] ='f';smsSettings [50] ='f'; }

   wifiOn=EEPROM.read(EEPROM_WIFI_ADD);DEBUG_PRINT("Wifi is:");DEBUG_PRINTLN(wifiOn ? F("On") : F("Off"));
   if(wifiOn) {smsSettings [8] ='O';smsSettings [9] ='n';smsSettings [10] =' '; }
   else {
          smsSettings [8] ='O';smsSettings [9] ='f';smsSettings [10] ='f';        
          EEPROM.write(EEPROM_FB_ADD, 0); EEPROM.commit();
          EEPROM.write(EEPROM_BLYNK_ADD, 0); EEPROM.commit();
        }

   fireBaseOn=EEPROM.read(EEPROM_FB_ADD);DEBUG_PRINT("FireBase is:");DEBUG_PRINTLN(fireBaseOn? F("On") : F("Off"));
   if (fireBaseOn) {smsSettings [24] ='O';smsSettings [25] ='n';smsSettings [26] =' '; } 
   else {smsSettings [24] ='O';smsSettings [25] ='f';smsSettings [26] ='f'; } 
   
   blynkOn=EEPROM.read(EEPROM_BLYNK_ADD);DEBUG_PRINT("Blynk is:");DEBUG_PRINTLN(blynkOn ? F("On") : F("Off"));
   if (blynkOn) {smsSettings [37] ='O';smsSettings [38] ='n';smsSettings [39] =' '; }
   else {smsSettings [37] ='O';smsSettings [38] ='f';smsSettings [39] ='f'; }

 
   errorCode = EEPROM.read(EEPROM_ERR_ADD);DEBUG_PRINT("Error code is:");DEBUG_PRINTLN(char (errorCode));
   smsSettings [66] =errorCode;

   sim800Available = EEPROM.read(EEPROM_SIM800_ADD);DEBUG_PRINT("SIM800L is:");DEBUG_PRINTLN(sim800Available ? F("On") : F("Off"));
   if (sim800Available) {smsSettings [80] ='O';smsSettings [81] ='k';smsSettings [82] =' '; }
   else {smsSettings [80] ='O';smsSettings [81] ='f';smsSettings [82] ='f'; }
   EEPROM.write(EEPROM_ERR_ADD, '0'); EEPROM.commit();
}

void sendToHMI(char *smsmsg, String notifier_subject, String notifier_body,String fb_path,String fb_cmdString)
{
  if(sim800Available)sim.SendSMS(smsmsg);
  if (blynkOn) myBlynk.notifierDebug(NOTIFIER_ID, notifier_body);
  if (fireBaseOn)fb.SendString( fb_path, fb_cmdString ); 
}

 

void otaGsm(void)
{
  DEBUG_PRINTLN("Firmware Upload..."); 
  enableWDG(DIS);
  ota.init(true);
  ota.startOtaUpdate(overTheAirURL);
}

void rebootSw(void)
{
 EEPROM.write(EEPROM_ERR_ADD, SW_RESET); EEPROM.commit();
 ESP.restart();
}

void smsActivation(int activate)
{
  if(activate) {sendToHMI("Sms is On", "Sms activation: ", "Sms is On",FB_NOTIFIER, "Sms is On" );
   DEBUG_PRINTLN("Sms is On");
   EEPROM.write(EEPROM_SMS_ADD, 1); EEPROM.commit();
   }
  if(!activate) {sendToHMI("Sms is Off", "Sms disactivation: ", "Sms is Off",FB_NOTIFIER, "Sms is Off" );
   DEBUG_PRINTLN("Sms is Off");
   EEPROM.write(EEPROM_SMS_ADD, 0); EEPROM.commit();smsOn =false;
   sim.sim800PowerOn(false)  ;
   }  
}

void firebaseOnActivation(int activation)
{
 if (wifiAvailable)
  {
    if (activation)
    {
    fb.init();
    sendToHMI("FireBase is On", "Firebase activation: ", "FireBase is On",FB_NOTIFIER, "FireBase is On" );
    DEBUG_PRINTLN("FireBase is On");
    EEPROM.write(EEPROM_FB_ADD, 1); EEPROM.commit();fireBaseOn =true;
    }
    else
    {
    sendToHMI("FireBase is Off", "Firebase disactivation: ", "FireBase is Off",FB_NOTIFIER, "FireBase is Offn" );
    DEBUG_PRINTLN("FireBase is Off");
    EEPROM.write(EEPROM_FB_ADD, 0); EEPROM.commit();fireBaseOn =false;   
    fb.endTheOpenStream(); 
    }
  } 
  else 
   {
     sendToHMI("Wifi not active..", "Firebase activation: ", "Wifi not active..",FB_NOTIFIER, "Wifi not active.." );
     fireBaseOn =false;EEPROM.write(EEPROM_FB_ADD, 0); EEPROM.commit();
    }
}  


void wifiActivation(int activation)
{
       if(activation)
       {
       wifiAvailable=fb.wifiConnect();
       if (wifiAvailable) 
         {
           sendToHMI("Wifi is On", "Wifi activation: ", "Wifi is On",FB_NOTIFIER, "Wifi is On" );
           DEBUG_PRINTLN("Wifi is On");
           EEPROM.write(EEPROM_WIFI_ADD, 1); EEPROM.commit();wifiOn = true;
         }
       else 
        {
          wifiOn = false;
          sendToHMI("Wifi not available", "Wifi activation error: ", "Wifi not available",FB_NOTIFIER, "Wifi not available" );
          EEPROM.write(EEPROM_WIFI_ADD, 0); EEPROM.commit();
         }
       }

       else
         {
       wifiOn = false;
       sendToHMI("Wifi is Off", "Wifi disactivation: ", "Wifi is Off",FB_NOTIFIER, "Wifi is Off" );
       DEBUG_PRINTLN("Wifi is Off");
       EEPROM.write(EEPROM_WIFI_ADD, 0); EEPROM.commit();
       EEPROM.write(EEPROM_ERR_ADD, WIFI_OFF); EEPROM.commit();
       ESP.restart();
        }
}

void blynkActivation (int activation)
{
      if (activation) // Blon
      { 
       if (wifiAvailable) 
         {
           myBlynk.init();
           sendToHMI("Blynk is On", "Blynk activation: ", "Blynk is On",FB_NOTIFIER, "Blynk is On");
           DEBUG_PRINTLN("Blynk is On");
           EEPROM.write(EEPROM_BLYNK_ADD, 1); EEPROM.commit();blynkOn = true;
         }
       else {
       sendToHMI("Wifi not active..", "Blynk activation: ", "Wifi not active..",FB_NOTIFIER, "Wifi not active.." );
       blynkOn = false;EEPROM.write(EEPROM_BLYNK_ADD, 0); EEPROM.commit();
       }
      }    

      else 
      { 
       blynkOn = false;
       sendToHMI("Blynke is Off", "Blynk disactivation: ", "Blynk is Off",FB_NOTIFIER, "Blynk is Off" );
       DEBUG_PRINTLN("Blynk is Off");
       EEPROM.write(EEPROM_BLYNK_ADD, 0); EEPROM.commit();
      }    
}

void sendVersion(void)
{
 util.printLocalTime(false); 
 sendToHMI(util.dateAndTimeChar, "Version : ", String(util.dateAndTimeChar),FB_NOTIFIER, String(util.dateAndTimeChar) );
}

int stringToInteger(String str)
{
char carray[5]; 
      str.toCharArray(carray, sizeof(carray));
      return ( atoi(carray));  
}

void ResetNetgeer(void)
          {
              delay(2000);
              digitalWrite(NETGEER_PIN, HIGH);
              delay(2000);
              digitalWrite(NETGEER_PIN, LOW); 
              DEBUG_PRINTLN("Netgeer Reset done: ");
          }


void internetSurvilance(void)
{       
 bool internetActive  = checkInternetConnection();
 if (!internetActive)  {DEBUG_PRINTLN("Internet Failure: ");  myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset Internet Failure");ResetNetgeer();}
} 


bool checkInternetConnection(void)
{
       bool pingInternet= Ping.ping("www.google.com");
       DEBUG_PRINT("Ping Google: ");DEBUG_PRINTLN(pingInternet ? F("succesiful") : F("failed"));
       return pingInternet;
}

void netgeerCtrl(void)
{
       if (   millis() - internetSurvilanceTimer > PING_GOOLE_TIMER)  {internetSurvilance();internetSurvilanceTimer= millis();}
       if (   (millis() - wifiSurvilanceTimer > WIFI_SURVILANCE_TIMER)  && (!wifiAvailable)  ) {wifiSurvilanceTimer= millis();DEBUG_PRINTLN("Wifi Failure: ");myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset Wifi Failure");ResetNetgeer();}
       if (millis() - NetgeerResetTimer > NETGEER_RESET_TIMER) {myBlynk.notifierDebug(NOTIFIER_ID, "Netgeer Reset 10 hours timer");NetgeerResetTimer= millis();DEBUG_PRINTLN("10 hours timer: ");ResetNetgeer();}
}     


void otaIdeSetup (void)
     {
        ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  wifiIde = true;
 }
 
void liveCtrl(void)
{
   if ( (millis() - liveTimerOn > LIVE_TIMER_ON) && !liveBit ) 
          {
            liveBit = true ;
            av.bluLed(liveBit);
            liveTimerOff = millis();
//            DEBUG_PRINT("Live : ");
  //          DEBUG_PRINTLN(liveBit ? F("On") : F("Off"));
            myBlynk.sendAlive(liveBit);
          }
    if ( (millis() - liveTimerOff > LIVE_TIMER_OFF) && liveBit ) 
          {
            liveBit = false ;
            av.bluLed(liveBit);
            liveTimerOn = millis();
      //      DEBUG_PRINT("Live : ");
    //        DEBUG_PRINTLN(liveBit ? F("On") : F("Off"));
            myBlynk.sendAlive(liveBit);
          }
}

void processSms(void)
{
      boolean isValidNumber =false;
      int smsID=0;
      
      smsReceived =  sim.smsString;
      
       for(byte i=0;i<smsReceived.length();i++)
          {
            if(isDigit(smsReceived.charAt(i))) isValidNumber =true;
            else isValidNumber=false;
          }
        if(isValidNumber)
        {
          smsValue = stringToInteger(smsReceived);
          if (smsValue >= 1 && smsValue <= 30)     smsID =FB_T433_CH_NR_ID;
          if (smsValue >= 41 && smsValue <= 48)    smsID =FB_AV_7SEG_ID;
          if (smsValue >= 900 && smsValue <= 1360) smsID =FB_FREQ_ID;
        }
        else
        {
          if      (smsReceived =="Reset")smsID = FB_RESET_ID;
          else if (smsReceived =="Firebase") smsID = FB_FB_OFF_ID;
          else if (smsReceived =="Blynk") smsID = FB_BLYNK_ON_OFF_ID;
          else if (smsReceived =="Wifi") smsID = FB_WIFI_OFF_ID;
          else if (smsReceived =="Ota") smsID = FB_OTA_ID;
          else if (smsReceived =="Sms") smsID = FB_SMS_ON_ID;
          else if (smsReceived =="Ver") smsID =FB_VERSION_ID;
          else if (smsReceived == "Settings" ) smsID = FB_SETTINGS_ID ;
          else if (smsReceived == "Netgeer" ) ResetNetgeer();
        }
        switch (smsID)
          {
            case FB_AV_7SEG_ID:
                recevierCh=smsValue-40;
                DEBUG_PRINT("FB_AV_7SEG: ");DEBUG_PRINTLN(recevierCh);
                if (recevierCh >= 1 && recevierCh <= 8) receiverAvByCh (recevierCh);
            break;
            case FB_FREQ_ID:
              recevierFreq=smsValue;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(recevierFreq);
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_T433_CH_NR_ID:
              remoteControlRcCh=smsValue;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(remoteControlRcCh);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );RCsent = false;}
            break;
            case FB_RESET_ID:
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(smsReceived);
              rebootSw();
            break;
            case FB_OTA_ID:
              otaCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(smsReceived);
              fb.endTheOpenStream();
              otaGsm ();
            break;
            case FB_SMS_ON_ID:
              smsOnOffCmd=myBlynk.blynkData;
              DEBUG_PRINT("FB_SMS_ON: ");DEBUG_PRINTLN(smsReceived);
              smsActivation(!smsOn);
              myBlynk.blynkSmsLed(sim800Available);
            break;
            case FB_VERSION_ID:
              DEBUG_PRINT("FB_VERSION: ");DEBUG_PRINTLN(smsReceived);
              sendVersion();
            break;
            case FB_FB_OFF_ID:
              DEBUG_PRINT("FB_FB_OFF: ");DEBUG_PRINTLN(smsReceived);
              firebaseOnActivation(!fireBaseOn);
              myBlynk.blynkFirebaseLed(fireBaseOn);
            break;
            case FB_BLYNK_ON_OFF_ID:
              DEBUG_PRINT("FB_BLYNK_ON_OFF: ");DEBUG_PRINTLN(smsReceived);
              blynkActivation(!blynkOn);
            break;
            case FB_WIFI_OFF_ID:
              DEBUG_PRINT("FB_WIFI_OFF: ");DEBUG_PRINTLN(myBlynk.blynkData);
              wifiActivation(!wifiOn);
            break;
            case FB_SETTINGS_ID :
              getSettingsFromEeprom();
              sendToHMI(smsSettings, "Setting ", smsSettings ,FB_NOTIFIER,smsSettings);
            break;
    }  
}


void processFirebase(void)
{
        switch (fb.eventID)
          {
            case FB_AV_7SEG_ID:
                recevierCh=fb.eventValue;
                DEBUG_PRINT("FB_AV_7SEG: ");DEBUG_PRINTLN(fb.eventValue);
                if (recevierCh >= 1 && recevierCh <= 8) receiverAvByCh (recevierCh);
            break;
            case FB_FREQ_ID:
              recevierFreq=fb.eventValue;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(fb.eventValue);
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_T433_CH_NR_ID:
              remoteControlRcCh=fb.eventValue;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(remoteControlRcCh);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 30) {remoteControl(remoteControlRcCh );RCsent = false;}
            break;
            case FB_RESET_ID:
              rebootCmd=fb.eventValue;
              DEBUG_PRINT("FB_RESET: ");DEBUG_PRINTLN(fb.eventValue);
              if(rebootCmd) rebootSw();
            break;
            case FB_OTA_ID:
              otaCmd=fb.eventValue;
              DEBUG_PRINT("FB_OTA: ");DEBUG_PRINTLN(fb.eventValue);
              if(otaCmd){fb.endTheOpenStream();  otaGsm ();}
            break;
            case FB_SMS_ON_ID:
              smsOnOffCmd=fb.eventValue;
              DEBUG_PRINT("FB_SMS_ON: ");DEBUG_PRINTLN(fb.eventValue);
              smsActivation(smsOnOffCmd);
              myBlynk.blynkSmsLed(sim800Available);
            break;
            case FB_VERSION_ID:
              verCmd=fb.eventValue;
              DEBUG_PRINT("FB_VERSION: ");DEBUG_PRINTLN(fb.eventValue);
              sendVersion();
            break;
            case FB_FB_OFF_ID:
              firebaseOnOffCmd=fb.eventValue;
              DEBUG_PRINT("FB_FB_OFF: ");DEBUG_PRINTLN(fb.eventValue);
              firebaseOnActivation(firebaseOnOffCmd);
              myBlynk.blynkFirebaseLed(fireBaseOn);
            break;
            case FB_BLYNK_ON_OFF_ID:
              blynkOnOffCmd=fb.eventValue;
              DEBUG_PRINT("FB_BLYNK_ON_OFF: ");DEBUG_PRINTLN(fb.eventValue);
            break;
            case FB_WIFI_OFF_ID:
              wifiOnOffCmd=fb.eventValue;
              DEBUG_PRINT("FB_WIFI_OFF: ");DEBUG_PRINTLN(fb.eventValue);
            break;
            case FB_SETTINGS_ID :
              getSettingsFromEeprom();
              sendToHMI(smsSettings, "Setting ", smsSettings ,FB_NOTIFIER,smsSettings);
            break;            
    }
}    
