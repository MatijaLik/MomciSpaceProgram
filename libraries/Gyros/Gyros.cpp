#include "Arduino.h"
#include <math.h>

#include "Gyros.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define acc acceleration

Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;


//Acceleration data correction
const double _AcXcal = 0;
const double _AcYcal = 0;
const double _AcZcal = 0;

//Gyro correction
const double _GyXcal = 0;
const double _GyYcal = 0;
const double _GyZcal = 0;

//Constructor
Gyros::Gyros()
{
    Serial.begin(9600);
    Serial.println("Starting MPU 6050");
    // Try to initialize!
    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void Gyros::refresh()
{
    mpu.getEvent(&a, &g, &temp);
}

//Vrne kotne hitrosti, enote so ja
double Gyros::getGyX()
{
    return g.gyro.x + _GyXcal;
}
double Gyros::getGyY()
{
    return g.gyro.y + _GyYcal;
}
double Gyros::getGyZ()
{
    return g.gyro.z + _GyZcal;
}

//Vrne pospesek, enote so ja
double Gyros::getAcX()
{
    return a.acceleration.x + _AcXcal;
}
double Gyros::getAcY()
{
    return a.acceleration.y + _AcYcal;
}
double Gyros::getAcZ()
{
    return a.acceleration.z + _AcZcal;
}

//Vrne pitch v stopinjah
double Gyros::getPitch()
{
    double x = a.acceleration.x;
    double y = a.acceleration.y;
    double z = a.acceleration.z;

    double pitch = atan(x / sqrt((y * y) + (z * z))); //pitch calculation
    //converting radians into degrees
    pitch = pitch * (180.0 / 3.14);
}

//Vrne yaw v stopinjah
double Gyros::getYaw()
{
    double x = a.acceleration.x;
    double y = a.acceleration.x;
    double z = a.acceleration.x;
    double roll = atan(y / sqrt((x * x) + (z * z))); //roll calculation
    roll = roll * (180.0 / 3.14);
}
