#include "DFRobot_MICS.h"
 
#define CALIBRATION_TIME   2    // Default calibration time is three minutes
#define ADC_PIN            A0   // Analog pin connected to the sensor's analog output
#define POWER_PIN          2    // Digital pin to power the sensor
 
DFRobot_MICS_ADC mics(ADC_PIN, POWER_PIN);

float analogInputA0 = 0;
 
void setup() 
{
  Serial.begin(115200);
  while (!Serial); // Wait for serial communication to initialize
  
  // Attempt to initialize the sensor
  while (!mics.begin()) {
    Serial.println("No devices found! Please check the connections.");
    delay(1000);
  }
  Serial.println("Device connected successfully!");
 
  // Check the current power state of the sensor
  uint8_t mode = mics.getPowerState();
  if (mode == SLEEP_MODE) {
    mics.wakeUpMode();
    Serial.println("Sensor woken up successfully!");
  } else {
    Serial.println("Sensor is already in wake-up mode.");
  }
 
  // Wait for the sensor to complete its warm-up time
  while (!mics.warmUpTime(CALIBRATION_TIME)) {
    Serial.println("Please wait until the warm-up time is over!");
    delay(1000);
  }
  Serial.println("Sensor is ready for use.");
}
 
void loop() 
{
  // Read gas data from the sensor
  float coConcentration = mics.getGasData(CO);
  float ch4Concentration = mics.getGasData(CH4);
  float c2h5ohConcentration = mics.getGasData(C2H5OH);
  float h2Concentration = mics.getGasData(H2);
  float nh3Concentration = mics.getGasData(NH3);
  float no2Concentration = mics.getGasData(NO2);

  analogInputA0 = analogRead(A0);
 
  // Print the gas concentrations to the serial monitor
  Serial.print("CO (Carbon Monoxide): ");
  Serial.print(coConcentration, 1);
  Serial.println(" PPM");
 
  Serial.print("CH4 (Methane): ");
  Serial.print(ch4Concentration, 1);
  Serial.println(" PPM");
 
  Serial.print("C2H5OH (Ethanol): ");
  Serial.print(c2h5ohConcentration, 1);
  Serial.println(" PPM");
 
  Serial.print("H2 (Hydrogen): ");
  Serial.print(h2Concentration, 1);
  Serial.println(" PPM");
 
  Serial.print("NH3 (Ammonia): ");
  Serial.print(nh3Concentration, 1);
  Serial.println(" PPM");
 
  Serial.print("NO2 (Nitrogen Dioxide): ");
  Serial.print(no2Concentration, 1);
  Serial.println(" PPM");
  Serial.println();

  Serial.print("Valor de la entrada Analogica A0: ");
  Serial.print(analogInputA0, 4);
  Serial.println(" ");
  Serial.println();
 
  delay(1000); // Wait for 1 second before the next reading
  
  // Uncomment the line below to put the sensor to sleep mode after each reading
  // mics.sleepMode();
}