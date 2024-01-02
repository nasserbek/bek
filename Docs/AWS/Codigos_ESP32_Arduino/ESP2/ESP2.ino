//Librerías necesarias
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//Pin del sensor de temperatura
#define PIN_TMP36       32

//Cliente MQTT
WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);

//Datos del WiFi
const char* ssid     = "Nombre_WiFi";
const char* password = "Contraseña";

//Datos del broker MQTT
char *server = "broker.emqx.io";
int port = 1883;

//Variables para usar millis como delay
unsigned long tact;
unsigned long tant = 0;

StaticJsonDocument<54> doc1; //Json para recibir
StaticJsonDocument<54> doc2; //Json para enviar

//Variables de botones, led y sensor de temperatura
float tempC;
int BtnPin = 23;
int LedPin = 4;
int Btn = 0;
int BtnAnt = 0;
int BtnWiFi = 0;
int BtnWiFiAnt = 0;
int LedVal = 0;
int LedVal2 = 0;

//Variable para almacenar el mensaje entrate MQTT
String resultS = "";

//Variable para almacenar el Json serializado
char Json[40];

void setup() {
  //Iniciar comunicación serial
  Serial.begin(115200);
  pinMode(BtnPin, INPUT);
  pinMode(LedPin, OUTPUT);
  //Iniciar el WiFi
  wifiInit();
  //Configurar el broker MQTT
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
}

void loop() {
  //Verificar conexión con el broker MQTT
   if (!mqttClient.connected()) {
    reconnect();
  }
  //Comprobar si hay mensajes entrates
  mqttClient.loop();

  //Flancod de subida botón de led y botón WiFi
  Btn = digitalRead(BtnPin);
  if(Btn == 1 & BtnAnt ==0 | BtnWiFi != BtnWiFiAnt)
  {
    //Se invierte el estado del led
    LedVal = !LedVal;
  }
  BtnWiFiAnt = BtnWiFi;
  BtnAnt = Btn;

  //Se escribe el estado del led al led
  digitalWrite(LedPin,LedVal);

  //Se hace la lectura de la temperatura
  tempC = temperatura();

  // Las siguientes lineas se encargan de enviar cada 2 segundos
  //las el valor de la temperatura y el estado del led
  tact = millis(); //Leer cuánto tiempo ha transcurrido
  if ( (tact-tant) >=2000) //Si la última vez que se ejecutó el if es mayor a 2 segundos, se ejecuta de nuevo el if
  {
    doc2["Temp"] = tempC; //Guardar el valor de la temperatura en formato
    doc2["LedState"] = digitalRead(LedPin); //Guardar el estado del led en formato Json
    serializeJson(doc2, Json); //Para poder mandar el formato Json guardado en doc2, se serializa y se guarda en la variable Json
    mqttClient.publish("ESP2/Val",Json); //Se publica el Json serializado en el tópico ESP2/Val
    tant = tact; //Se guarda el tiempo en que se ejecutó el if
    Serial.print("Json enviado: ");
    Serial.println(Json); //Se imprime el Json enviado en el monitor serial
  }

}

void wifiInit() {
    Serial.print("Conectándose a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
        delay(500);  
    }
    Serial.println("");
    Serial.println("Conectado a WiFi");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
  }

//Función para obtener temperatura
float temperatura(void)
{
  int adcVal = analogRead(PIN_TMP36); //Leer pin analógico
  float Volt = adcVal * (3.3 / 4095); //Hacer la conversión a volts
  float Celcius = 100*(Volt) -50; //Hacer la conversión a Celcius
  return Celcius; //Regresar la temperatura
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
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.f_str());
  }
  //Obtener los valores del Json recibido
  BtnWiFi = doc1["ESP2/LED"];
  //Imprimir el valor del Json en el monitor serial
  Serial.print(" Boton WiFi: ");
  Serial.println(BtnWiFi);
}

void reconnect() {
  //Mientras se conecta el broker MQTT
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");
    // Es importante asignar un ID al dispositivo conectado al broker
    //si se tienen ID's iguales se puede no recibir ningún mensaje
    //el ID de este ESP32 es "ESP2"
    if (mqttClient.connect("ESP2")) {
      //Imprimir en el monitor serial conectado
      Serial.println("Conectado");
      //Se suscribe al tópico ESP/main
      mqttClient.subscribe("ESP/main");
    }
  }
}
