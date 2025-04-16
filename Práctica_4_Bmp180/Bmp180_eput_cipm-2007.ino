#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <math.h>

Adafruit_BMP085 bmp;

// Constantes físicas
const float R_d = 287.05;     // J/(kg·K), constante del aire seco
const float R_v = 461.495;    // J/(kg·K), constante del vapor de agua

// Humedad relativa fija
const float RH = 23.23;       // en %

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
    Serial.println("No se detectó el BMP180. Verifica conexiones.");
    while (1);
  }
  Serial.println("Sensor BMP180 listo.");
}

void loop() {
  float tempC = bmp.readTemperature();   // Temperatura en °C
  float pressure = bmp.readPressure();   // Presión en Pa
  float tempK = tempC + 273.15;          // Temperatura en Kelvin
  const float pi = 3.1415926535897932;
  const float v = 2.78; // velocidad del viento en m/s
  const float r = 1.0;  // radio de la columna de aire en metros
  float A = pi * r * r; // área

  // Calcular presión de vapor de saturación (Pa) usando fórmula de Magnus-Tetens
  float p_sat = 611.21 * exp((17.502 * tempC) / (240.97 + tempC)); // Pa

  // Presión parcial de vapor de agua
  float p_v = (RH / 100.0) * p_sat;

  // Presión del aire seco
  float p_d = pressure - p_v;

  // Densidad del aire con fórmula CIPM-2007
  float density = (p_d / (R_d * tempK)) + (p_v / (R_v * tempK));

  // Mostrar resultados
  Serial.print("Temperatura: ");
  Serial.print(tempC);
  Serial.println(" °C");

  Serial.print("Presion: ");
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print("Humedad relativa: ");
  Serial.print(RH);
  Serial.println(" %");

  Serial.print("Densidad del aire (CIPM-2007): ");
  Serial.print(density, 6);
  Serial.println(" kg/m^3");

  // Calcular energía por unidad de tiempo (potencia del viento)
  float P = 0.5 * density * A * (v * v * v); 

  Serial.print("Energía por unidad de tiempo: ");
  Serial.print(P);
  Serial.println(" W");


  Serial.println("---------------------------");
  delay(5000); // Espera 5 segundos entre lecturas
}