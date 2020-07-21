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
<<<<<<< HEAD
private:
    Adafruit_BMP280 _bmp;
    double _STANDARD_PRESSURE;
=======
    const double STANDARD_PRESSURE;

private:
    Adafruit_BMP280 _bmp;
>>>>>>> e82b9bab6762cf8582ef04469cca94bca47a9fb4
    double _referencePressure;
};

#endif