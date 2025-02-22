#include <SoftwareSerial.h>

// Definir los pines de RX y TX
#define RX 2
#define TX 3

// Crear el objeto SoftwareSerial
SoftwareSerial miSerial(RX, TX);

// Variables para almacenar los datos recibidos
String datosRecibidos = "";
String latitudStr = "";
String longitudStr = "";
String horaStr = "";  // Variable para la hora
char latDir, lonDir;  // Para almacenar N/S y E/W

void setup() {
  Serial.begin(9600);   // Comunicación serial por hardware (depuración)
  miSerial.begin(9600); // Comunicación serial por software
}

void loop() {
  while (miSerial.available()) {
    char dato = miSerial.read();  // Leer un carácter

    if (dato == '\n') {  // Si se detecta el fin de línea
      if (datosRecibidos.startsWith("$GPGLL")) {
        // Extraer latitud (posición 7 a 16 en la cadena)
        latitudStr = datosRecibidos.substring(7, 17);
        latDir = datosRecibidos[18];  // Norte o Sur

        // Extraer longitud (posición 20 a 30 en la cadena)
        longitudStr = datosRecibidos.substring(20, 31);
        lonDir = datosRecibidos[32];  // Este u Oeste

        // Extraer hora (posición 34 a 39 en la cadena)
        horaStr = datosRecibidos.substring(34, 40);

        // Convertir coordenadas a grados decimales
        float latitud = convertirCoordenadas(latitudStr);
        float longitud = convertirCoordenadas(longitudStr);

        // Aplicar signo según hemisferio
        if (latDir == 'S') latitud = -latitud;
        if (lonDir == 'W') longitud = -longitud;

        // Imprimir coordenadas convertidas junto con la hora
        Serial.print(latitud, 6);
        Serial.print(",");
        Serial.print(longitud, 6);
        Serial.print(",");
        Serial.println(horaStr);  // Agregar la hora en UTC
      }

      datosRecibidos = ""; // Limpiar buffer
    } else {
      datosRecibidos += dato; // Acumular datos
    }
  }
}

// Función para convertir coordenadas de "ddmm.mmmm" a decimal
float convertirCoordenadas(String coordStr) {
  float coord = coordStr.toFloat();  // Convertir string a float
  int grados = (int)(coord / 100);   // Extraer la parte entera (grados)
  float minutos = coord - (grados * 100);  // Obtener los minutos
  return grados + (minutos / 60);    // Convertir a decimal
}

