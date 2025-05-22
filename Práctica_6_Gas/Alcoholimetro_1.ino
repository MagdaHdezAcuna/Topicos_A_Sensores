int alcohol;
void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(7,OUTPUT);

}

void loop() {
   
   alcohol = analogRead(A0);//midel las ppm de un gas
   float Nivel_alcohol= 0.0;
   Nivel_alcohol = alcohol * 0.926; //fomrula 

   if (alcohol>49){
    digitalWrite(2,LOW); //apaga led verde
    digitalWrite(3,HIGH); //enciende led rojo
    digitalWrite(7,HIGH); //enciende buzzer por 1s
    delay(1000);
    digitalWrite(7,LOW);//apaga buzzer
   }
   else{
    digitalWrite(2,HIGH);
    digitalWrite(3,LOW);
   }
  Serial.print("Lectura cruda: ");
  Serial.print(alcohol);
  Serial.print(" ppm  |  Nivel estimado: ");
  Serial.print(Nivel_alcohol);
  Serial.println(" mg/lt");

  delay(100);
}
