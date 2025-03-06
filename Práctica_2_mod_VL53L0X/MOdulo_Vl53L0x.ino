#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data (verifica la medición)
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter); //imprime la medion en milimietros pero si se divide en 10.0 se hacen "cm"
  } else {
    Serial.println(" out of range "); //marca fuera de rango que es aproximadamente 1200mm (1227mm medido) y un minimo de 50mm (45mm sin error) (con error de 5-15mm en 20mm) en la hoja de datos 
  }
    
  delay(100);
}