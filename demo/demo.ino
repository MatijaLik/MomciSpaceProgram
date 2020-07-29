#include <Servo.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <SD.h>
#include <SPI.h>
#include <avr/wdt.h>

//#include <SocialniDemokrati.h>

//SETTING BEGIN
const short BMP_MISO = 12;
const short BMP_MOSI = 11;
const short SCLK = 13;

const short BMP_CS = 3;
const short SD_CS = 10;

const short OPERATING_FREQUENCY = 1; //Refresh rate v hercih
float TIME_shortERVAL = 1.0 / OPERATING_FREQUENCY;
const short DELAY_shortERVAL = TIME_shortERVAL * 1000;

//SETTING END

float cas = 0;
short stage = 0;

float pressure = -1;
float altitude = -1;

bool verboseLogging = 1;

float _referencePressure = 1013.00;

String FILENAME;

//Adafruit_BMP280 barometer(BMP_CS, BMP_MOSI, BMP_MISO, SCLK);
Adafruit_BMP280 barometer;


Servo servo;
File f;

bool barometerReferenceSet = false;


void barometerSetup()
{
    if (!barometer.begin(0x76))
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1)
        {
        };
    }

    Serial.println("Barometer initialized");
    /* Default settings from datasheet. */
    barometer.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                          Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                          Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                          Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                          Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void SD_Setup()
{
    pinMode(SD_CS, OUTPUT);
    if (!SD.begin(SD_CS))
    {
        Serial.println(F("Card failed"));
        // don't do anything more:
        while (1)
        {
        };
        return;
    }

    Serial.println(F("SD card initialized"));
}

/*
void ciganQueueSetup()
{
    for (short i = 0; i < OPERATING_FREQUENCY + 1; i++)
        ciganQueue[i] = 0.0;
}

void ciganPushFront(float value)
{

    for (short i = OPERATING_FREQUENCY; i > 0; i--)
    {
        ciganQueue[i] = ciganQueue[i - 1];
    }

    ciganQueue[0] = value;
}
*/

void ringPiezo()
{
    tone(-1, 1000); // Send 1KHz sound signal...
    delay(1000);           // ...for 1 sec
    noTone(-1);     // Stop sound...
    delay(1000);
}

void incrementTime()
{
    //poveca cas za nek shorterval in ga zaokrozi na 2 decimalki

    float nextTime = cas + TIME_shortERVAL;
    nextTime = floor(nextTime * 100);
    nextTime /= 100;
    cas = nextTime;
}

void updateSensorValues()
{
    //Barometer  
    pressure = barometer.readPressure();
    altitude = barometer.readAltitude(_referencePressure);
    

    //ciganPushFront(altitude);
}

void readFromSerial()
{
    if (Serial.available() > 0)
    {
        String s = Serial.readString();

        s.toLowerCase();
        s.trim();
        if (s == "pref")
        {
            Serial.println(F("Manually setting reference pressure"));
            barometerReferenceSet = true;
            _referencePressure = barometer.readPressure() / 100;
        }
        else if (s == "startup")
        {
            Serial.println(F("Vehicle is now in startup"));
            stage = 1;
        }
        else if (s == "stage")
        {
            Serial.println(F("Staging"));
            stage++;
        }
        else if (s == "save")
        {
            Serial.println(F("Saving flight log"));
            f.close();
            f = SD.open(FILENAME, FILE_WRITE);
        }
        else if (s == "c")
            deployChutes();
        else if (s == "chutes")
            deployChutes();
        else if (s == "terminate")
        {
            Serial.println(F("Terminating program"));
            f.close();
            while (1)
            {
            };
        }
        else if (s == "shut")
            verboseLogging = 0;
        else if (s == "speak")
            verboseLogging = 1;
        else if (s == "reboot")
        {
            f.close();
            Serial.println(F("Rebooting"));
            delay(500);
            wdt_enable(WDTO_15MS);
            while (1)
            {
            };
        }
    }

    //pocisti vse ostalo
    while (Serial.available() > 0)
    {
        // read the incoming byte:
        short incomingByte = Serial.read();
    }
}

void writeToSerial()
{
    Serial.print(F("STAGE: "));
    Serial.println(stage);
    Serial.print(F("Time: "));
    Serial.println(cas);
    Serial.println();

    Serial.print(F("Alt: "));
    Serial.println(altitude);
    Serial.print(F("Pres: "));
    Serial.println(pressure);
    Serial.println();

    Serial.println(F("####"));
}

void writeOnSd()
{
    f = SD.open(FILENAME, FILE_WRITE);
   
    if (f)
    {
        f.print(cas);
        f.print(F(",")); 
        f.print(stage);
        f.print(F(","));    
        f.print(pressure);
        f.print(F(","));    
        f.print(altitude);
        f.print(F(","));       
    
        // close the file:
        f.close();
    }
    else
    {
        // if the file didn't open, print an error:
        Serial.println(F("error opening file"));
        while(1){};
    }

    
}

void deployChutes()
{
    Serial.println(F("DEPLOYING CHUTES"));
    servo.write(-1);
}

void idle()
{
    //
}

void armed()
{
    //Nastavi barometer reference poshort
    if (!barometerReferenceSet)
    {
        barometerReferenceSet = true;
        _referencePressure = barometer.readPressure() / 100;
    }

    updateSensorValues();

    //Prestopi v POWERED FLIGHT
}

void poweredFlight()
{

    updateSensorValues();

    //Prestopi v UNPOWERED FLIGHT
}

void unpoweredFlight()
{

    updateSensorValues();

    //Prestopi v BALLISTIC DESCENT
    //Pogoj: visina je manj kot pred 1s

    if (altitude < -1)
        stage = 4;
}

void ballisticDescent()
{

    updateSensorValues();

    //Prestopi v CHTUE DESCENT
    //Pogoj: visina je manj kot nek treshold
    if (altitude <= -1)
    {
        deployChutes();
        stage = 5;
    }
}

void chuteDescent()
{

    updateSensorValues();

    //Prestopi v CHTUE DESCENT
    //Pogoj: visina je manj kot nek treshold
    if (altitude <= -1)
    {
        f.close();
        stage = 6;
    }
}

void landed()
{
    ringPiezo();
}

void setup()
{
    Serial.begin(9600);
    Serial.println(F("DEVICE BOOTED"));
    //ciganQueueSetup();

    barometerSetup();
    SD_Setup();

    MCUSR = 0; //za soft reboot

    servo.attach(-1);
    servo.write(-1);

    Serial.println(F("name"));
    while (!(Serial.available() > 0))
    {
        //caka na input
    }
    
    String s = Serial.readString();
    s.toLowerCase();
    s.trim();
    FILENAME = s + ".txt";

    Serial.println(FILENAME);

    f = SD.open(FILENAME, FILE_WRITE);
    if (f)
    {
        Serial.print(F("Writing to file..."));
        f.println("t,s,p,a");
        // close the file:
        f.close();
        Serial.println(F("done."));
    }
    else
    {
        // if the file didn't open, print an error:
        Serial.println(F("error opening file"));
    }

    Serial.println(F("#################"));
}

void loop()
{
    incrementTime();
    updateSensorValues();
    readFromSerial();
    
    if (verboseLogging)
        writeToSerial();
        
    writeOnSd();

    switch (stage)
    {
    case 0:
        idle();
        break;
    case 1:
        armed();
        break;
    case 2:
        poweredFlight();
        break;
    case 3:
        unpoweredFlight();
        break;
    case 4:
        ballisticDescent();
        break;
    case 5:
        deployChutes();
        break;
    case 6:
        landed();
        break;
    }
    delay(DELAY_shortERVAL);
}
