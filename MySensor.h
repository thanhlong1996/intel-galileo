/*------------------------------------------- Sensor Library --------------------------------------------*/
/* This Library support almost all need function to work with Sensor 
 * 
 * Created 2017
 * by Nguyen Thanh Long
 * Modified 13 April 2017 - version 1
 * by Nguyen Thanh Long

 * File require: MySensor.h, MySensor_LDR.cpp, MySensor_LM35.cpp
 * Assembling: Arduino 1.6.9
 * Reference link:
   --> https://github.com/codebendercc/arduino-core-files/blob/master/v102/hardware/tools/avr/lib/avr/include/stdint.h

 * Acronyms table:
   --> LDR - Photoresistor
 *
 * This code is released in public domains
 */
#ifndef MY_SENSOR_H_
#define MY_SENSOR_H_

//////////////////////////////////////////
///////SOME OLD IDE ERROR CONFIG//////////
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"  // Beta version
#else
  #include "WProgram.h" // Non - Beta version
#endif
//////////////////////////////////////////
#include <math.h> // Import some math function (in global namespace)
#include <stdint.h> 

class MySensor_LM35 {
  public:
    MySensor_LM35();
    MySensor_LM35(uint8_t pin);
    bool begin(uint8_t pin = 4);
    bool config(int8_t arefVol = 5);
    float getlastTemp(void);
    float readTemperature(void); // In Celcius
    float readTemperature(uint8_t times);
    float readKelvinTemp(void); // In Kelvin
    float readFahrenheitTemp(void); // In Fahrenheit
  private:
    uint8_t pinData;
    float lastTemp = 0;
    int8_t refVol = 5;
};

class MySensor_LDR {
  public:
    MySensor_LDR();
    MySensor_LDR(uint8_t pin);
    bool begin(uint8_t pin = 3);
    bool config(int8_t arefVol = 5, uint32_t otherRes = 1000);
    float getlastLux(void);
    float readLux(void);
    float readLux(uint8_t times);
  private:
    uint8_t pinData;
    float lastLux = 0;
    int8_t refVol = 5;
    uint32_t refRes = 1000;
};

#endif // MY_SENSOR_H_

