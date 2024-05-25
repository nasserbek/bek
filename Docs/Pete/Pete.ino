#define BLYNK_DEVICE_NAME "TEST"

#define BLYNK_TEMPLATE_ID           "TMPL5Y2na6zpd"
#define BLYNK_TEMPLATE_NAME         "CSR"
#define BLYNK_AUTH_TOKEN "1Wq6Re2q9eTOK8D5vfHhynNN2B_XoZ83" 

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
char ssid[] = "XIAOMI_BEK";
char pass[] = "ali09042010";

int blynk_timeout_seconds = 5;                // Timeout value for the Blynk.connect(timeout) command. The library default is 18 seconds
long check_connection_seconds = 30;           // Check the connection every 30 seconds
int wait_between_wifi_attempts_millis = 500;  // How long do we wait (in milliseconds) between each WiFi connection attempt
int max_wifi_connect_attempts = 15;           // How may times we try to connect to WiFi before giving-up 

BlynkTimer timer; 


void setup()
{
  Serial.begin(115200);
  Blynk.config(BLYNK_AUTH_TOKEN); // When we try to connect to Blynk, these are the credentials we'll use
  Connect_to_WiFi();              // Call the function that attempts to connect to WiFi

  timer.setInterval(check_connection_seconds * 1000L, Check_Connections);

  Connect_To_Blynk();             // Call the function that attempts to connect to Blynk
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) // Commented-out for testing
  {
    if(Blynk.connected())
    {
      // Only execute Blynk.run if we are connected to Blynk
      Blynk.run();
    }
  }   
  timer.run();  // feed the timer process
}


 void Connect_to_WiFi()
{
  Serial.println(F("Connecting to Wi-Fi..."));
  
  int wifi_attempt_count = 1; // used to track how many times we've tried to connect to WiFi

  if (WiFi.status() != WL_CONNECTED)
  {
      WiFi.begin(ssid, pass); // connect to WiFi
  }

  while (WiFi.status() != WL_CONNECTED  && wifi_attempt_count < max_wifi_connect_attempts) // Loop until we've connected, or reached the maximum number of attemps allowed
  {

    Serial.print(F("Wi-Fi connection - attempt # "));
    Serial.print(wifi_attempt_count);
    Serial.print(F(" of "));
    Serial.println(max_wifi_connect_attempts); 
    timer.run();
    delay(wait_between_wifi_attempts_millis);
    timer.run();
    wifi_attempt_count++;       
  }

  // We reach this point when either we're connected to Wi-Fi, or we've reached the maximum number of attempts.
  // We need to do differnet things, depending which it is...
  if (WiFi.status() == WL_CONNECTED)
  {
    // We get here if we're connected to WiFi
    WiFi.mode(WIFI_STA); // Put the NodeMCU in Station Mode, so it doesn't broadcast its SSID
    Serial.println(F("Successfully connected to Wi-Fi"));
  }
  else
  {  
    // we get here if we tried multiple times, but can't connect to Wi-Fi. We need to go into standalone mode...
    Serial.println(F("Failed to connect to Wi-Fi"));    
  }
}


void Connect_To_Blynk()
{
  if (WiFi.status() == WL_CONNECTED)// Commented-out for testing
  {
    // if we're connected to WiFi then try to connect to Blynk...
    Serial.println(F("Attempting to connect to Blynk..."));      
    Blynk.connect(blynk_timeout_seconds*1000);

    Serial.println(F("Blynk.connect() attempt ended"));                         // For testing 
    if(Blynk.connected())
    {
      Serial.println(F("Blynk connection attempt succeeded - connected"));       // For testing         
    }
    else
    {
      Serial.println(F("Blynk connection attempt failed - not connected"));     // For testing   
    }
  }
}


void Check_Connections() // Called with a timer 
{
  Serial.print("Checking connections.. ");
  
  if (WiFi.status() == WL_CONNECTED)
  {
    // We get here if we are connected to WiFi...
    Serial.print("WiFi Okay,  ");

    // now check if we're connected to Blynk...
    if(Blynk.connected())
    {
      Serial.println("Blynk Okay");      
    }  
    else
    {
      // We get here if we are connected to WiFi, but not to Blynk, so try to re-connect...
      Serial.println("Blynk not connected");        
      
      Connect_To_Blynk();        
    }
  }
  else
  {
    // We get here if we aren't connected to WiFi & Blynk, so try to re-connect...
    Serial.println("WiFi not connected");   
    Connect_to_WiFi(); // Commented-out for testing
    Connect_To_Blynk();   
  }
}
