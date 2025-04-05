#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Pines de los LEDs (D2 a D7)
const int leds[] = {2, 3, 4, 5, 6, 7};

void setup() {
    Serial.begin(9600);
    Wire.begin();
    mpu.initialize();

    // Cambiar el rango del acelerómetro a ±8g
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_8);

    // Configurar pines LED como salidas
    for (int i = 0; i < 6; i++) {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 no conectado!");
        while (1);
    }
    Serial.println("MPU6050 conectado!");
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convertir aceleraciones a gravedad terrestre (±8g → 1g = 4096)
    float accel_x = ax / 4096.0;
    float accel_y = ay / 4096.0;
    float accel_z = az / 4096.0;

    // Apagar todos los LEDs
    for (int i = 0; i < 6; i++) {
        digitalWrite(leds[i], LOW);
    }

    float absX = abs(accel_x);
    float absY = abs(accel_y);
    float absZ = abs(accel_z);

    String face = ""; // Variable con nombre de la cara

    // Detección de orientación
    if (absX > absY && absX > absZ) {
        if (accel_x > 0.8) {
            digitalWrite(2, HIGH);
            face = "Frente";
        } else if (accel_x < -0.8) {
            digitalWrite(3, HIGH);
            face = "Atrás";
        }
    } else if (absY > absX && absY > absZ) {
        if (accel_y > 0.8) {
            digitalWrite(4, HIGH);
            face = "Izquierda";
        } else if (accel_y < -0.8) {
            digitalWrite(5, HIGH);
            face = "Derecha";
        }
    } else {
        if (accel_z > 0.8) {
            digitalWrite(6, HIGH);
            face = "Arriba";
        } else if (accel_z < -0.8) {
            digitalWrite(7, HIGH);
            face = "Abajo";
        }
    }

    if (face != "") {
        Serial.println(face); // Imprime "Frente", "Atrás", etc.
    }

    delay(200); // Ajusta sensibilidad
}
