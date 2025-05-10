// ARDUINO DEMO FOR GROVE-ATH20
//
#include <Wire.h>
#include "ATH20.h"

ATH20 ATH;

void setup()
{
    Serial.begin(9600);
    Serial.println("ATH20 DEMO");
    ATH.begin();
}

void loop()
{
    float humi, temp;

    int ret = ATH.getSensor(&humi, &temp);

    if(ret)     // Datos obtenidos correctamente
    {
        Serial.print("HUMEDAD: ");
        Serial.print(humi*100);
        Serial.print("%\t TEMPERATURA: ");
        Serial.println(temp);
    }
    else        // Fallo de obtención de datos 
    {
        Serial.println("MEDICIÓN DE DATOS FALLIDO");
    }

    delay(100);
}
