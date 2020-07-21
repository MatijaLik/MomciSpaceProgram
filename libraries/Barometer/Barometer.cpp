#include "Arduino.h"
#include <Adafruit_BMP280.h>
#include "Barometer.h"


<<<<<<< HEAD
double _STANDARD_PRESSURE = 1019.66;
double _referencePressure = 1019.66;

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

//Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bmp(BMP_CS); // hardware SPI


//Constructor
Barometer::Barometer()
{   
    Serial.begin(9600);
    Serial.println("Attempting to start Barometer");
    Adafruit_BMP280 _bmp(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);
=======
const double STANDARD_PRESSURE = 1019.66;
double _referencePressure = 1019.66;

//Constructor
Barometer::Barometer()
{   
    Adafruit_BMP280 _bmp;
>>>>>>> e82b9bab6762cf8582ef04469cca94bca47a9fb4
    if (!_bmp.begin())
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while(1);
    }
<<<<<<< HEAD

    Serial.println("Barometer initialized");
=======
>>>>>>> e82b9bab6762cf8582ef04469cca94bca47a9fb4
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
<<<<<<< HEAD
    return _bmp.readAltitude(_STANDARD_PRESSURE);
=======
    return _bmp.readAltitude(STANDARD_PRESSURE);
>>>>>>> e82b9bab6762cf8582ef04469cca94bca47a9fb4
}

double Barometer::getRelAltitude(){
    //Vrne visino v metrih, glede na referencno
    return _bmp.readAltitude(_referencePressure);
}