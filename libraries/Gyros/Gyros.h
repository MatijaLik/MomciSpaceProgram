/*
  Barometer.h - Knjižnica za inicializacijo
  in komunikacijo z BMP280 barometrom preko I2C.
*/

#ifndef Gyros_h
#define Gyros_h

#include "Arduino.h"
#include <Wire.h>
#include <math.h>

class Gyros
{
public:
    Gyros();
    void refresh();
    double getGyX();
    double getGyY();
    double getGyZ();
    double getAcX();
    double getAcY();
    double getAcZ();
    double getPitch(); 
    double getRoll();

private:
    int16_t _AcX, _AcY, _AcZ, _GyX, _GyY, _GyZ;
};

#endif