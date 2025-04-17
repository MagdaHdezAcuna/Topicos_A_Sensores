#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// Crear objeto del sensor BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// Variables de tiempo y temperatura
unsigned long previousMillis = 0;
const unsigned long intervalo60s = 60000; // 60 segundos

float Tmin = 99.0;
float Tmax = -99.0;
float TB = 10.0; // Temperatura base
float UC_total = 0;

// Bandera de maduración
bool maduro = false;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("No se detectó el sensor BMP180.");
    while (1);
  }
  Serial.println("Sensor BMP180 detectado correctamente.");
  Serial.println("Monitoreando unidades calor cada 60 segundos...");
}

void loop() {
  if (maduro) return; // Si ya maduró, salir del loop

  sensors_event_t event;
  bmp.getEvent(&event);

  if (event.pressure) {
    float temperatura;
    bmp.getTemperature(&temperatura);

    // Actualiza Tmin y Tmax
    if (temperatura < Tmin) Tmin = temperatura;
    if (temperatura > Tmax) Tmax = temperatura;

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervalo60s) {
      previousMillis = currentMillis;

      // Calcular unidades calor del intervalo
      float UC_minuto = ((Tmax + Tmin) / 2.0) - TB;
      if (UC_minuto < 0) UC_minuto = 0;
      UC_total += UC_minuto;

      // Mostrar reporte
      Serial.println("\n========= REPORTE CADA 60 SEGUNDOS =========");
      Serial.print("Tmin: "); Serial.print(Tmin); Serial.println(" °C");
      Serial.print("Tmax: "); Serial.print(Tmax); Serial.println(" °C");
      Serial.print("UC del intervalo: "); Serial.println(UC_minuto);
      Serial.print("UC acumuladas: "); Serial.println(UC_total);

      // Determinar etapa fenológica
      Serial.print("Etapa fenológica: ");
      if (UC_total <= 40)
        Serial.println("Emergencia");
      else if (UC_total <= 80)
        Serial.println("Germinación");
      else if (UC_total <= 250)
        Serial.println("Desarrollo vegetativo");
      else if (UC_total <= 450)
        Serial.println("Floración");
      else if (UC_total <= 650)
        Serial.println("Llenado de vainas");
      else {
        Serial.println("Maduración");
        Serial.println("La planta ha llegado a la maduración completa.");
        Serial.println("Deteniendo mediciones.");
        maduro = true; // ya maduró
      }

      Serial.println("============================================");

      // Reiniciar Tmin y Tmax para siguiente ciclo
      Tmin = 99.0;
      Tmax = -99.0;
    }
  } else {
    Serial.println("Error al leer el sensor.");
  }

  delay(2000); // Lectura cada 2 segundos
}

