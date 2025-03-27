#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =================== CONFIGURACIÓN DEL LCD ===================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =================== AJUSTE: cuántos pulsos por revolución ===================
#define PULSES_PER_REV  2  
// Si solo detectas 1 pulso por giro, ponlo en 1
// Si detectas 2 (media luna con dos transiciones), ponlo en 2
// Ajusta según tu prueba manual

// =================== VARIABLES GLOBALES ===================
volatile unsigned long lastPulseTime = 0;    // Tiempo (micros) del último pulso detectado
volatile unsigned long period = 0;          // Período entre el último pulso y el anterior
const unsigned long debounceThreshold = 3000;  // Ignorar pulsos que lleguen en menos de 3000us (3ms)

// Para muestreo y filtrado
unsigned long lastSampleTime = 0;           
const unsigned long samplePeriod = 50000;   // 50 ms (20 muestras/seg)
float rpmFiltered = 0;                      
const float alpha = 0.2;                    // Factor de suavizado (0 < alpha < 1)

// Tiempo máximo sin recibir pulsos para forzar RPM=0
const unsigned long maxPulseInterval = 500000;  // 500ms

// =================== INTERRUPCIÓN ===================
void isr() {
  unsigned long now = micros();
  
  // Evitar conteo de pulsos muy seguidos (ruido)
  if ((now - lastPulseTime) > debounceThreshold) {
    if (lastPulseTime != 0) {
      period = now - lastPulseTime;  
    }
    lastPulseTime = now;
  }
}

// =================== SETUP ===================
void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();

  // Configura el pin del sensor IR (por ejemplo, pin 2 con pull-up interno)
  pinMode(2, INPUT_PULLUP);

  // Ajusta FALLING o RISING según tu sensor
  attachInterrupt(digitalPinToInterrupt(2), isr, FALLING);
  
  lastSampleTime = micros();
}

// =================== LOOP ===================
void loop() {
  unsigned long currentTime = micros();

  // Verifica si han pasado 50 ms para tomar una muestra
  if (currentTime - lastSampleTime >= samplePeriod) {
    lastSampleTime = currentTime;

    // Lee el periodo de forma atómica
    noInterrupts();
    unsigned long p = period;
    unsigned long lastPulse = lastPulseTime;
    interrupts();

    float rpmInstant = 0;

    // Si pasaron más de 'maxPulseInterval' sin pulsos, asumimos ventilador parado
    if ((currentTime - lastPulse) > maxPulseInterval) {
      rpmFiltered = 0;
    }
    else {
      // Si p > 0, calculamos la frecuencia
      if (p > 0) {
        float freq = 1e6 / (float)p;   // 1 / (periodo en seg)
        rpmInstant = freq * 60.0 / PULSES_PER_REV;
      }
      // Filtrado exponencial
      rpmFiltered = alpha * rpmInstant + (1.0 - alpha) * rpmFiltered;
    }

    // Envía datos por Serial (tiempo en seg, RPM)
    Serial.print(millis() / 1000.0, 3);
    Serial.print(",");
    Serial.println(rpmFiltered, 2);

    // Actualiza el LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("__TACOMETRO__");
    lcd.setCursor(0, 1);
    lcd.print(rpmFiltered, 2);
    lcd.print(" RPM");
  }
}
