#ifndef CAYENNE_APP_H
#define CAYENNE_APP_H

#include <Arduino.h>


class cayenne{
  
  private:
           int old_fbonCAYENNE;
         
  public:
    cayenne(void) ;
    void init();
    void cayenneRun();
};
#endif
