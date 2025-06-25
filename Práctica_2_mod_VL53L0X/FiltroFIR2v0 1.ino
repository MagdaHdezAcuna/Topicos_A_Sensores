unsigned long t1 = 0, t2 = 0; // Tiempos para intervalos
int dt_us = 833; 
#define orden 17
const float coeficientes_b [orden]{
  -0.027453, -0.028409, -0.027609, -0.024859, -0.019722,
  -0.011495, -0.000213,  0.014649,  0.031128,
   0.047213,  0.060954,  0.070652,  0.075029,
   0.073407,  0.065795,  0.052936,  0.036245
};
float memoria [orden];
unsigned int y_n;
void setup() {
  pinMode(13,OUTPUT);
  DDRD = 0xFF;
}

void loop() {
  for(int n = 1; n<= orden;n++){
    memoria[n] = 0;
  }
  while(1){
    t1 = micros();  //Muestra de tiempo 1
    digitalWrite(13,HIGH);
    for(int k = orden; k!= 1;k--){
      memoria[k] = memoria[k-1];
    }
    int lectura = analogRead(0);
    byte lectura8 = lectura >>2;
    memoria[1] = lectura8;
    y_n=0;
    for(int n = 1; n<= orden;n++){
      y_n += coeficientes_b[n]*memoria[n];
    }
    PORTD = y_n&255;
    digitalWrite(13,LOW);
    t2 = micros();  //Muestra de tiempo 2
    while((t2-t1)<dt_us){
      t2 = micros();
    }
  }
}
