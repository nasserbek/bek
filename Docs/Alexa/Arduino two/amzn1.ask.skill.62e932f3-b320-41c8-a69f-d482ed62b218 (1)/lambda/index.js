/* *
 * This sample demonstrates handling intents from an Alexa skill using the Alexa Skills Kit SDK (v2).
 * Please visit https://alexa.design/cookbook for additional examples on implementing slots, dialog management,
 * session persistence, api calls, and more.
 * */
const Alexa = require('ask-sdk-core');
//const Alexa = require('ask-sdk');
const http = require('http');
const axios = require('axios');

const firebase = require('firebase/app');
require('firebase/database');

// PLEASE FILL IN YOUR VALUES INSIDE CONFIG OBJECT. REFER TO THIS TUTORIAL TO GET STARTED : 

const config = {
  apiKey: "AIzaSyC50dSUPvWI-YD1dY-00pKgr120uzi6tjQ",
  authDomain: "bekiot-pc.firebaseapp.com",
  databaseURL: "https://bekiot-pc-default-rtdb.firebaseio.com",
  projectId: "bekiot-pc",
  storageBucket: "bekiot-pc.appspot.com",
  messagingSenderId: "711778656214",
  appId: "1:711778656214:web:039b2e460f16e5918a0e77",
  measurementId: "G-4FFKS73D4F"
};

firebase.initializeApp(config);
const database = firebase.database();



const myAuthPathBek = '/external/api/update?token=B1pZ48rPHfdQ8LxlqCoiPk8fxWBbv7B0&';
const myAuthPathBekGet = 'http://159.65.55.83/external/api/get?token=B1pZ48rPHfdQ8LxlqCoiPk8fxWBbv7B0&'

const myAuthPathBek2 = '/B1pZ48rPHfdQ8LxlqCoiPk8fxWBbv7B0/update/';
const myAuthPathBek2Get = 'http://159.65.55.83/B1pZ48rPHfdQ8LxlqCoiPk8fxWBbv7B0/get/';

var myAuthPath ;
var myAuthPathGet ;
const valueString = '=';
var virtualPin;
var virtualPinValue;
const BEK = 1;
var bek;
var blynkFeedback;


function sendToBlynk (_virtualPin, _virtualPinValue)
  {
   const blynkPath = myAuthPath.concat(_virtualPin).concat(valueString).concat(_virtualPinValue);
        var options = 
            {
            host: '159.65.55.83',//'blynk-cloud.com',
            path: blynkPath,
            port: 80,
            method: 'GET'
            }
        const req = http.request(options, (res) => { });
        // send the request
        req.write('');
        req.end();
}

function commandOnOff(commandValue)
{
    var cmd;
      switch (commandValue) 
      {
        case 'on':
            cmd= '1';
        break;
        case 'off':
            cmd = '0';
        break;
     }
 return  cmd;
}

function commandOnOffRelay(commandValue)
{
    var cmd;
      switch (commandValue) 
      {
        case 'on':
            cmd= '0';
        break;
        case 'off':
            cmd = '1';
        break;
     }
 return  cmd;
}

function commandOnOffVirtualPin(commandValue)
{
    var cmd;
      switch (commandValue) 
      {
        case 'off':
            cmd= '0';
        break;
        case 'on':
            cmd = '1';
        break;
     }
 return  cmd;
}


const GetBlynkDataIntentHandler = {
   canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'GetBlynkDataIntent';
    },

  async handle(handlerInput) {
    const responseBuilder = handlerInput.responseBuilder;
    const blynkDataStatus = handlerInput.requestEnvelope.request.intent.slots.BlunkDataString.value ;
    
    var blynkFeedback    ;
    try {
      //Call the progressive response service
      await callDirectiveService(handlerInput,"Space is a bit far way. I'll need a few seconds to get the information from Blynk.");

    } catch (err) {
      // if it failed we can continue, just the user will wait longer for first response
      console.log("error : " + err);
    }
    try {

      //bek2 = ya1T2eipkMhB3NvyLeAyRVRHqPAUXUG-    bek =D4AU1HexWcErQ9vtpkP_EgocpnoArZKC
       if  (blynkDataStatus === 'pulse width') virtualPin = 'V98';  
      //Make the external API call which will take time , 
       await axios.get(myAuthPathGet.concat(virtualPin))
       // await axios.get('http://api.open-notify.org/astros.json')
    //   .then(res => res.data)
       .then(res => {
        blynkFeedback = res;
        })

      // Now create the normal response
      // let's purposely insert a 5 second delay for this demo. You should add enough delay to get the response back from API.

      // shouldn't go longer than 5 seconds else lambda might time out
      await sleep(5000);

      let speechOutput = `${blynkDataStatus} is  ${blynkFeedback} `;
      
      return responseBuilder
        .speak(speechOutput)
        .reprompt("Ask progressive responce for blynk feedback.")
        .getResponse();

    } catch (err) {
      console.log(`Error processing events request: ${err}`);
      return responseBuilder
        .speak('Blynk is disconnected')
        .reprompt("Error Ask progressive responce for blynk feedback.")
        .getResponse();
    }
  },
};


const WaitIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'WaitIntent';
    },
    handle(handlerInput) {
       const speakOutput = 'OK';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt('I am waiting')
            .getResponse();
    }
};

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    handle(handlerInput) {
        if (BEK === 1) { myAuthPath = myAuthPathBek; myAuthPathGet = myAuthPathBekGet;bek ='BEK one vit rolle!' ; }
        else {myAuthPath = myAuthPathBek2; myAuthPathGet = myAuthPathBek2Get; bek ='BEK2' ; }
        
        const speakOutput = 'Hi, this is ' + bek;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt('ask me something')
            .getResponse();
    }
};

const HelloNasIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'HelloNasIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Hello Nas!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};


const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
       const speakOutput = 'you can say video off to turn video off, three to select room number three, zap on to activate zapping , one zap off to disactivate channel 1 zapping, reset router, reboot to reboot ESP, solo video to select remote control only video, channel seven to switch receiver to channel 7 , up or down to increase or descrease video hcannel, pulse 300, repeat 12, or wait';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt('ask me something')
            .getResponse();
    }
};


const ZapCmdIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'ZapCmdIntent';
    },
    handle(handlerInput) {
        
        var slotValue = handlerInput.requestEnvelope.request.intent.slots.ZapCmd.value ;

        const speakOutput = 'Ok,  Zap is ' + slotValue;
    virtualPinValue = commandOnOff(slotValue);
    virtualPin = 'V71';
    sendToBlynk (virtualPin, virtualPinValue);

    return handlerInput.responseBuilder
    .speak(speakOutput)
     .reprompt('ask me something')
    .getResponse();
    }
};


const RouterRestartIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'RouterRestartIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Ok, restarting the router';
        
        virtualPinValue = '1';
        virtualPin      = 'V15';
        sendToBlynk (virtualPin, virtualPinValue);
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};


const RestartEspIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'RestartEspIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Ok, rebooting the ESP';

        virtualPinValue = '1';
        virtualPin      = 'V8';
        sendToBlynk (virtualPin, virtualPinValue);

        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};


const ZapRoomSelIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'ZapRoomSelIntent';
    },
    handle(handlerInput) {
        const roomChNrValue = handlerInput.requestEnvelope.request.intent.slots.RoomZapChNr.value ;
        const roomChCmdValue = handlerInput.requestEnvelope.request.intent.slots.RoomZapCmdOnOff.value ;
        const speakOutput = 'Ok, channel ' + roomChNrValue + ' zap is ' + roomChCmdValue ;        
        
  switch (roomChNrValue) {
        case '1':
            virtualPin = 'V81';
        break;
        case '2':
            virtualPin = 'V82';
        break;
        case '3':
            virtualPin = 'V83';
        break;
        case '4':
            virtualPin = 'V84';
        break;
        case '5':
            virtualPin = 'V85';
        break;
        case '6':
            virtualPin = 'V86';
        break;
        case '7':
            virtualPin = 'V87';
        break;
        case '8':
            virtualPin = 'V88';
        break;    
        
        case '9':
            virtualPin = 'V89';
        break;
        case '10':
            virtualPin = 'V94';
        break;
        case '11':
            virtualPin = 'V95';
        break;
        case '12':
            virtualPin = 'V96';
        break;
        case '13':
            virtualPin = 'V97';
        break;
        case '14':
            virtualPin = 'V106';
        break;
        case '15':
            virtualPin = 'V107';
        break; 
    }
  
    virtualPinValue = commandOnOff(roomChCmdValue);
    sendToBlynk (virtualPin, virtualPinValue);
    
    return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt('ask me something')
            .getResponse();
    }
};


const AvRcBothIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'AvRcBothIntent';
    },
    handle(handlerInput) {
        
    const roomRemoteControlSel = handlerInput.requestEnvelope.request.intent.slots.AvRcBothTabValue.value ;
    const speakOutput = 'Ok, remote control with ' + roomRemoteControlSel ;
    
    switch (roomRemoteControlSel) {
        case 'solo video':
            virtualPinValue = '1';
        break;
        case 'solo radio':
            virtualPinValue = '2';
        break;
        case 'both':
            virtualPinValue = '3';
        break;
      }
    virtualPin = 'V19';
    sendToBlynk (virtualPin, virtualPinValue);
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt('ask me something')
            .getResponse();
    }
};


const RoomSelectionIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'RoomSelectionSelIntent';
    },
    handle(handlerInput) {
    
    const roomNr = handlerInput.requestEnvelope.request.intent.slots.RoomOneToFiveTab.value ;
    const speakOutput = 'Ok, room number  ' + roomNr + ' selected'  ;
    
    switch (roomNr) {
        case '1':
            virtualPin = 'V3';  
            virtualPinValue = '1';
        break;
        case '2':
            virtualPin = 'V3';  
            virtualPinValue = '2';
        break;
        case '3':
            virtualPin = 'V3';  
            virtualPinValue = '3';
        break;
        case '4':
            virtualPin = 'V3';  
            virtualPinValue = '4';
        break;
        case '5':
            virtualPin = 'V3';  
            virtualPinValue = '5';
        break;
        
        case '6':
            virtualPin = 'V16';  
            virtualPinValue = '6';
        break;
        case '7':
            virtualPin = 'V16';  
            virtualPinValue = '7';
        break;
        case '8':
            virtualPin = 'V16';  
            virtualPinValue = '8';
        break;
        case '9':
            virtualPin = 'V16';  
            virtualPinValue = '9';
        break;
        case '10':
            virtualPin = 'V16';  
            virtualPinValue = '10';
        break;
        
        case '11':
            virtualPin = 'V17';  
            virtualPinValue = '11';
        break;
        case '12':
            virtualPin = 'V17';  
            virtualPinValue = '12';
        break;
        case '13':
            virtualPin = 'V17';  
            virtualPinValue = '13';
        break;
        case '14':
            virtualPin = 'V17';  
            virtualPinValue = '14';
        break;
        case '15':
            virtualPin = 'V17';  
            virtualPinValue = '15';
        break;
        
     
    }

        sendToBlynk (virtualPin, virtualPinValue);
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};



const VideoOnOffIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'VideoOnOffIntent';
    },
    handle(handlerInput) {
        const videoOnOff = handlerInput.requestEnvelope.request.intent.slots.VideoOnOffCmd.value ;
        const speakOutput = 'Ok, video is   ' + videoOnOff   ;

        virtualPinValue = commandOnOffVirtualPin(videoOnOff);
        virtualPin      = 'V27';
        sendToBlynk (virtualPin, virtualPinValue);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt('ask me something')
            .getResponse();
    }
};


const VideoSurfingIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'VideoSurfingIntent';
    },
    handle(handlerInput) {
    
    const channelNr = handlerInput.requestEnvelope.request.intent.slots.ChannelSurfValue.value ;
    const speakOutput = 'Ok, video channel number   ' + channelNr ;

    virtualPinValue = channelNr;
    virtualPin = 'V2';  
    sendToBlynk (virtualPin, virtualPinValue);
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};

const VideoUpDownIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'VideoUpDownIntent';
    },
    handle(handlerInput) {
    
    const channelDir = handlerInput.requestEnvelope.request.intent.slots.UpDwonValue.value ;
    const speakOutput = 'Ok, channel ' + channelDir ;
    virtualPinValue = '1';
    if (channelDir ==='up') virtualPin = 'V90';
    else if (channelDir ==='down') virtualPin = 'V91';
 
    sendToBlynk (virtualPin, virtualPinValue);
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};




const PulseWidthIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'PulseWidthIntent';
    },
    handle(handlerInput) {
    
    const pulse = handlerInput.requestEnvelope.request.intent.slots.PulseValue.value ;
    const speakOutput = 'Ok, pulse width ' + pulse ;
    
    virtualPinValue = pulse;
    virtualPin = 'V98';
  
    sendToBlynk (virtualPin, virtualPinValue);
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};




const RepetitionIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
         &&  Alexa.getIntentName(handlerInput.requestEnvelope) === 'RepetitionIntent';
    },
    handle(handlerInput) {
    
    const repetion = handlerInput.requestEnvelope.request.intent.slots.RepetitionValue.value ;
    const speakOutput = 'Ok, repeat ' + repetion + ' times';
    
    virtualPinValue = repetion;
    virtualPin = 'V101';
  
    sendToBlynk (virtualPin, virtualPinValue);
    
        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};









const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Ok, ciao!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};
/* *
 * FallbackIntent triggers when a customer says something that doesn’t map to any intents in your skill
 * It must also be defined in the language model (if the locale supports it)
 * This handler can be safely added but will be ingnored in locales that do not support it yet 
 * */
const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Sorry';

        return handlerInput.responseBuilder
            .speak(speakOutput)
             .reprompt('ask me something')
            .getResponse();
    }
};
/* *
 * SessionEndedRequest notifies that a session was ended. This handler will be triggered when a currently open 
 * session is closed for one of the following reasons: 1) The user says "exit" or "quit". 2) The user does not 
 * respond or says something that does not match an intent defined in your voice model. 3) An error occurs 
 * */
const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Session ended: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        // Any cleanup logic goes here.
        return handlerInput.responseBuilder.getResponse(); // notice we send an empty response
    }
};
/* *
 * The intent reflector is used for interaction model testing and debugging.
 * It will simply repeat the intent the user said. You can create custom handlers for your intents 
 * by defining them above, then also adding them to the request handler chain below 
 * */
const IntentReflectorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest';
    },
    handle(handlerInput) {
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        const speakOutput = `You just triggered ${intentName}`;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};
/**
 * Generic error handling to capture any syntax or routing errors. If you receive an error
 * stating the request handler chain is not found, you have not implemented a handler for
 * the intent being invoked or included it in the skill builder below 
 * */
const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Sorry, I had trouble doing what you asked. Please try again.';
        console.log(`~~~~ Error handled: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt('ask me something')
            .getResponse();
    }
};







function callDirectiveService(handlerInput, message) {
  // Call Alexa Directive Service.
  const requestEnvelope = handlerInput.requestEnvelope;
  const directiveServiceClient = handlerInput.serviceClientFactory.getDirectiveServiceClient();

  const requestId = requestEnvelope.request.requestId;
  const endpoint = requestEnvelope.context.System.apiEndpoint;
  const token = requestEnvelope.context.System.apiAccessToken;

  // build the progressive response directive
  const directive = {
    header: {
      requestId,
    },
    directive: {
      type: "VoicePlayer.Speak",
      speech: message
    },
  };
  // send directive
  return directiveServiceClient.enqueue(directive, endpoint, token);
}


function sleep(milliseconds) {
  return new Promise(resolve => setTimeout(resolve(), milliseconds));
}





const InputIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'InputIntent';
    },
    async handle(handlerInput) {
        const moodSlot =  "dvr" ;//Alexa.getSlotValue(handlerInput.requestEnvelope, 'mood');
        var tagValue=50;
        let speakOutput = '';
        try{
            database.goOnline();
            await database.ref('Zapping/control/global/0').on('value',function(snapshot) {
                     tagValue= snapshot.val().frequency;
            });

            await database.ref('/Zapping/control/global/0/').update({
            dvr : tagValue
        })
        

        database.goOffline();
        
      if (moodSlot) {
      let resolution = handlerInput.requestEnvelope.request.intent.slots.mood.resolutions.resolutionsPerAuthority[0];
      if (resolution.status.code === "ER_SUCCESS_MATCH") {
        let mood = resolution.values[0].value.id;
        switch (mood) {
          case "1":
            speakOutput = `I just added your mood. But I'm very sorry to hear that you're feeling ${moodSlot}. You can always cheer up!`;
            break;
          case "2":
            speakOutput = `Got it. I've added your mood to the list. You can try and relax to feel better!`;
            break; 
          case "3":
            speakOutput = `I understand that you're ${moodSlot} right now & I've added that to your list. Watching a movie can help you get over this!`;
            break;
          case "4":
            speakOutput = `Ah! You're ${moodSlot}. Okay! I'll not bother you now.`;
            break;
          case "5":
            speakOutput = `That's good to hear! I've added this mood to the list`;
            break;
          case "6":
            speakOutput = `wow!! I'm so glad to hear that. `;
            break;
          case "7":
            speakOutput = `Amazing! If you're feeling ${moodSlot} then I bet you're having a great time now!`;
            break;
          case "8":
            speakOutput = `Great! That made me ${moodSlot} too! Added this.`;
            break;
          default:
            speakOutput = `Okay, i just added this.`;
            break;
        }
        }
        }
        }catch(e){
            console.log("Catch logs here: ",e);
            speakOutput = `There was a problem adding the ${moodSlot}`
        }
        console.log("Out of Try Catch");
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};


/**
 * This handler acts as the entry point for your skill, routing all request and response
 * payloads to the handlers above. Make sure any new handlers or interceptors you've
 * defined are included below. The order matters - they're processed top to bottom 
 * */
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        HelloNasIntentHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        
        GetBlynkDataIntentHandler,
        
        ZapCmdIntentHandler,
        RouterRestartIntentHandler,
        RestartEspIntentHandler,
        ZapRoomSelIntentHandler,
        AvRcBothIntentHandler,
        RoomSelectionIntentHandler,
        VideoOnOffIntentHandler,
        VideoSurfingIntentHandler,
        VideoUpDownIntentHandler,
        PulseWidthIntentHandler,
        RepetitionIntentHandler,
        WaitIntentHandler,
        InputIntentHandler,
        
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withCustomUserAgent('sample/hello-nas/v1.2')
    .lambda();