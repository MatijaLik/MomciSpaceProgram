#include "Arduino.h"
#include <SPI.h>
#include <SD.h>

#include "SocialniDemokrati.h"

//Constructor
SocialniDemokrati::SocialniDemokrati(int csPin)
{
    pinMode(csPin, OUTPUT);
    if (!SD.begin(csPin))
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");

    _headerString();
}

//Naredi vrh preglednice
/*
Čas, relativno na launch
Stopnja leta

Zracni tlak
Visina
Trenutna hitrost -> (Odvod višine, integral pospeška?)
Orientacija -> arccos( acceleration/g)

Accelerometer (x, y, z)
Gyroscope (pitch, yaw, roll)
*/

void SocialniDemokrati::_headerString()
{
    String s = "Time,Stage,Air pressure,Altitude,Vertical velocity,Longitudal axis velocity,Pitch,Yaw,AcX,AcY,AcZ,GyX,GyY,GyZ";
    _saveData(s);
}

void SocialniDemokrati::logData(
    double time,
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
    double GyZ)
{
    String s;
    _appendToString(s, time);
    s += stage + ",";
    s = _appendToString(s, pressure);
    s = _appendToString(s, altitude);
    s = _appendToString(s, Vvelocity);
    s = _appendToString(s, LAvelocity);

    s = _appendToString(s, pitch);
    s = _appendToString(s, yaw);

    s = _appendToString(s, AcX);
    s = _appendToString(s, AcY);
    s = _appendToString(s, AcZ);

    s = _appendToString(s, GyX);
    s = _appendToString(s, GyY);
    s = _appendToString(s, GyZ);

    _saveData(s);
}

String SocialniDemokrati::_appendToString(String s, double i)
{
    return s + String(i) + ",";
}

void SocialniDemokrati::_saveData(String s)
{

    File sensorData = SD.open("data.csv", FILE_WRITE);
    if (sensorData)
    {
        sensorData.println(s);
        sensorData.close(); // close the file
    }
}