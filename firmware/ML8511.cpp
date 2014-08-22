//C++ file for reading UV Index from the ML8511 Sensor
//By Maruchi Kim
#include <Arduino.h>
#include "Pins.h"

int averageAnalogRead(int pinToRead) {
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float getUVIntensity() {
  int uvPin = UV;
  int uvLevel = averageAnalogRead(uvPin);
//  int refLevel = averageAnalogRead(REF_3V3);
  int refLevel = 1023;  
  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;
  
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);

  return uvIntensity;
}
