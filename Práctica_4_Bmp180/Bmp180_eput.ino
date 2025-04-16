#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("No se detectó el BMP180. Verifica conexiones.");
    while (1);
  }
  Serial.println("Sensor BMP180 listo.");
}

void loop() {
  float temp = bmp.readTemperature();  // °C
  float pressure = bmp.readPressure(); // Pa
  const float pi = 3.1415926535897932;
  const float v = 2.78; // velocidad del viento en m/s
  const float r = 1.0;  // radio de la columna de aire en metros
  float A = pi * r * r; // área

  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Presión: ");
  Serial.print(pressure);
  Serial.println(" Pa");

  // Calcular densidad de aire aproximada
  float density = pressure / (287.05 * (temp + 273.15)); // ρ = P / (R*T)

  Serial.print("Densidad del aire (aprox): ");
  Serial.print(density);
  Serial.println(" kg/m^3");

  // Calcular energía por unidad de tiempo (potencia del viento)
  float P = 0.5 * density * A * (v * v * v); 

  Serial.print("Energía por unidad de tiempo: ");
  Serial.print(P);
  Serial.println(" W");

  Serial.println("-----------------------------");
  delay(5000); // cada 5 segundos
}