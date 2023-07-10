#ifndef FB_H
#define FB_H

#include <Arduino.h>



class fireBase {

  private:
  
  int  t433ChStatus[32];
  int _sevenSegmentsReading, _avOutput, _intFB;
  bool _AV_Status, _DVR_Status, _tenthDigitReady, _unitDigitReady;
  String _path, _cmdString ;
  String   t433ChStatusPath[32];
  String _output;
 
  public:
    String eventPath, eventTypeFb ;
    int eventValue;
    int eventID=0;
    
    fireBase(void) ;
    bool init();
    bool wifiConnect(void);
    int  getInteger( String _path);
    bool SendString(String _path,String _cmdString);
    void sendAlive(String _path);
  //  int stringToInt(String str);
    bool initStreams(void);
    bool firebaseRun(void);
    bool SendInt(String _path,int _cmdInt);
    bool  endTheOpenStream(void);

};


#endif
