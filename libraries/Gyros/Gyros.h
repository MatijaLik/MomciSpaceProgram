/*
  Barometer.h - Knjižnica za inicializacijo
  in komunikacijo z BMP280 barometrom preko I2C.
*/

#ifndef Gyros_h
#define Gyros_h

#include "Arduino.h"
#include <Wire.h>
#include <math.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

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
    double getYaw();

};

#endif