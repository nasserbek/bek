#ifndef ROUTERS_H
#define ROUTERS_H

//field
const char* SSID_METEOR   ="BEK_METEOR_2.4G";
IPAddress   BLYNK_PI4(192,168,10,195);
const char* SSID_METEOR2   ="Meteor-2.4g";
IPAddress   BLYNK_PC(192,168,11,195);
const char* WIFI_PW_METEOR  =  "Ali09042010_";

//ch
const char* SSID_CH     ="Bbox-Bek-2.4G" ;
IPAddress   BLYNK_CH_PI3(192,168,1,195);
IPAddress   BLYNK_CH_OMV1(192,168,1,4);
IPAddress   BLYNK_CH_OMV_PVE(192,168,1,116);
const char* WIFI_PW_CH    =  "Ali09042010_";

//nice
IPAddress   BLYNK_SFR(192,168,1,46);
const char* SSID_SFR      ="SFR_BEK-23C0";
const char* WIFI_PW_SFR     =  "ali09042010";

//mobile
IPAddress   BLYNK_FLIP7(10,174,107,53);  //
const char* SSID_FLIP7    ="BEK_FLIP7" ;
const char* WIFI_PW_FLIP7   =  "ali09042010";

struct NetworkConfig
{
    const char* ssid;
    const char* wifiPw;
    IPAddress server1;
    IPAddress server2;
    IPAddress server3;
    uint16_t port;
    const char* location;
};

NetworkConfig nets[] =
{
    {SSID_METEOR      , WIFI_PW_METEOR   , BLYNK_PI4    , BLYNK_PC     , BLYNK_FLIP7     , 8080, "PLS"},
    {SSID_METEOR2     , WIFI_PW_METEOR   , BLYNK_PI4    , BLYNK_PC     , BLYNK_FLIP7     , 8080, "PLS"},
    {SSID_FLIP7       , WIFI_PW_FLIP7    , BLYNK_FLIP7  , BLYNK_FLIP7   , BLYNK_FLIP7     , 8080, "MOBILE"},
    {SSID_CH          , WIFI_PW_CH       , BLYNK_CH_PI3 , BLYNK_CH_OMV1 , BLYNK_CH_OMV_PVE, 8080, "CH"},
    {SSID_SFR         , WIFI_PW_SFR      , BLYNK_SFR    , BLYNK_SFR     , BLYNK_SFR       , 8080, "NICE"} 
};
int NUM_NETWORKS = sizeof(nets) / sizeof(nets[0]);

#endif
