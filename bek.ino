#include "main.h"
bool aliveTimout = false;
 reciever av;
 fireBase fb;
 sim800L sim; 
 otaUpload ota; 
 ntpServerUtil util;
 blynk myBlynk;


void setup() 
{
   av.bluLed(ON);
   Serial.begin(115200);
   EEPROM.begin(EEPROM_SIZE);
   EEPROM.write(EEPROM_WIFI_ADD, 1); EEPROM.commit();wifiOn=true;
   EEPROM.write(EEPROM_FB_ADD, 1); EEPROM.commit();fireBaseOn=true;
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
       if (blynkOn)myBlynk.init();
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

    av.bluLed(OFF);
}


void loop(void) 
{
       resetWdg();    //reset timer (feed watchdog) 
       if (util.systemTimer(true, aliveTimer.prevMillis, 2)) aliveTimer.timeOut =!aliveTimer.timeOut; 
       av.bluLed(aliveTimer.timeOut);
       processCommands();
 //      wifiSurvilance();
}



void processCommands(void)
{
        if(blynkOn) 
        {
          myBlynk.blynkRun();
          if(blynkEvent = myBlynk.getData () )  processBlynk(); 
          myBlynk.sendAlive(aliveTimer.timeOut);
        }

        if (fireBaseOn)
        {
        if(fbEvent = fb.firebaseRun())          processFirebase();
   //     if(aliveSent != aliveTimer.timeOut) fb.SendString( FB_ALIVE, String(aliveTimer.timeOut) );
        }
        
        if(sim800Available) 
        {
          if( smsEvent =sim.smsRun()) processSms();
        }
       
        avOutput = av.Read_Analog_Av_Output(AV_OUTPUT_AN);    
        aliveSent != aliveTimer.timeOut;      
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
          else if (smsReceived       == "Settings" ) smsID = FB_SETTINGS_ID ;
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
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 30) remoteControl(remoteControlRcCh );
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


void processBlynk(void)
{
        switch (myBlynk.blynkEventID)
          {
            case FB_AV_7SEG_ID:
                recevierCh=myBlynk.blynkData;
                DEBUG_PRINT("FB_AV_7SEG: ");DEBUG_PRINTLN(myBlynk.blynkData);
                if (recevierCh >= 1 && recevierCh <= 8) receiverAvByCh (recevierCh);
            break;
            case FB_FREQ_ID:
              recevierFreq=myBlynk.blynkData;
              DEBUG_PRINT("FB_FREQ: ");DEBUG_PRINTLN(myBlynk.blynkData);
              if (recevierFreq >= 920 && recevierFreq <= 1500) receiverAvByFreq (recevierFreq);
            break;
            case FB_T433_CH_NR_ID:
              remoteControlRcCh=myBlynk.blynkData;
              DEBUG_PRINT("FB_T433_CH_NR: ");DEBUG_PRINTLN(myBlynk.blynkData);
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 15) remoteControl(remoteControlRcCh );
            break;
            case FB_T315_CH_NR_ID:
              remoteControlRcCh=myBlynk.blynkData;
              DEBUG_PRINT("FB_T315_CH_NR: ");DEBUG_PRINTLN( (myBlynk.blynkData) -15);
              if (remoteControlRcCh >= 16 && remoteControlRcCh <= 30) remoteControl(remoteControlRcCh );
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
              if (remoteControlRcCh >= 1 && remoteControlRcCh <= 30) remoteControl(remoteControlRcCh );
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



void remoteControl(int cmd )
{
     if (blynkOn)    myBlynk.blynkRCLed(1);
     if (fireBaseOn) fb.SendString (FB_RC_LED, "1" );
     av.rcPower(ON);  //RC Vcc Pin 2
     delay(200);
     mySwitch.send(CH_433[cmd], RC_CODE_LENGTH);
     DEBUG_PRINT("ch433:");DEBUG_PRINTLN(cmd);
     delay(200);
     av.rcPower(OFF);
     if (blynkOn)    
      {
        if (cmd >= 1 && cmd <= 15) {myBlynk.blynkRCLed(0);myBlynk.resetT433Cmd(cmd);}
        if (cmd >= 16 && cmd <= 30) {myBlynk.blynkRCLed315(0);myBlynk.resetT315Cmd(cmd);}
      }
     if (fireBaseOn) {fb.SendString (FB_RC_LED, "0" );fb.SendString (FB_AV_OUTPUT, String(avOutput) );}
}

void receiverAvByFreq (int Freq)
{
  bool ack=0;
       int PLL_value =( 512 * ( 1000000 * (Freq + 479.5) ) ) / (16*4000000) ;
       ack = av.Tuner_PLL(av_pll_addr, PLL_value);
       if (fireBaseOn) {fb.SendString (FB_ACK_LED, String(ack) ); fb.SendString (FB_AV_OUTPUT, String(avOutput) );}
       if (blynkOn)  { myBlynk.blynkAckLed(ack);myBlynk.sendRsss(avOutput);}
       myBlynk.frequencyValue(Freq );
       DEBUG_PRINT("Received manual_freq:");DEBUG_PRINTLN(manual_freq);
       DEBUG_PRINT("ack: ");DEBUG_PRINTLN(ack ? F("NotACK") : F("ACK"));
}

void receiverAvByCh (int Ch)
{
       bool ack = av.Tuner_PLL(av_pll_addr, PLL[Ch]);
       if (fireBaseOn) fb.SendString (FB_ACK_LED, String(ack) ); 
       if (blynkOn) myBlynk.blynkAckLed(ack);
       myBlynk.sevenSegValue(Ch );
       DEBUG_PRINT("Received freq channel:");DEBUG_PRINTLN(Ch);
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
//  if (blynkOn) myBlynk.notifierDebug(notifier_subject, notifier_body);
  if (fireBaseOn)fb.SendString( fb_path, fb_cmdString ); 
}

void wifiSurvilance(void)
{       
       if (wifiOn && ( ! (wifiAvailable=fb.wifiConnect() ) ) )   //if Wifi lost then activaate Sim and restart
       {
          sendToHMI("Wifi lost while on", "WIFI_LOST_ERR: ", "Wifi lost while on",FB_NOTIFIER, "Wifi lost while on" );
          EEPROM.write(EEPROM_FB_ADD, 0); EEPROM.commit();
          EEPROM.write(EEPROM_BLYNK_ADD, 0); EEPROM.commit();
          EEPROM.write(EEPROM_WIFI_ADD, 0); EEPROM.commit();
          EEPROM.write(EEPROM_ERR_ADD, WIFI_LOST_ERR); EEPROM.commit();
          EEPROM.write(EEPROM_SMS_ADD, 1); EEPROM.commit();smsOn =true;  
          sim800Available = smsOn && sim.init();
          if (sim800Available) {EEPROM.write(EEPROM_SIM800_ADD, 1); EEPROM.commit();}
          else {EEPROM.write(EEPROM_SIM800_ADD, 0); EEPROM.commit();smsOn =false;}
     //     ESP.restart();
       }
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
