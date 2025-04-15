#include <SFE_BMP180.h>  // Librería para manejar el sensor BMP180
#include <Wire.h>        // Librería para comunicación I2C

// Crear un objeto del sensor BMP180
SFE_BMP180 bmp;

// Define la presión atmosférica al nivel del mar (en mbar/hPa) para calcular altitud con mayor precisión.
// Este valor debe ajustarse según la altitud de tu localidad.
#define PRESION_NIVEL_MAR 1023.0

// Factor de suavizado para el filtro EMA (Media Exponencial Móvil), entre 0.1 y 0.5 suele ser adecuado
float alpha = 0.1;

// Altitud filtrada con EMA, valor inicial en 0
float filteredaltitude = 0.0;

void setup() {
  // Iniciar comunicación por el puerto serial a 9600 baudios
  Serial.begin(9600);

  // Inicializar el sensor BMP180
  if (bmp.begin()) {
    Serial.println("BMP180 iniciado correctamente.");
  } else {
    Serial.println("Error al iniciar BMP180.");
    while (1); // Si falla el sensor, se detiene el programa aquí
  }
}

void loop() {
  char status;             // Variable para almacenar el estado de las operaciones
  double temperature;      // Temperatura medida (en grados Celsius)
  double pressure;         // Presión medida (en mbar / hPa)
  double altitude;         // Altitud calculada (en metros)

  // Iniciar medición de temperatura
  status = bmp.startTemperature();
  if (status != 0) {
    delay(status); // Esperar el tiempo requerido por el sensor

    // Obtener la temperatura
    status = bmp.getTemperature(temperature);
    if (status != 0) {

      // Iniciar medición de presión con precisión máxima (modo 3)
      status = bmp.startPressure(3);
      if (status != 0) {
        delay(status); // Esperar el tiempo requerido por el sensor

        // Obtener la presión, usando el valor de temperatura ya leído
        status = bmp.getPressure(pressure, temperature);
        if (status != 0) {

          // Calcular altitud en base a la presión medida y la presión a nivel del mar
          altitude = bmp.altitude(pressure, PRESION_NIVEL_MAR);

          // Aplicar filtro EMA a la altitud para suavizar los datos
          filteredaltitude = alpha * altitude + (1 - alpha) * filteredaltitude;

          // Mostrar altitud filtrada en el monitor serial
          Serial.print("Altura: ");
          Serial.print(filteredaltitude, 2);  // Mostrar con 2 decimales
          Serial.println(" metros");

        } else {
          Serial.println("Error al obtener presión.");
        }
      }
    }
  }

  // Esperar 0.5 segundos antes de tomar una nueva medición
  delay(500);
}