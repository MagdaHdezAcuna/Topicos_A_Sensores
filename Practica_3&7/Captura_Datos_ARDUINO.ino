#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

float yaw = 0;  // Inicializamos Yaw como acumulador

//////// Variables de red neuronal /////

const int node = 2;
const int input = 2;
const int outputs = 2;
float P[input][1];
float W1[node][input] = {{2.89075,4.25297},
                        {-0.64984,-2.85748}};
float b1[node][1];

float W2[outputs][node] = {{0.98084,2.19659},
                          {-3.4294,0.49034} };

float b2[outputs][1] = {{-1.2250},
                        {-1.9698}};  

float a2[outputs][1];
float aux = 0.0;
int maxValue = 1023;
int minValue = 0;                                             

void setup() {
    Serial.begin(9600);  
    Wire.begin();
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 no conectado!");
        while (1);
    }

    Serial.println("MPU6050 conectado!");
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Convertir aceleraciones a g
    float accel_x = ax / 16384.0;
    float accel_y = ay / 16384.0;
    float accel_z = az / 16384.0;


////////// PRIMER CAPA DE ENTRADA /////////

    // Calcular Pitch y Roll usando el acelerómetro
    float pitch = atan2(accel_y, accel_z) * 180 / PI;
    float roll = atan2(-accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * 180 / PI;

    // Obtener Yaw a partir del giroscopio (integración simple)
    float gyro_z = gz / 131.0;  // Convertimos a grados/s
    yaw += gyro_z * 0.1;  // Integración simple (suponiendo 100 ms entre lecturas)

    pitch = map(pitch,-180,180,0,1023);
    roll = map(roll,-180,180,0,1023);

  P[0][0] = dataNormalized((float)pitch,minValue,maxValue);
  P[1][0] = dataNormalized((float) roll,minValue,maxValue);

/////// segunda capa //////////

for(int i=0; i<node; i++){
  aux=0.0;
  for(int j=0;j < input;j++){
    aux=aux+W1[i][j]*P[j][0];
  }
  a1[i][0] = tansig(aux+b1[i][0]);
}

////////// Tercera Capa /////////////

for(int i=0; i<outputs;i++){
  aux =0.0;
  for(int j=0; j<node;j++){
    aux = aux +  W2[i][j]*a1[j][0];
  }
  a2[i][0] =tansig(aux+b2[i][0]);
}

if(a2[0][0] <=0 && a2[1][0]){
  
}




    // Enviar datos en el formato esperado por Octave
    Serial.print("<");
    Serial.print(pitch, 2);
    Serial.print(",");
    Serial.print(roll, 2);
    Serial.print(",");
    Serial.print(yaw, 2);
    Serial.println(">");

    delay(100);  // Muestreo de 100ms
}

float dataNormalized(int inputData, int minData, int maxData){

float valueNorm;
valueNorm = 2.0*(inputData-minData) / (maxData-minData) - 1; 

return valueNorm;
}



