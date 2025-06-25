unsigned long t1 = 0, t2 = 0; // Tiempos para intervalos
int dt_us = 500; 

unsigned int y_n;
float x_n=0,x_1=0, x_2=0, y_1=0, y_2=0;

void setup() {
  pinMode(13,OUTPUT);
  DDRD = 0xFF;
}

void loop() {
  PORTD = 0;
  while(1){
    t1 = micros();  //Muestra de tiempo 1
    digitalWrite(13,HIGH);
    int lectura = analogRead(0);
    byte lectura8 = lectura >>2;
    x_n = lectura8;
    y_n= x_n*0.020083365564211+x_1*(2*0.020083365564211)+x_2*0.020083365564211+y_1*1.561018075800718-y_2*0.6413515338057563;
    y_2 = y_1;
    y_1 = y_n;
    x_2 = x_1;
    x_1 = x_n;
    PORTD = y_n&255;
    digitalWrite(13,LOW);
    t2 = micros();  //Muestra de tiempo 2
    while((t2-t1)<dt_us){
      t2 = micros();
    }
  }
}
