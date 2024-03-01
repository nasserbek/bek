// aws.h

#ifndef AWS_H
#define AWS_H

extern blynk myBlynk;

/*************************************************NODE RED AWS IOT ZONE********************************************************************************************/
void retriveDataFromTopic (char* topic, byte* payload, unsigned int length )
{
        for (int i=0;i<length;i++) //Converts the received message to String
        {      
          resultS= resultS + (char)payload[i];
        }

        DeserializationError error = deserializeJson(doc1, resultS); //Command to derealize the received Json
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
                return (Q_EVENT_RM_ID_10_V112);
          break;

          case 2:
                    return (Q_EVENT_RM_ID_02_V122);
          break;
          case 3:
                    return (Q_EVENT_RM_ID_03_V123);
          break;
          case 4:
                    return (Q_EVENT_RM_ID_04_V124);
          break;
          case 5:
                    return (Q_EVENT_RM_ID_05_V125);
          break;
          case 6:
                    return (Q_EVENT_RM_ID_06_V126);
          break;
          case 7:
                   return (Q_EVENT_RM_ID_07_V127);
          break;
          case 8:
                    return (Q_EVENT_RM_ID_08_V93);
          break;
          case 9:
                    return (Q_EVENT_RM_ID_09_V80);
          break;
          case 10:
                    return (Q_EVENT_RM_ID_10_V21);
          break;
          case 11:
                   return (Q_EVENT_RM_ID_11_V14);
          break;
          case 12:
                    return (Q_EVENT_RM_ID_12_V15);
          break;
          case 13:
                    return (Q_EVENT_RM_ID_13_V23);
          break;
          case 14:
                    return (Q_EVENT_RM_ID_14_V103);
          break;
          case 15:
                    return (Q_EVENT_RM_ID_15_V104);
          break;
          case 16:
                    return (Q_EVENT_RM_ID_16_V105);
          break;
          case 17:
                    return (Q_EVENT_RM_ID_17_V90);
          break;
          case 18:
                   return (Q_EVENT_RM_ID_18_V91);
          break;
          case 19:
                    return (Q_EVENT_RM_ID_19_V92);
          break;       
          default:
           return (Q_EVENT_RM_ID_10_V112);            
        }  
 return 1;      
}     


void callback(char* topic, byte* payload, unsigned int length) {
  resultS = "";   //Empty variable from serialized Json

  if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_VIDEO)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["CMD"]; 
        nodeRedeventdata =getChID (doc1["VIDEO"]) ;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
         V_Remote_CSR1=false; myBlynk.resetRemoteVideo(1);
         V_Remote_CSR2=false; myBlynk.resetRemoteVideo(2);
         V_Remote_CSR2=false; myBlynk.resetRemoteVideo(3);
     }

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_ZAP)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["ZAP"];
        nodeRedeventdata = Q_EVENT_ZAP_V71;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapStatus(_nodeRedData);
    }

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_RX)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["RX"];
        nodeRedeventdata = Q_EVENT_SELECTED_RECIEVER_V9;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.RelaySelect(_nodeRedData);
        
    }   
    
  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_AV_RC)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["AVRC"];
        nodeRedeventdata = Q_EVENT_ROOM_AV_RC_V19;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.sendAvRxIndex(_nodeRedData);
      }   

  else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_DVR)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["DVR"];
        nodeRedeventdata = Q_EVENT_DVR_ON_OFF_V27;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.dvrSwitch(_nodeRedData); 

    }   

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_REBOOT)
    {
        myBlynk.TerminalPrint(" Received topic is: " + String(topic) +"Rebooting ESP");
        ESP.restart();
    }  


else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_SCAN)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["SCAN"];
        nodeRedeventdata = Q_SCAN_ACTIVE_CH_V4;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.resetSetupAndScan(_nodeRedData); 
     }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_REPEAT)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["REPEAT"];
        nodeRedeventdata = Q_EVENT_REPEAT_V3;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.resetSetupAndScan(_nodeRedData); 
     }
     
else if(String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_PRESET)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["PRESET"];
        nodeRedeventdata = Q_EVENT_RESET_FREQ_V26;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        
    }
    
else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_ZAPAUTO)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["ZAPAUTO"];
        nodeRedeventdata = Q_EVENT_AUTOMATIC_RC_L_R_V5;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
        myBlynk.zapAutoLocalRC(_nodeRedData); 
    }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_RC)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["RC"];
        nodeRedeventdata = Q_EVENT_RC_CH_NR_V1;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
         RC_Remote_CSR1=false; myBlynk.resetRemoteRC(1);
         RC_Remote_CSR2=false; myBlynk.resetRemoteRC(2);
         RC_Remote_CSR2=false; myBlynk.resetRemoteRC(3);
    }

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_LOCAL_WEB_OTA)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["OTAWEB"];
        nodeRedeventdata = Q_EVENT_OTA_LOCAL_WEB_WIFI_V6;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
    }    

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_GITHUB_WEB_OTA)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["OTAGITHUB"];
        nodeRedeventdata = Q_EVENT_OTA_GITHUB_V7;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
    }       

else if (String(topic) == AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA)
    {
        retriveDataFromTopic(topic, payload,length);
        _nodeRedData  = doc1["OTAIDE"];
        nodeRedeventdata = Q_EVENT_WIFI_IDE_V11;
        xQueueSend(g_event_queue_handle, &nodeRedeventdata, portMAX_DELAY);
    } 
         
 myBlynk.TerminalPrint(" Topic " + String(topic) +" Payload: "+ String(_nodeRedData));
}


void connectAWS()
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
    Serial.print(".");
    delay(1000);
    return;
  }
   if (!client.connected())
  {
    myBlynk.TerminalPrint("AWS IoT Timeout!");
    return;
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
  client.subscribe(  AWS_IOT_SUBSCRIBE_TOPIC_IDE_OTA); 
  
  doc2["version"] = VERSION_ID;
  serializeJson(doc2, Json); // print to client
  client.publish(AWS_IOT_SUBSCRIBE_TOPIC_VERSION , Json);
  myBlynk.TerminalPrint("AWS IoT Connected!");
}



/*************************************************END OF NODE RED AWS IOT ZONE********************************************************************************************/


#endif
