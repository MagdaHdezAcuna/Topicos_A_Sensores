#include <Wire.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

float alpha = 0.1;  // Factor de suavizado EMA
float filteredDistance = 0.0;  // Distancia filtrada

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Inicializa el sensor VL53L0X
    if (!lox.begin()) {
        Serial.println("Error al iniciar VL53L0X");
        while (1);
    }
    delay(100);
}

void loop() {
    for (int angle = 0; angle <= 180; angle += 5) {  // Barrido cada 5°

        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);  // Obtiene medición

        if (measure.RangeStatus != 4) {  // Verifica datos válidos
            float currentDistance = measure.RangeMilliMeter / 10.0;  // Convertir mm a cm
            filteredDistance = alpha * currentDistance + (1 - alpha) * filteredDistance;  // Aplica filtro EMA

            // Convierte de coordenadas polares a cartesianas
            float theta = radians(angle);  // Convierte ángulo a radianes
            float x = filteredDistance * cos(theta);  // Coordenada X
            float y = filteredDistance * sin(theta);  // Coordenada Y

            
            Serial.print(x);//muestra la cordenada x en el serial monitor
            Serial.print(",");
            Serial.println(y);//muestra la cordenada x en el serial monitor
        } else {
            Serial.println("FUERA DE RANGO");
        }

        delay(100);//100
    }
}
