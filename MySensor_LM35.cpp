/*------------------------------------------- Sensor LM35 Class --------------------------------------------*/
/* This define MySensor_LM35 class function
 * 
 * Created 2017
 * by Nguyen Thanh Long
 * Modified 13 April 2017 - version 1
 * by Nguyen Thanh Long
 * 
 * This code is released in public domains
 */
 
#include "MySensor.h"

MySensor_LM35::MySensor_LM35() {
  // Do nothing  
}

MySensor_LM35::MySensor_LM35(uint8_t pin) : pinData(pin) {
  pinMode(this->pinData, INPUT);
}

bool MySensor_LM35::begin(uint8_t pin) {
  this->pinData = pin;
  pinMode(this->pinData, INPUT);
  return true;
}

bool MySensor_LM35::config(int8_t arefVol) {
  this->refVol = arefVol;
  return true;
}

float MySensor_LM35::getlastTemp(void) {
  return this->lastTemp;
}

float MySensor_LM35::readTemperature(void) {
  float val = analogRead(this->pinData);
  return lastTemp = val * (this->refVol / 1024.0) * 100.0;
}

float MySensor_LM35::readTemperature(uint8_t times) {
  if (times <= 0) times = 1;  // Repair error input
  float sum = 0;
  for (int i = 0; i < times; i++) {
    sum += readTemperature();
  }
  return sum / times;
}

float MySensor_LM35::readKelvinTemp(void) {
  return readTemperature() + 273.15;
}

float MySensor_LM35::readFahrenheitTemp(void) {
  return readTemperature() * 1.8 + 32;
}



