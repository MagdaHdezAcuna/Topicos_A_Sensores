#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 no conectado!");
    while (1);
  }
  Serial.println("MPU6050 conectado!");
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convertimos la lectura a valores en G (1 G = 9.81 m/s²)
  float accel_x = ax / 16384.0;
  float accel_y = ay / 16384.0;
  float accel_z = az / 16384.0;

  // Graficamos los tres ejes
  Serial.print("X:");
  Serial.print(accel_x);
  Serial.print(" Y:");
  Serial.print(accel_y);
  Serial.print(" Z:");
  Serial.println(accel_z);

  delay(200); // Intervalo pequeño para mayor sensibilidad
}
