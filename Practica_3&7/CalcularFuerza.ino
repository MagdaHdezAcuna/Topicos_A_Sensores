#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const float masa = 0.045; // masa del objeto en kg
const int muestras = 10; // cuántas veces se repite

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("Error: MPU6050 no conectado.");
    while (1);
  }

  Serial.println("Iniciando detección de fuerza aplicada...");
  delay(2000);
}

void loop() {
  float aceleracion_total;
  int16_t ax, ay, az;

  // Obtener datos de aceleración
  mpu.getAcceleration(&ax, &ay, &az);

  // Convertir a "g" (rango por defecto ±2g → dividir entre 16384)
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  // Convertir de "g" a m/s² → 1g = 9.81 m/s²
  float ax_ms2 = ax_g * 9.81;
  float ay_ms2 = ay_g * 9.81;
  float az_ms2 = az_g * 9.81;

  // Aceleración total: Pitágoras
  aceleracion_total = sqrt(ax_ms2 * ax_ms2 + ay_ms2 * ay_ms2 + az_ms2 * az_ms2);

  // Calcular fuerza
  float fuerza = masa * aceleracion_total;

  // Mostrar resultado
  Serial.print("Aceleración total: ");
  Serial.print(aceleracion_total);
  Serial.print(" m/s²  → Fuerza: ");
  Serial.print(fuerza);
  Serial.println(" N");

  delay(100); // espera para la siguiente lectura
}
