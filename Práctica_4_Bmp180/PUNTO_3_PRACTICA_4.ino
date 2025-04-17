#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp(10085);  // Objeto BMP180

// Configuración de pines
const int lm35Pin = A3;     // LM35 en A3
const int relayPin = 3;     // Relevador en D3 (foco)
const int fanPin = 4;       // Ventilador en D4

// Variables de control
float presionSistema;       // Almacenará la presión del BMP180
float tempBMP, tempLM35;    // Temperaturas de ambos sensores
unsigned long startTime;    // Variable para el tiempo inicial

void setup() {
  Serial.begin(9600);
  // Configurar pines de salida
  pinMode(relayPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  // Iniciar con ambos dispositivos apagados
  digitalWrite(relayPin, LOW);
  digitalWrite(fanPin, LOW);

  if (!bmp.begin()) {           // Verificar BMP180
    Serial.println("ERROR: BMP180 no detectado!");
    while(1);
  }
  
  startTime = millis();  // Registrar tiempo de inicio
  
  Serial.println("Sistema Termodinámico Controlado");
  Serial.println("Comandos:");
  Serial.println("1 - Encender foco");
  Serial.println("0 - Apagar foco");
  Serial.println("2 - Encender ventilador");
  Serial.println("3 - Apagar ventilador");
  Serial.println("-----------------------------------");
  // Nuevo encabezado del CSV con tiempo
  Serial.println("Tiempo (s),Presion (kPa),Temp BMP (°C),Temp LM35 (°C),Foco,Ventilador");
}

void loop() {
  // 1. Control por comandos seriales
  if (Serial.available()) {
    char cmd = Serial.read();
    switch(cmd){
      case '1':
        digitalWrite(relayPin, HIGH);
        break;
      case '0':
        digitalWrite(relayPin, LOW);
        break;
      case '2':
        digitalWrite(fanPin, HIGH);
        break;
      case '3':
        digitalWrite(fanPin, LOW);
        break;
    }
  }

  // 2. Lectura de sensores
  sensors_event_t eventoPresion;
  bmp.getEvent(&eventoPresion);
  presionSistema = eventoPresion.pressure / 10.0;  // Convertir a kPa

  bmp.getTemperature(&tempBMP); 
  tempLM35 = (analogRead(lm35Pin)) * 500.0 / 1023.0; 

  // 3. Calcular tiempo transcurrido
  float tiempoTranscurrido = (millis() - startTime) / 1000.0;

  // 4. Mostrar datos en formato CSV con tiempo
  Serial.print(tiempoTranscurrido, 3);  // Tiempo con 3 decimales
  Serial.print(",");
  Serial.print(presionSistema, 2);  
  Serial.print(",");
  Serial.print(tempBMP, 3);         
  Serial.print(",");
  Serial.print(tempLM35, 3);        
  Serial.print(",");
  Serial.print(digitalRead(relayPin)); 
  Serial.print(",");
  Serial.println(digitalRead(fanPin)); 

  delay(400);
}