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

    //Serial.println("MPU6050 conectado!");
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convertir aceleraciones a g (gravedad terrestre)
    float accel_x = ax / 16384.0;
    float accel_y = ay / 16384.0;
    float accel_z = az / 16384.0;

    // Convertir giroscopio a grados/s
    float gyro_x = gx / 131.0;
    float gyro_y = gy / 131.0;
    float gyro_z = gz / 131.0;

    // Calcular Pitch y Roll con acelerómetro
    float pitch = atan2(accel_y, accel_z) * 180 / PI;
    float roll = atan2(-accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * 180 / PI;
    float yaw = gyro_z; // Yaw se obtiene integrando el giroscopio, pero aquí mostramos la velocidad angular.

    // Mostrar datos
   /*Serial.print("Accel (g): X=");
    Serial.print(accel_x);
    Serial.print(" Y=");
    Serial.print(accel_y);
    Serial.print(" Z=");
    Serial.print(accel_z); */
    
   /* Serial.print(" | Gyro (°/s): X=");
    Serial.print(gyro_x);
    Serial.print(" Y=");
    Serial.print(gyro_y);
    Serial.print(" Z=");
    Serial.print(gyro_z);
*/
    //Serial.print(" | Pitch=");
    //Serial.println(pitch);
    //Serial.print(" Roll=");
    //Serial.println(roll);
    //Serial.print(" Yaw=");
    //Serial.println(yaw);

    pitch=map(pitch,-180,180,0,1023);
    roll=map(roll,-180,180,0,1023);
    trama(pitch,roll);


    delay(100);
}

void trama(int value1, int value2){
  Serial.print("<");
  
  if (value1 > 999) {
    Serial.print(value1);
  }else if (value1 > 99) {
    Serial.print("0");
    Serial.print(value1);
  }else if(value1>9){
    Serial.print("00"); 
    Serial.print(value1);
  }else{
    Serial.print("000");
    Serial.print(value1);
    }
  Serial.print(",");
  
  if (value2 > 999) {
    Serial.print(value2);
  }else if (value2 > 99) {
    Serial.print("0");
    Serial.print(value2);
  }else if(value2>9){
    Serial.print("00"); 
    Serial.print(value2);
  }else{
    Serial.print("000");
    Serial.print(value2);
    }
  Serial.print(">");
  }





