#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Pines de los LEDs (D2 a D7)
const int leds[] = {2, 3, 4, 5, 6, 7};

void setup() {
    Serial.begin(115200);
    Wire.begin();
    mpu.initialize();

    // Configurar pines LED como salidas
    for(int i = 0; i < 6; i++) {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }

    if(!mpu.testConnection()) {
        Serial.println("MPU6050 no conectado!");
        while(1);
    }
    Serial.println("MPU6050 conectado!");
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convertir aceleraciones a gravedad terrestre
    float accel_x = ax / 16384.0;
    float accel_y = ay / 16384.0;
    float accel_z = az / 16384.0;

    // Determinar orientación
    float absX = abs(accel_x);
    float absY = abs(accel_y);
    float absZ = abs(accel_z);

    // Apagar todos los LEDs primero
    for(int i = 0; i < 6; i++) {
        digitalWrite(leds[i], LOW);
    }

    // Lógica de detección de posición
    if(absX > absY && absX > absZ) {
        if(accel_x > 0.8) {           // Cubo inclinado hacia FRENTE
            digitalWrite(2, HIGH);
        } else if(accel_x < -0.8) {    // Cubo inclinado hacia ATRÁS
            digitalWrite(3, HIGH);
        }
    } 
    else if(absY > absX && absY > absZ) {
        if(accel_y > 0.8) {            // Cubo inclinado hacia IZQUIERDA
            digitalWrite(4, HIGH);
        } else if(accel_y < -0.8) {    // Cubo inclinado hacia DERECHA
            digitalWrite(5, HIGH);
        }
    } 
    else {
        if(accel_z > 0.8) {            // Posición NEUTRAL (arriba)
            digitalWrite(6, HIGH);
        } else if(accel_z < -0.8) {    // Cubo boca abajo
            digitalWrite(7, HIGH);
        }
    }

    delay(200); // Ajusta para mayor sensibilidad
}