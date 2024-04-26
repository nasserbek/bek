//Librerías necesarias
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Cliente MQTT
WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

StaticJsonDocument<54> doc1; //Json para recibir
StaticJsonDocument<54> doc2; //Json para enviar

//Constantes OLED
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 //El OLED no tiene botón reset

//Declaración de la pantalla
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Datos del WiFi
const char* ssid     = "Nombre_WiFi";
const char* password = "Contraseña";

//Datos del Broker MQTT
char *server = "broker.emqx.io";
int port = 1883;

//Variables para usar millis como delay
unsigned long tact;
unsigned long tant = 0;

int nd; // Animación pantalla
String dots = ""; // Animación pantalla

//Variables correspondientes a los botones físicos
int Btn1 = 2; //Pin2
int Btn2 = 4; //Pin4
int Btn3 = 23; //Pin23
int BtnInicio = 0;
int BtnInicioAnt=0;
int BtnEnter = 0;
int BtnEnterAnt=0;
int Btnsel = 0;
int BtnselAnt=0;

//Variables para la interfaz del display
int sel = 0;
int pantalla = 0;

//Variables para almacenar la información de los leds y sensores
int LedState1=0;
int LedVal1=0;
float Temp = 0;
int LedState2=0;
int LedVal2=0;
float Pot = 0;

//Variable para almacenar el mensaje entrate MQTT
String resultS = "";

//Variable para almacenar el Json serializado a enviar
char Json[40];

void setup() {
  //Iniciar comunicación serial
  Serial.begin(115200);
  //Inicializar pantalla Oled
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 failed"));
    for(;;);
  }
  //Conectar al WiFi
  conectarWiFi();
  //Configurar broker MQTT
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop() {
  //Verificar conexión con el broker MQTT
  if (!mqttClient.connected()) {
    reconnect(); //Reconectar en caso de que se desconecte
  }
  //Comprobar si hay mensajes entrates
  mqttClient.loop();

  //Se limpia el display
  display.clearDisplay();

  //Se selecciona la pantalla deseada
  switch(pantalla)
  {
    case 0:
    pantallauno();
    break;
    
    case 1:
    pantallados();
    break;
    
    case 2:
    pantallatres();    
    break;
  }

  // Las siguientes lineas se encargan de enviar cada 2 segundos
  //las señales para encender o apagar los leds del ESP1 y ESP2
  tact = millis(); //Leer cuánto tiempo ha trasncurrido
  if ( (tact-tant) >=2000) //Si la última vez que se ejecutó el if es mayor a 2 segundos, se ejecuta de nuevo el if
  {
    doc2["ESP1/LED"] = LedVal1; //Guardar la señal del Led ESP1 en formato Json
    doc2["ESP2/LED"] = LedVal2; //Guardar la señal del Led ESP2 en formato Json
      
    serializeJson(doc2, Json); //Para poder mandar el formato Json guardado en doc2, se serializa y se guarda en la variable Json
    mqttClient.publish("ESP/main",Json); //Se publica el Json serializado en el tópico ESP/main
    tant = tact; //Se guarda el tiempo en que se ejecutó el if
    Serial.print("Json enviado: ");
    Serial.println(Json); //Se imprime el Json enviado en el monitor serial
  }
  
  display.display();  //Se muestra la pantalla en el display
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Vaciar la variable del Jsonn serializado
  resultS = "";
  //Se convierte el mensaje recibido en String
  for (int i=0;i<length;i++) {
    resultS= resultS + (char)payload[i];
  }
  //Comando para deserealizar el Json recibido
  DeserializationError error = deserializeJson(doc1, resultS);
  if (error) 
  {
    //Verificar si hay un error
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.f_str());
  }

  // Debido a que estamos suscritos a 2 tópicos, se crean condiciones
  //dependiendo del tópico que se recibe el mensaje
  if(String(topic) == "ESP1/Val")
  {
  //Obtener los valores del Json recibido
  Pot = doc1["Pot"];
  LedState1 = doc1["LedState"];
  //Imprimir en el monitor serial
  Serial.print("Del Tópico ");
  Serial.print(topic);
  Serial.print(" el voltaje en el pot es ");
  Serial.print(Pot);
  Serial.print("  y el estado del LED es ");
  Serial.println(LedState1);
  }
  
  if(String(topic) == "ESP2/Val")
  {
  //Obtener los valores del Json recibido
  Temp = doc1["Temp"];
  LedState2 = doc1["LedState"];
  //Imprimir en el monitor serial
  Serial.print("Del Tópico ");
  Serial.print(topic);
  Serial.print(" la temperatura es ");
  Serial.print(Temp);
  Serial.print("  y el estado del LED es ");
  Serial.println(LedState2);
  }
}


void reconnect() {
  //Mientras se conecta al broker MQTT
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");
    // Es importante asignar un ID al dispositivo conectado al broker
    //si se tienen ID's iguales se puede no recibir ningún mensaje
    //el ID de este ESP32 es "ESPMAIN"
    if (mqttClient.connect("ESPMAIN")) {
      //Imprimir en el monitor serial conectado
      Serial.println("Conectado");
      //Suscribirse a los tópicos de ambos ESP que estarán mandando información
      mqttClient.subscribe("ESP1/Val");
      mqttClient.subscribe("ESP2/Val");
    } 
  }
}

// En esta función se conecta al Wifi y se realiza una pequeña
//animación en el display
void conectarWiFi()
{
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED){
    delay(500);
    imprimirConectando();
  }
  imprimirConectado();
  
}
void imprimirConectando(){
  display.clearDisplay();
  if(nd>3)
  {
    dots = " ";
    nd=0;
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,24);
  display.print("Conectando");
  display.print(dots);
  dots = dots + ".";
  nd++;
  display.display();
}
// Esta fucnión imprime los datos de la red a la que se conectó
// en el display
void imprimirConectado(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5,16);
  display.print("Conectado a:");
  display.setCursor(5,28);
  display.print(ssid);
  display.setCursor(5,40);
  display.print("Direccion IP:");
  display.setCursor(5,52);
  display.print(WiFi.localIP());
  display.display();
  delay(4000);
}


void pantallauno(void)
{
  //Flanco de subida para el botón 2(Select)
  Btnsel = digitalRead(Btn2);
  if(Btnsel == 1 & BtnselAnt == 0)
  {
    sel = !sel; //Cada que se presione el botón se invierte la selección
  }
  BtnselAnt = Btnsel;

  //Imprimir Inicio en el display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("Inicio");

  //Dependiendo de la selección se resaltará ESP1 o ESP2 en el display
if(sel == 0)
  {
  display.fillRoundRect(30, 18, 65, 20, 2, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(35, 20);
  display.println("ESP 1");

  display.setTextColor(WHITE);
  display.setCursor(35, 40);
  display.println("ESP 2");
  
  }
  else if (sel == 1)
  {
  display.setTextColor(WHITE);
  display.setCursor(35, 20);
  display.println("ESP 1");

  display.fillRoundRect(30, 38, 65, 20, 2, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(35, 40);
  display.println("ESP 2");
  }

  //Flanco de subida botón 3(Enter)
  BtnEnter = digitalRead(Btn3);  
    if(BtnEnter == 1 & BtnEnterAnt == 0)
    {
      //Si se presiona enter y está seleccionado el ESP1, se cambia a la pantalla dos
     if(sel == 0)
      {
        pantalla = 1;
     }
      //Si se presiona enter y está seleccionado el ESP2, se cambia a la pantalla tres
     if(sel == 1)
     {
        pantalla = 2;
      }
    }
    BtnEnterAnt = BtnEnter;
}

void pantallados(void)
{
  //Imprimir ESP1 en el display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("Esp 1");

  //Imprimir el valor del potenciómetro en el display
  display.setTextSize(1);
  display.setCursor(7, 30);
  display.print("Pot:");
  display.println(Pot);

  //Imprimir el estado del led en el display
  display.setCursor(7, 50);
  display.print("Led: ");
  display.setTextColor(BLACK);
  display.fillRoundRect(35, 48, 20, 10, 2, WHITE);

  //Dependiendo del estado del led se imprime ON/OFF
  if(LedState1 == 0)
  {
  display.print("OFF");
  display.drawCircle(90, 40, 15, WHITE);
  }
  if(LedState1 == 1)
  {
    display.print("ON");
    display.fillCircle(90, 40, 15, WHITE);
  }

  //Flanco de subida botón 1 (Home)
  BtnInicio = digitalRead(Btn1);
    if(BtnInicio == 1 & BtnInicioAnt == 0)
    {
      //Cuando se presione home se regresa a la pantalla uno
     pantalla = 0;
    }
    BtnInicio = BtnInicio;
    
  //Flanco de subida botón 3(Enter)
  BtnEnter = digitalRead(Btn3);
  if(BtnEnter == 1 & BtnEnterAnt == 0)
  {
    //Si se presiona enter en la pantalla Uno, se invierte la señal de encendido de led
    LedVal1 = !LedVal1;
  }
  BtnEnterAnt = BtnEnter;
}

void pantallatres(void)
{
  //Imprimir ESP2 en el display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("Esp 2");

  //Imprimir el valor de la temperatura en el display
  display.setTextSize(1);
  display.setCursor(7, 30);
  display.print("Temp:");
  display.print(Temp);

  //Imprimir el estado del led en el display
  display.setCursor(7, 50);
  display.print("Led: ");
  display.setTextColor(BLACK);
  display.fillRoundRect(35, 48, 20, 10, 2, WHITE);

  //Dependiendo del estado se imprime ON/OFF
  if(LedState2 == 0)
  {
  display.print("OFF");
  display.drawCircle(90, 40, 15, WHITE);
  }
  if(LedState2 == 1)
  {
    display.print("ON");
    display.fillCircle(90, 40, 15, WHITE);
  }

  //Flanco de subida botón 1(Home)
  BtnInicio = digitalRead(Btn1);
  if(BtnInicio == 1 & BtnInicioAnt == 0)
  {
    //Cuando se presione home se regresa a la pantalla uno
   pantalla = 0;
  }
  BtnInicio = BtnInicio;

  //Flanco de subida botón 3(Enter)
  BtnEnter = digitalRead(Btn3);
  if(BtnEnter == 1 & BtnEnterAnt == 0)
  {
    //Si se presiona enter en la pantalla Uno, se invierte la señal de encendido de led
    LedVal2 = !LedVal2;
  }
  BtnEnterAnt = BtnEnter;
    
}
