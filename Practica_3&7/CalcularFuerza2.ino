#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const float masa = 0.045; // masa del objeto en kg
const float umbral_g = 0.5; // umbral mínimo para considerar que hubo empuje (en g)

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("Error: MPU6050 no conectado.");
    while (1);
  }

  Serial.println("Esperando empuje en eje X...");
  delay(2000);
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convertir aceleración en X a g y luego a m/s²
  float ax_g = ax / 16384.0;
  float ax_ms2 = ax_g * 9.81;

  // Solo actuar si la aceleración en X supera el umbral
  if (abs(ax_g) > umbral_g) {
    float fuerza = masa * ax_ms2;

    Serial.print("Empuje detectado en eje X: ");
    Serial.print(ax_ms2);
    Serial.print(" m/s²  → Fuerza: ");
    Serial.print(fuerza);
    Serial.println(" N");

    delay(500); // evitar lecturas múltiples por un solo empuje
  }

  delay(50); // ciclo normal de lectura
}
