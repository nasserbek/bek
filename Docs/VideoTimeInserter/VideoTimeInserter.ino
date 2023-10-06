/*
  VideoTimeInserter.ino
    
    Demonstrates a very basic on screen time display.


    This example code is in the public domain.
    Requires Arduino 1.0 or later.


    History :
      1.0 ~ Creation of MAX7456Demo from MAX7456Demo 0.4
            2013.01.31 - F. Robert Honeyman <www.coldcoredesign.com>
            
      1.1 ~ for OSD Shield
            We check the system of video in,and set the sytem of video out same with it.
            (If the video in system is the PAL,the 1.0 verison will not work well,
            so we add the checking and setting functions)
            2015.02.02 - HuangChao <www.tinysine.com>
      
*/



// Included Libraries //////////////////////////////////////////////////////////

  #include <SPI.h>
  #include <MAX7456.h>





// Pin Mapping /////////////////////////////////////////////////////////////////
  
  // pinValue = 0 means "not connected"

  //  FDTI Basic 5V                   ---  Arduino  VCC      (AVCC,VCC)
  //  FDTI Basic GND                  ---  Arduino  GND      (AGND,GND)
  //  FDTI Basic CTS                  ---  Arduino  GND      (AGND,GND)
  //  FDTI Basic DTR                  ---  Arduino  GRN
  //  FDTI Basic TXO                  ---> Arduino  TXO [PD0](RXD)
  //  FDTI Basic RXI                 <---  Arduino  RXI [PD1](TXD)
  
  
  //  Max7456 +5V   [DVDD,AVDD,PVDD]  ---  Arduino  VCC      (AVCC,VCC)
  //  Max7456 GND   [DGND,AGND,PGND]  ---  Arduino  GND      (AGND,GND)
  //  Max7456 CS    [~CS]            <---  Arduino  10  [PB2](SS/OC1B)
  //  Max7456 CS    [~CS]            <---  Mega2560 43  [PL6]
  const byte osdChipSelect             =            10;
  //  Max7456 DIN   [SDIN]           <---  Arduino  11  [PB3](MOSI/OC2)
  //  Max7456 DIN   [SDIN]           <---  Mega2560 51  [PB2](MOSI)
  const byte masterOutSlaveIn          =                      MOSI;
  //  Max7456 DOUT  [SDOUT]           ---> Arduino  12  [PB4](MISO)
  //  Max7456 DOUT  [SDOUT]           ---> Mega2560 50  [PB3](MISO)
  const byte masterInSlaveOut          =                      MISO;
  //  Max7456 SCK   [SCLK]           <---  Arduino  13  [PB5](SCK)
  //  Max7456 SCK   [SCLK]           <---  Mega2560 52  [PB1](SCK)
  const byte slaveClock                =                      SCK;
  //  Max7456 RST   [~RESET]          ---  Arduino  RST      (RESET)
  const byte osdReset                  =            15;
  //  Max7456 VSYNC [~VSYNC]          -X-
  //  Max7456 HSYNC [~HSYNC]          -X-
  //  Max7456 LOS   [LOS]             -X-





// Global Macros ///////////////////////////////////////////////////////////////
  
  //#define DEBUG true





// Global Constants ////////////////////////////////////////////////////////////

  const unsigned long debugBaud = 9600;         // Initial serial baud rate for 
                                                //   Debug PC interface
  




// Global Variables ////////////////////////////////////////////////////////////
  
  HardwareSerial Debug = Serial;                // Set debug connection
  
  MAX7456 OSD( osdChipSelect );





// Hardware Setup //////////////////////////////////////////////////////////////

  void setup() 
  {
    unsigned char system_video_in=NULL; 
    // Initialize the Serial connection to the debug computer:
    Debug.begin( debugBaud );
    
    
    // Initialize the SPI connection:
    SPI.begin();
    SPI.setClockDivider( SPI_CLOCK_DIV2 );      // Must be less than 10MHz.
    
    // Initialize the MAX7456 OSD:
    OSD.begin();                                // Use NTSC with default area.
    OSD.setSwitchingTime( 5 );                  // Set video croma distortion 
                                                //   to a minimum.
    //OSD.setCharEncoding( MAX7456_ASCII );       // Only needed if ascii font.
    
    system_video_in=OSD.videoSystem();
    if(NULL!=system_video_in)
    {
        OSD.setDefaultSystem(system_video_in) ;
    }
    else
    {
        OSD.setDefaultSystem(MAX7456_NTSC) ;
    }
    
    OSD.display();                              // Activate the text display.
    
  }
  // setup()





// Main Code ///////////////////////////////////////////////////////////////////

  void loop() 
  {
    char room[] = "213";
   
      OSD.setCursor( 0, -1 );
      OSD.print( room );
     
      while (!OSD.notInVSync())
      {
        // Add aditional code here.
      }
        
  } 
  // loop()





// Interrupt Methods ///////////////////////////////////////////////////////////





// Local Methods ///////////////////////////////////////////////////////////////

  byte isLeapYear( int year ) 
  {
    if (year % 400 == 0) return 1;
    else if (year % 100 == 0) return 0;
    else if (year % 4 == 0) return 1;
    return 0;
  }
  //end isLeapYear()

