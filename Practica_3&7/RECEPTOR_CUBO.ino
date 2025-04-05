void setup() {
    Serial.begin(9600);

    for (int i = 2; i <= 7; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
}

void loop() {
    if (Serial.available()) {
        char received = Serial.read();
        Serial.print("Dato recibido: ");
        Serial.println(received);

        // Apagar todos los LEDs antes de encender el correspondiente
        for (int i = 2; i <= 7; i++) {
            digitalWrite(i, LOW);
        }

        // Encender el LED correspondiente según el carácter recibido
        switch (received) {
            case 'a': digitalWrite(2, HIGH); break; // Frente
            case 'b': digitalWrite(3, HIGH); break; // Atrás
            case 'c': digitalWrite(4, HIGH); break; // Izquierda
            case 'd': digitalWrite(5, HIGH); break; // Derecha
            case 'e': digitalWrite(6, HIGH); break; // Arriba
            case 'f': digitalWrite(7, HIGH); break; // Abajo
        }
    }
}
