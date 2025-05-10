#include <Wire.h>
#include <SparkFun_Qwiic_Humidity_AHT20.h>

// Crear un objeto del sensor AHT20
AHT20 myAHT20;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Inicializar el sensor
  if (myAHT20.begin() == false) {
    Serial.println("No se encontró el sensor AHT20. Verifica la conexión.");
    while (1); // Detiene el programa si no se detecta el sensor
  }

  Serial.println("Sensor AHT20 encontrado. Iniciando lectura de humedad...");
}

void loop() {
  // Leer humedad relativa
  float humedad = myAHT20.getHumidity();

  // Mostrar el valor en el monitor serial
  Serial.print("Humedad relativa: ");
  Serial.print(humedad);
  Serial.println(" %");

  delay(2000); // Esperar 2 segundos antes de la próxima lectura
}