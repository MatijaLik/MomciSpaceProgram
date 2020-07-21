/*
  Barometer.h - Knjižnica za inicializacijo
  in komunikacijo z BMP280 barometrom preko I2C.
*/

#ifndef SocialniDemokrati_h
#define SocialniDemokrati_h

#include "Arduino.h"
#include <SPI.h>
#include <SD.h>

class SocialniDemokrati
{
public:
    SocialniDemokrati(int csPin);
    void logData(double time,
    String stage,
    double pressure,
    double altitude,
    double Vvelocity,
    double LAvelocity,

    double pitch,
    double yaw,

    double AcX,
    double AcY,
    double AcZ,

    double GyX,
    double GyY,
    double GyZ);
private:
  void _headerString();
  String _appendToString(String s, double i);
  void _saveData(String s);
};

#endif