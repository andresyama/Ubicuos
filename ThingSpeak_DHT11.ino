#include "ThingSpeak.h"  //libreria Thingspeak
#include "WiFi.h"        //libreria conexión WiFi
#include "DHT.h"         //libreria Sensor de Humedad y temperatura


#define pin1 13 //Pin del sensor DHT11.

//Conexion a la red WiFI

const char* ssid="YAMA MERA";  //nombre de la red de Wifi
const char* password="Magico2209"; //contraseña red Wifi

//Datos para conexion con Thingspeak
unsigned long channelID = 1517335;
const char* WriteAPIKey = "V36GQ91QZMPRTYGO";

//Creamos un objeto
WiFiClient Client;

DHT dht11(pin1, DHT11);

void setup() {
  Serial.begin(115200);
  Serial.println("Test de sensores:");

//Conexion al Router WiFi
  WiFi.begin(ssid,password);
  while (WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.println("Conectando a su red WiFi...");
  }
  Serial.println("Conectado Correctamente a su red.");

//inicializamos el servidor de Thigspeak

  ThingSpeak.begin(Client);
  
  dht11.begin();
}

void loop() {
  delay(2000);
  leerdht11();

  //Enviar datos a ThingSpeak
  ThingSpeak.writeFields(channelID,WriteAPIKey);
  Serial.println("Datos enviados a ThingSpeak!");
  delay(15000);
  
}
//Void para lectura del sensor
void leerdht11(){
  float t1 = dht11.readTemperature();
  float h1 = dht11.readHumidity();
//Validacion si el sensor esta dando informacion o si tiene un error de lectura
  while (isnan(t1) || isnan(h1)){
    Serial.println("Error en toma de muestras, repitiendo lectura");
    delay(2000);
    t1 = dht11.readTemperature();
    h1 = dht11.readHumidity();
  }
//Impresión de los datos leidos por el sensor
  Serial.print("Temperatura DHT11: ");
  Serial.print(t1);
  Serial.println(" ºC.");

  Serial.print("Humedad DHT11: ");
  Serial.print(h1);
  Serial.println(" %."); 

  Serial.println("-----------------------");

  //Asignacion de campos de sensor
  ThingSpeak.setField(1,t1);
  ThingSpeak.setField(2,h1);
}
