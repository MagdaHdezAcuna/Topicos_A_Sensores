#include <Wire.h>
#include <Adafruit_AHT10.h>

Adafruit_AHT10 aht;

// Configuración de pines
const int lm35Pin = A3;
const int relayPin = 3;
const int fanPin = 4;

// Variables de control
float humedadSistema;
float tempAHT, tempLM35;
float setpoint = 25.0;      // Temperatura deseada inicial
float hysteresis = 0.5;     // Margen de histéresis en °C
bool controlActive = false; // Estado del control automático

unsigned long startTime;
unsigned long previousMillis = 0;
const long interval = 400;

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  digitalWrite(fanPin, LOW);

  if (!aht.begin()) {
    Serial.println("ERROR: AHT10 no detectado!");
    while(1);
  }
  
  startTime = millis();
  
  Serial.println("Sistema Termodinámico Controlado");
  Serial.println("Comandos:");
  Serial.println("1/0 - Encender/Apagar foco");
  Serial.println("2/3 - Encender/Apagar ventilador");
  Serial.println("SXX.X - Establecer setpoint (ej: S25.5)");
  Serial.println("C - Activar/Desactivar control automático");
  Serial.println("HX.X - Ajustar histéresis (ej: H1.0)");
  Serial.println("Tiempo(s),Humedad(%),TempAHT(°C),TempLM35(°C),Setpoint(°C),Foco,Ventilador,Control");
}

void controlTemperatura() {
  if (!controlActive) return;

  float currentTemp = tempAHT; // Usamos la temperatura del AHT10 para el control
  
  // Control ON-OFF con histéresis
  if (currentTemp < (setpoint - hysteresis)) {
    digitalWrite(relayPin, HIGH);  // Calentar
    digitalWrite(fanPin, LOW);     // Apagar ventilador al calentar
  }
  else if (currentTemp > (setpoint + hysteresis)) {
    digitalWrite(relayPin, LOW);   // Apagar calentamiento
    digitalWrite(fanPin, HIGH);    // Enfriar
  }
  else {
    // Mantener estado actual dentro de la banda muerta
    digitalWrite(fanPin, LOW);
  }
}

void procesarComandos() {
  while(Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() > 0) {
      char cmd = input[0];
      String valor = input.substring(1);

      switch(cmd) {
        case 'S': 
          setpoint = valor.toFloat();
          Serial.print("Setpoint actualizado: ");
          Serial.println(setpoint);
          break;
          
        case 'H':
          hysteresis = valor.toFloat();
          Serial.print("Histéresis actualizada: ");
          Serial.println(hysteresis);
          break;
          
        case 'C':
          controlActive = !controlActive;
          Serial.print("Control automático: ");
          Serial.println(controlActive ? "ACTIVADO" : "DESACTIVADO");
          break;
          
        case '1': 
          digitalWrite(relayPin, HIGH);
          controlActive = false; // Desactiva control automático al mando manual
          break;
          
        case '0': 
          digitalWrite(relayPin, LOW); 
          break;
          
        case '2': 
          digitalWrite(fanPin, HIGH);
          controlActive = false; // Desactiva control automático al mando manual
          break;
          
        case '3': 
          digitalWrite(fanPin, LOW); 
          break;
          
        default:
          Serial.println("Comando no válido");
          break;
      }
    }
  }
}

void loop() {
  procesarComandos();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sensors_event_t humedad, temp;
    if(aht.getEvent(&humedad, &temp)) {
      humedadSistema = humedad.relative_humidity;
      tempAHT = temp.temperature;
      
      tempLM35 = (analogRead(lm35Pin)) * 500.0 / 1023.0;
      
      controlTemperatura(); // Ejecutar control automático

      // Envío de datos
      float tiempo = (millis() - startTime) / 1000.0;
      Serial.print(tiempo, 3);
      Serial.print(",");
      Serial.print(humedadSistema, 2);
      Serial.print(",");
      Serial.print(tempAHT, 3);
      Serial.print(",");
      Serial.print(tempLM35, 3);
      Serial.print(",");
      Serial.print(setpoint, 1);
      Serial.print(",");
      Serial.print(digitalRead(relayPin));
      Serial.print(",");
      Serial.print(digitalRead(fanPin));
      Serial.print(",");
      Serial.println(controlActive ? 1 : 0);
    }
  }
}