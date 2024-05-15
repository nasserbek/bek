// aws.h

#ifndef AWS_H
#define AWS_H

extern blynk myBlynk;
extern void dvrOnOff (bool cmd);
void callback(char* topic, byte* payload, unsigned int length);
StaticJsonDocument<54> rxDoc; //Json to receive in

void awsTerminal(bool aws, String str)
{
  StaticJsonDocument<54> doc; //Json to send from
  if(aws)
    {
     doc["TERMINAL"] = str;
     serializeJson(doc, Json); // print to client
     client.publish(AWS_IOT_SUBSCRIBE_TOPIC_TERMINAL, Json);
    }      
}


/*************************************************NODE RED AWS IOT ZONE********************************************************************************************/

bool connectAWS()
{
  StaticJsonDocument<54> doc2; //Json to send from
  
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  //client.setCallback(messageHandler); 
  client.setCallback(callback);

 
  myBlynk.TerminalPrint("Connecting Client to AWS IOT");
  
  
  while (!client.connect(THINGNAME))
  {
    Serial.println("Connecting to AWS:");
    Serial.println( THINGNAME );
    Serial.print(".");
    delay(1000);
    return false;
  }
   if (!client.connected())
  {
    myBlynk.TerminalPrint("AWS IoT Timeout!");
    return false;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_VIDEO);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAP);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_RX);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_AV_RC);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_DVR);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_REBOOT);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_SCAN);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_REPEAT);  
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_PRESET); 
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO);  
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMERON); 
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPTIMEROFF);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_RC);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_ZAPCH);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA);
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA);    
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA); 
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC_LIVE);
  
  doc2["version"] = VERSION_ID;
  serializeJson(doc2, Json); // print to client
  client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VERSION , Json);
  myBlynk.TerminalPrint("AWS IoT Connected!");
  
  String str = WiFi.SSID() + " " + "IP:" + WiFi.localIP().toString();
  awsTerminal(awsConnected, str ) ;
  
  return true;
}



void retriveDataFromTopic (char* topic, byte* payload, unsigned int length )
{    
        for (int i=0;i<length;i++) //Converts the received message to String
        {      
          resultS= resultS + (char)payload[i];
        }
               
        DeserializationError error = deserializeJson(rxDoc, resultS); //Command to derealize the received Json
        if (error) 
        {
          myBlynk.TerminalPrint(F("deserializeJson() failed with code "));
          myBlynk.TerminalPrint(F("deserializeJson() failed with code "));
        } 
        _nodeRedEvent = true; 
        hmi = NODE_RED; 
}

bool getDataNodeRed(void)
{
    if (_nodeRedEvent)
    {
      nodeRedData=_nodeRedData;
      nodeRedEventID =_nodeRedEventID;
      _nodeRedEvent = false;
      nodeRedActive = true; 
      return true;
    }  
    else return false;
}

int getChID(int ch)
{
        switch (ch)
        {
          case 1:
                return Q_EVENT_RM_ID_10_V112;
          break;

          case 2:
                    return Q_EVENT_RM_ID_02_V122;
          break;
          case 3:
                    return Q_EVENT_RM_ID_03_V123;
          break;
          case 4:
                    return Q_EVENT_RM_ID_04_V124;
          break;
          case 5:
                    return Q_EVENT_RM_ID_05_V125;
          break;
          case 6:
                    return Q_EVENT_RM_ID_06_V126;
          break;
          case 7:
                   return Q_EVENT_RM_ID_07_V127;
          break;
          case 8:
                    return Q_EVENT_RM_ID_08_V93;
          break;
          case 9:
                    return Q_EVENT_RM_ID_09_V80;
          break;
          case 10:
                    return Q_EVENT_RM_ID_10_V21;
          break;
          case 11:
                   return Q_EVENT_RM_ID_11_V14;
          break;
          case 12:
                    return Q_EVENT_RM_ID_12_V15;
          break;
          case 13:
                    return Q_EVENT_RM_ID_13_V23;
          break;
          case 14:
                    return Q_EVENT_RM_ID_14_V103;
          break;
          case 15:
                    return Q_EVENT_RM_ID_15_V104;
          break;
          case 16:
                    return Q_EVENT_RM_ID_16_V105;
          break;
          case 17:
                    return Q_EVENT_RM_ID_17_V90;
          break;
          case 18:
                   return Q_EVENT_RM_ID_18_V91;
          break;
          case 19:
                    return Q_EVENT_RM_ID_19_V92;
          break;       
          default:
          return Q_EVENT_RM_ID_10_V112;            
        }  
 return 1;      
}     


void callback(char* topic, byte* payload, unsigned int length) {
  resultS = "";   //Empty variable from serialized Json
  
  
  if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_VIDEO)
    {// "VIDEO": 3, "CMD":1
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["CMD"];; 
        nodeRedeventdata =getChID (rxDoc["VIDEO"]);
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
     }

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_ZAP)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["ZAP"];
        nodeRedeventdata = Q_EVENT_ZAP_V71;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapStatus(_nodeRedData);
    }

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_RX)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["RX"];
        nodeRedeventdata = Q_EVENT_SELECTED_RECIEVER_V9;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.RelaySelect(_nodeRedData);
        
    }   
    
  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_AV_RC)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["AVRC"];
        nodeRedeventdata = Q_EVENT_ROOM_AV_RC_V19;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.sendAvRxIndex(_nodeRedData);
      }   

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_DVR)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["DVR"];
        nodeRedeventdata = Q_EVENT_VIDEO_ON_OFF_V81;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        dvrOnOff(_nodeRedData); 

    }   

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_REBOOT)
    {
        myBlynk.TerminalPrint(" Received topic is: " + String(topic) +"Rebooting ESP");
        ESP.restart();
    }  


else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_SCAN)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["SCAN"];
        nodeRedeventdata = Q_SCAN_ACTIVE_CH_V4;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.resetSetupAndScan(_nodeRedData); 
     }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_REPEAT)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["REPEAT"];
        nodeRedeventdata = Q_EVENT_REPEAT_V3;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.resetSetupAndScan(_nodeRedData); 
     }
     
else if(String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_PRESET)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["PRESET"];
        nodeRedeventdata = Q_EVENT_RESET_FREQ_V26;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        
    }
    
else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["ZAPAUTO"];
        nodeRedeventdata = Q_EVENT_AUTOMATIC_RC_L_R_V5;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapAutoLocalRC(_nodeRedData); 
    }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_RC)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["RC"];
        nodeRedeventdata = Q_EVENT_RC_CH_NR_V1;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
         RC_Remote_CSR1=false; 
         RC_Remote_CSR2=false; 
         RC_Remote_CSR2=false; 
    }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["OTAWEB"];
        nodeRedeventdata = Q_EVENT_OTA_LOCAL_WEB_WIFI_V6;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
    }    

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["OTAGITHUB"];
        nodeRedeventdata = Q_EVENT_OTA_GITHUB_V7;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
    }       

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = rxDoc["OTAIDE"];
        nodeRedeventdata = Q_EVENT_WIFI_IDE_V11;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
    } 

    else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_LIVE)
    {
      retriveDataFromTopic(topic, payload,length);
      
      StaticJsonDocument<54> doc; 
      doc["LIVE"] = LiveSec;
      serializeJson(doc, Json); 
      #ifdef CSR    
          client.publish(AWS_IOT_PUBLISH_TOPIC_LIVE_1, Json);
       #endif 
       
       #ifdef CSR2    
          client.publish(AWS_IOT_PUBLISH_TOPIC_LIVE_2, Json);
       #endif 
       
       #ifdef CSR3  
          client.publish(AWS_IOT_PUBLISH_TOPIC_LIVE_3, Json);
       #endif

       #ifdef TEST 
          client.publish(AWS_IOT_PUBLISH_TOPIC_LIVE_4, Json);
       #endif 
    } 
 myBlynk.TerminalPrint(" Topic " + String(topic) +" Payload: "+ String(_nodeRedData));
}






/*************************************************END OF NODE RED AWS IOT ZONE********************************************************************************************/


#endif
