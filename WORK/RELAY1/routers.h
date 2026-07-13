#ifndef ROUTERS_H
#define ROUTERS_H

//field
const char* SSID_METEOR             ="BEK_METEOR_2.4G";
const char* SSID_METEOR_EXT         ="BEK_METEOR_2.4G_EXT";
const char* WIFI_PW_METEOR          ="Ali09042010_";
IPAddress   BLYNK_PI4(192,168,10,196);
IPAddress   BLYNK_PC(192,168,10,195);
IPAddress   BLYNK_TEST(192,168,1,46);
//field2 Box 5g
const char* SSID_METEOR2            ="Meteor_2.4g";  
const char* WIFI_PW_METEOR2          ="Ali09042010_";
IPAddress   BLYNK_PI3(192,168,11,196);
IPAddress   BLYNK_PC2(192,168,11,195);

//ch
const char* SSID_CH                 ="Bbox-Bek-2.4G" ;
const char* WIFI_PW_CH              ="Ali09042010_";
IPAddress   BLYNK_CH_PI3(192,168,1,195);
IPAddress   BLYNK_CH_PC(192,168,1,46);
IPAddress   BLYNK_CH_OMV_PVE(192,168,1,116);


//nice
const char* SSID_SFR                ="SFR_BEK-23C0";
const char* WIFI_PW_SFR             ="ali09042010";
IPAddress   BLYNK_SFR(192,168,1,46);

//mobile
const char* SSID_FLIP7              ="BEK_FLIP7" ;
const char* WIFI_PW_FLIP7           ="ali09042010";
IPAddress   BLYNK_FLIP7(10,174,107,53);  //

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
    {SSID_METEOR      , WIFI_PW_METEOR   , BLYNK_PI4    , BLYNK_PC      , BLYNK_TEST     , 8080, "FIELD"},
    {SSID_METEOR_EXT  , WIFI_PW_METEOR   , BLYNK_PI4    , BLYNK_PC      , BLYNK_TEST     , 8080, "FIELD_EXTENTION"},
    {SSID_FLIP7       , WIFI_PW_FLIP7    , BLYNK_FLIP7  , BLYNK_FLIP7   , BLYNK_FLIP7    , 8080, "MOBILE"},
//    {SSID_METEOR2     , WIFI_PW_METEOR2  , BLYNK_PI3    , BLYNK_PC2     , BLYNK_TEST     , 8080, "XXX"},
//    {SSID_CH          , WIFI_PW_CH       , BLYNK_CH_PI3 , BLYNK_CH_PC , BLYNK_CH_OMV_PVE , 8080, "CH"},
//    {SSID_SFR         , WIFI_PW_SFR      , BLYNK_SFR    , BLYNK_SFR     , BLYNK_SFR      , 8080, "NICE"} 
};
int NUM_NETWORKS = sizeof(nets) / sizeof(nets[0]);

#endif
