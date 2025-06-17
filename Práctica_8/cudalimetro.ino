// --- Variables globales ---
volatile int NumPulsos1 = 0;  // Pulsos del sensor 1
volatile int NumPulsos2 = 0;  // Pulsos del sensor 2

const int PinSensor1 = 2;     // Pin del sensor 1 (interrupción 0)
const int PinSensor2 = 3;     // Pin del sensor 2 (interrupción 1)

const float factor_conversion = 15.5;  // Factor calibrado para convertir a L/min

// --- Rutinas de interrupción ---
void ContarPulsos1() {
  NumPulsos1++;
}

void ContarPulsos2() {
  NumPulsos2++;
}

// --- Función para medir frecuencias ---
void ObtenerFrecuencias(int &f1, int &f2) {
  NumPulsos1 = 0;
  NumPulsos2 = 0;
  interrupts();        // Activar interrupciones
  delay(1000);         // Medición de 1 segundo
  noInterrupts();      // Pausar interrupciones para lectura segura
  f1 = NumPulsos1;
  f2 = NumPulsos2;
}

// --- Configuración inicial ---
void setup() {
  Serial.begin(9600);
  pinMode(PinSensor1, INPUT);
  pinMode(PinSensor2, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(PinSensor1), ContarPulsos1, RISING);
  attachInterrupt(digitalPinToInterrupt(PinSensor2), ContarPulsos2, RISING);
}

// --- Bucle principal ---
void loop() {
  int frecuencia1, frecuencia2;
  ObtenerFrecuencias(frecuencia1, frecuencia2);

  // Caudal Sensor 1
  float caudal1_Lmin = frecuencia1 / factor_conversion;
  float caudal1_Ls   = caudal1_Lmin / 60.0;
  float caudal1_Lh   = caudal1_Lmin * 60.0;

  // Caudal Sensor 2
  float caudal2_Lmin = frecuencia2 / factor_conversion;
  float caudal2_Ls   = caudal2_Lmin / 60.0;
  float caudal2_Lh   = caudal2_Lmin * 60.0;

  // Suma total de ambos sensores
  float total_Lmin = caudal1_Lmin + caudal2_Lmin;
  float total_Ls   = caudal1_Ls + caudal2_Ls;
  float total_Lh   = caudal1_Lh + caudal2_Lh;

  // --- Mostrar datos por serial ---
  Serial.println("------ SENSOR 1 ------");
  Serial.print("Frecuencia: "); Serial.print(frecuencia1); Serial.print(" Hz\t");
  Serial.print("Caudal: "); Serial.print(caudal1_Ls, 4); Serial.print(" L/s\t");
  Serial.print(caudal1_Lmin, 3); Serial.print(" L/min\t");
  Serial.print(caudal1_Lh, 1); Serial.println(" L/h");

  Serial.println("------ SENSOR 2 ------");
  Serial.print("Frecuencia: "); Serial.print(frecuencia2); Serial.print(" Hz\t");
  Serial.print("Caudal: "); Serial.print(caudal2_Ls, 4); Serial.print(" L/s\t");
  Serial.print(caudal2_Lmin, 3); Serial.print(" L/min\t");
  Serial.print(caudal2_Lh, 1); Serial.println(" L/h");

  Serial.println("------ TOTAL ------");
  Serial.print("Total Caudal: ");
  Serial.print(total_Ls, 4); Serial.print(" L/s\t");
  Serial.print(total_Lmin, 3); Serial.print(" L/min\t");
  Serial.print(total_Lh, 1); Serial.println(" L/h");
  Serial.println();
}
