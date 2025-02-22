#include <SoftwareSerial.h>

// Definir los pines de RX y TX
#define RX 2
#define TX 3

// Crear el objeto SoftwareSerial
SoftwareSerial miSerial(RX, TX);

// Variable para almacenar los datos recibidos
String datosRecibidos = "";
String latitud ="";
String longitud ="";

void setup() {
  Serial.begin(9600);   // Comunicación serial por hardware (depuración)
  miSerial.begin(9600); // Comunicación serial por software

  
}

void loop() {

    int i=0;
    int n = 0;

  // Verificar si hay datos disponibles en SoftwareSerial
  while (miSerial.available()) {
    char dato = miSerial.read();  // Leer un carácter

    if (dato == '\n') {  // Si se detecta el fin de línea
      //Serial.print("Mensaje recibido: ");
      //Serial.println(datosRecibidos); // Imprimir el mensaje completo

      if (datosRecibidos.startsWith("$GPGLL")) {
        //Serial.print("Mensaje GPGSA recibido: ");
        //Serial.println(datosRecibidos);

        for(i=7;i<17;i++){
          Serial.print(datosRecibidos[i]);
        }
          Serial.print(",");
          
        for(i=20;i<31;i++){
          Serial.print(datosRecibidos[i]);
        }

        Serial.println();

      }

      datosRecibidos = ""; // Limpiar la variable para recibir un nuevo mensaje
    } else {
      datosRecibidos += dato; // Concatenar el carácter al string
    }
  }
  
}

