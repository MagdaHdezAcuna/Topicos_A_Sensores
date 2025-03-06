#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
float alpha = 0.1; // Factor de suavizado EMA (ajustable) rangos de entre 0.1 a 0.5
float filteredDistance = 0.0; // Valor filtrado inicializado

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(1);
  }
  
  Serial.println("Test de prueba VL53L0X");
  if (!lox.begin()) {
    Serial.println(F("No se pudo iniciar VL53L0X"));
    while(1);
  }

  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  lox.rangingTest(&measure, false); // Obtener medición

  if (measure.RangeStatus != 4) {  // Verifica que la medición sea válida
    float currentDistance = measure.RangeMilliMeter / 10.0; // Convertir a cm
    filteredDistance = alpha * currentDistance + (1 - alpha) * filteredDistance; // Aplicar filtro EMA

    Serial.print("Distancia medida (cm): ");
    Serial.print(currentDistance);
    Serial.print(" | Distancia filtrada (cm): ");
    Serial.println(filteredDistance);
  } else {
    Serial.println("Fuera de rango");
  }
    
  delay(100);
}
