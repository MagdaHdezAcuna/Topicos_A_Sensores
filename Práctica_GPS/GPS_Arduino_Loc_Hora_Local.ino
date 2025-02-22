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
String horaStr = "";  // Variable para la hora en UTC
char latDir, lonDir;  // Para almacenar N/S y E/W

void setup() {
  Serial.begin(9600);   // Comunicación serial por hardware (depuración)
  miSerial.begin(9600); // Comunicación serial por software
}

void loop() {
  while (miSerial.available()) {
    char dato = miSerial.read();  // Leer un carácter

    if (dato == '\n') {  // Fin de línea detectado
      if (datosRecibidos.startsWith("$GPGLL")) {
        // Extraer latitud (posición 7 a 16)
        latitudStr = datosRecibidos.substring(7, 17);
        latDir = datosRecibidos[18];  // 'N' o 'S'

        // Extraer longitud (posición 20 a 30)
        longitudStr = datosRecibidos.substring(20, 31);
        lonDir = datosRecibidos[32];  // 'E' o 'W'

        // Extraer hora en UTC (posición 34 a 40, formato "hhmmss")
        horaStr = datosRecibidos.substring(34, 40);

        // Convertir coordenadas a grados decimales
        float latitud = convertirCoordenadas(latitudStr);
        float longitud = convertirCoordenadas(longitudStr);

        // Aplicar signo según hemisferio
        if (latDir == 'S') latitud = -latitud;
        if (lonDir == 'W') longitud = -longitud;

        // Convertir la hora UTC a hora local en México (UTC-6)
        String horaLocal = convertirHoraLocal(horaStr);

        // Imprimir latitud, longitud y hora local
        Serial.print(latitud, 6);
        Serial.print(",");
        Serial.print(longitud, 6);
        Serial.print(",");
        Serial.println(horaLocal);
      }
      datosRecibidos = ""; // Limpiar buffer
    } else {
      datosRecibidos += dato; // Acumular datos
    }
  }
}

// Función para convertir coordenadas de "ddmm.mmmm" a decimal para colocarla en google Maps
float convertirCoordenadas(String coordStr) {
  float coord = coordStr.toFloat();  // Convertir string a float
  int grados = (int)(coord / 100);     // Extraer la parte entera (grados)
  float minutos = coord - (grados * 100);  // Obtener los minutos
  return grados + (minutos / 60);      // Convertir a decimal
}

// Función para convertir hora UTC ("hhmmss") a hora local en México (formato "hh:mm:ss")
String convertirHoraLocal(String horaUTC) {
  // Extraer horas, minutos y segundos
  int hora = horaUTC.substring(0, 2).toInt();
  int minuto = horaUTC.substring(2, 4).toInt();
  int segundo = horaUTC.substring(4, 6).toInt();

  // Convertir UTC a hora local (restar 6 horas)
  int horaLocal = hora - 6;
  if (horaLocal < 0) {
    horaLocal += 24; // Ajustar si es negativo
  }

  // Formatear la hora local con ceros a la izquierda
  String horaLocalStr = "";
  if (horaLocal < 10) horaLocalStr += "0";
  horaLocalStr += String(horaLocal) + ":";

  if (minuto < 10) horaLocalStr += "0";
  horaLocalStr += String(minuto) + ":";

  if (segundo < 10) horaLocalStr += "0";
  horaLocalStr += String(segundo);

  return horaLocalStr;
}
