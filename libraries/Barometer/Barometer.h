/*
  Barometer.h - Knjižnica za inicializacijo
  in komunikacijo z BMP280 barometrom preko I2C.
*/

#ifndef Barometer_h
#define Barometer_h

#include "Arduino.h"
#include <Adafruit_BMP280.h>

class Barometer
{
public:
    Barometer();
    double getPressure();
    void setPressureReference();
    double getAbsAltitute();
    double getRelAltitude();
private:
    Adafruit_BMP280 _bmp;
    double _STANDARD_PRESSURE;
    double _referencePressure;
};

#endif