
#include <ESP8266WiFi.h>


// utlrasonic pinout
#define ULTRASONIC1_TRIG_PIN     5   // pin TRIG to D1
#define ULTRASONIC1_ECHO_PIN     4   // pin ECHO to D2

#define ULTRASONIC2_TRIG_PIN     14   // pin TRIG to D5
#define ULTRASONIC2_ECHO_PIN     12   // pin ECHO to D6

// user config: TODO
const char* ssid = "YAMA MERA";             // Nombre de la red wifi (SSID)
const char* password = "Magico2209";         // WIFI
const char* host = "parqueadero.magicohogar.com";  // Dirección IP local o remota, del Servidor Web
const int   port = 80; 
const int   watchdog = 2000;        // Frecuencia del Watchdog
unsigned long previousMillis = millis(); 

int ID_TARJ=1;
String line;
int parqueadero=1;
int numPuesto1=1;
int numPuesto2=2;
int ocupacion1, ocupacion2;
void setup() {
  Serial.begin(115200);
   // ultraonic setup 
  pinMode(ULTRASONIC1_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC1_ECHO_PIN, INPUT); 
  pinMode(ULTRASONIC2_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC2_ECHO_PIN, INPUT);
  Serial.print("Conectando a...");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");  
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());  
}

void loop() {
  long duration, distance;
  digitalWrite(ULTRASONIC1_TRIG_PIN, LOW);  
  delayMicroseconds(2); 
  digitalWrite(ULTRASONIC1_TRIG_PIN, HIGH);
  delayMicroseconds(10); 
  digitalWrite(ULTRASONIC1_TRIG_PIN, LOW);
  duration = pulseIn(ULTRASONIC1_ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
     
  Serial.println("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.println("El puesto de parqueo 1 esta activo");
  if (distance >0 && distance<40){
    ocupacion1 = 1;
    Serial.println("uno ocupadini");
    } else {
      ocupacion1= 0;
      Serial.println("uno libreee");
      }

  digitalWrite(ULTRASONIC2_TRIG_PIN, LOW);  
  delayMicroseconds(2);         
  digitalWrite(ULTRASONIC2_TRIG_PIN, HIGH);
  delayMicroseconds(10);         
  digitalWrite(ULTRASONIC2_TRIG_PIN, LOW);
  duration = pulseIn(ULTRASONIC2_ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  Serial.println("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.println("El puesto 2 de parqueo esta activo");
  if (distance >0 && distance<40){
    ocupacion2 = 1;
    Serial.println("dos ocupadini");
    } else {
      ocupacion2= 0;
      Serial.println("dos libreee");
      }
  
  unsigned long currentMillis = millis();  
        
  if ( currentMillis - previousMillis > watchdog ) {
    previousMillis = currentMillis;
    WiFiClient client;  
    if (!client.connect(host, port)) {
      Serial.println("Conexión falló...");
      return;
    }
    String url = "/conexion_arduino/actualizacionPuesto_prueba.php?oc=";
    url += ocupacion1;
    url += "&puesto1=";
    url += numPuesto1;
    url += "&oc2=";
    url += ocupacion2;
    url += "&puesto2=";
    url += numPuesto2;
    url += "&parq=";
    url += parqueadero;

       // Envío de la solicitud al Servidor
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
    unsigned long timeout2 = millis();
       
    while (client.available() == 0) {
      if (millis() - timeout2 > 50000) {
      Serial.println(">>> Superado tiempo de espera!");
      client.stop();
      return;
      }
    }
    while(client.available()){
      line = client.readStringUntil('\r');
      //Serial.print(line);     
    }       
  }
  delay(500);       
}
