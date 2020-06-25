#include "Arduino.h"
#include <Wire.h>
#include<math.h>

#include "Gyros.h"

using namespace std;

const int MPU = 0x68;
int16_t _AcX, _AcY, _AcZ, _GyX, _GyY, _GyZ;

//Acceleration data correction
const double _AcXcal = -950;
const double _AcYcal = -300;
const double _AcZcal = 0;

//Gyro correction
const double _GyXcal = 480;
const double _GyYcal = 170;
const double _GyZcal = 210;

//Constructor
Gyros::Gyros()
{
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
}

void Gyros::refresh()
{
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 12, true);
    _AcX = Wire.read() << 8 | Wire.read();
    _AcY = Wire.read() << 8 | Wire.read();
    _AcZ = Wire.read() << 8 | Wire.read();
    _GyX = Wire.read() << 8 | Wire.read();
    _GyY = Wire.read() << 8 | Wire.read();
    _GyZ = Wire.read() << 8 | Wire.read();
}

//Vrne kotne hitrosti, enote so ja
double Gyros::getGyX(){
    return _GyX + _GyXcal;
}
double Gyros::getGyY(){
    return _GyY + _GyYcal;
}
double Gyros::getGyZ(){
    return _GyZ + _GyZcal;
}

//Vrne pospesek, enote so ja
double Gyros::getAcX(){
    return _AcX + _AcXcal;
}
double Gyros::getAcY(){
    return _AcY + _AcYcal;
}
double Gyros::getAcZ(){
    return _AcZ + _AcZcal;
}

//Vrne pitch v stopinjah
double Gyros::getPitch() 
{
    double x = _AcX;
    double y = _AcY;
    double z = _AcZ;

    double pitch = atan(x/sqrt((y*y) + (z*z))); //pitch calculation
    //converting radians into degrees
    pitch = pitch * (180.0/3.14);
    
}

//Vrne roll v stopinjah
double Gyros::getRoll(){
    double x = _AcX;
    double y = _AcY;
    double z = _AcZ;
    double roll = atan(y/sqrt((x*x) + (z*z))); //roll calculation
    roll = roll * (180.0/3.14) ;
}
