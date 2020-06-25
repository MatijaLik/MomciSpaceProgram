#include "Arduino.h"
#include <Adafruit_BMP280.h>
#include "Barometer.h"


const double STANDARD_PRESSURE = 1019.66;
double _referencePressure = 1019.66;

//Constructor
Barometer::Barometer()
{   
    Adafruit_BMP280 _bmp;
    if (!_bmp.begin())
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while(1);
    }
    /* Default settings from datasheet. */
    _bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}


double Barometer::getPressure(){
    //Izpise tlak v pascalih
    return _bmp.readPressure();
}

void Barometer::setPressureReference(){
    //Nastavi trenutni tlak v hPa kot referencno tocko
    _referencePressure = _bmp.readPressure()/ 100.0;
}

double Barometer::getAbsAltitute(){
    //Vrne visino v metrih, glede na standardni tlak
    return _bmp.readAltitude(STANDARD_PRESSURE);
}

double Barometer::getRelAltitude(){
    //Vrne visino v metrih, glede na referencno
    return _bmp.readAltitude(_referencePressure);
}