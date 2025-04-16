#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 bmp180;

void setup() {
  Serial.begin(9600);

  if (bmp180.begin())
    Serial.println("BMP180 iniciado correctamente");
  else {
    Serial.println("Error al iniciar el BMP180");
    while(1); // bucle infinito
  }
}

void loop() {
  char status;
  double T, P;

  status = bmp180.startTemperature(); // Inicio de lectura de temperatura
  if (status != 0) {   
    delay(status); // Pausa para que finalice la lectura
    status = bmp180.getTemperature(T); // Obtener temperatura en °C
    if (status != 0) {
      status = bmp180.startPressure(3); // Inicio lectura de presión
      if (status != 0) {        
        delay(status); // Pausa para que finalice la lectura        
        status = bmp180.getPressure(P, T); // Obtener presión en mb
        if (status != 0) {
          
          // Convertir presión a Pa y temperatura a K
          double pressure_Pa = P * 100.0;      
          double temp_K = T + 273.15;
          const double R = 287.05;

          // Calcular densidad del aire
          double density = pressure_Pa / (R * temp_K);

          // Mostrar resultados
          Serial.print("Temperatura: ");
          Serial.print(T, 2);
          Serial.print(" °C, ");

          Serial.print("Presion: ");
          Serial.print(P, 2);
          Serial.println(" mb");

          Serial.print("Densidad del aire: ");
          Serial.print(density, 6);
          Serial.println(" kg/m^3");

          Serial.println("-----------------------------");
        }      
      }      
    }   
  } 
  delay(2000); // Espera 2 segundos entre lecturas
}