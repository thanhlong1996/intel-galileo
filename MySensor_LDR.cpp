/*------------------------------------------- Sensor LDR Class --------------------------------------------*/
/* This define MySensor_LM35 class function
 * 
 * Created 2017
 * by Nguyen Thanh Long
 * Modified 13 April 2017 - version 1
 * by Nguyen Thanh Long
 * 
 * Reference: 
   --> https://github.com/QuentinCG/Arduino-Light-Dependent-Resistor-Library/blob/master/LightDependentResistor/LightDependentResistor.h
   --> https://www.allaboutcircuits.com/projects/design-a-luxmeter-using-a-light-dependent-resistor/
 * 
 * This code is released in public domains
 */

#include "MySensor.h"

MySensor_LDR::MySensor_LDR() {
  // Do nothing
}

MySensor_LDR::MySensor_LDR(uint8_t pin) : pinData(pin) {
  pinMode(this->pinData, INPUT);
}

bool MySensor_LDR::begin(uint8_t pin) {
  this->pinData = pin;
  pinMode(this->pinData, INPUT);
  return true;
}

bool MySensor_LDR::config(int8_t arefVol, uint32_t otherRes) {
  this->refVol = arefVol;
  this->refRes = otherRes;
  return true;
}

float MySensor_LDR::getlastLux(void) {
  return this->lastLux;
}

float MySensor_LDR::readLux(void) {
  float val = analogRead(this->pinData);
  float resVol = (val / 1024.0) * this->refVol;
  float ldrVol = this->refVol - resVol;
  float ldrRes = (ldrVol / resVol) * this->refRes;
  float result = 12518931.0 / (float)pow(ldrRes, 1.405);
  return lastLux = result;
}

float MySensor_LDR::readLux(uint8_t times) {
  if (times <= 0) times = 1;  // Repair error input
  float sum = 0;
  for (int i = 0; i < times; i++) {
    sum += readLux();
  }
  return sum / times;
}

