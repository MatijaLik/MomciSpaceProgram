#include <Servo.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SD.h>
#include <avr/wdt.h>

//#include <SocialniDemokrati.h>

String stageToString[] = {
    "IDLE",
    "ARMED",
    "POWERED FLIGHT",
    "UNPOWERED FLIGHT",
    "BALLISTIC DESCENT",
    "CHUTE DESCENT",
    "LANDED",
};

//SETTING BEGIN

#define BMP_SCK (13)
#define BMP_MISO (8)
#define BMP_MOSI (11)
#define BMP_CS (10)

const int OPERATING_FREQUENCY = 1; //Refresh rate v hercih
double TIME_INTERVAL = 1.0 / OPERATING_FREQUENCY;
const int DELAY_INTERVAL = TIME_INTERVAL * 1000;

const int LED_RED = -1;   //digitalni
const int LED_GREEN = -1; //digitalni
const int LED_BLUE = -1;  //digitalni

const int PIEZO_PIN = -1;  //mora imeti PWM
const int CS_PIN = -1;     // googlaj
const int BUTTON_PIN = -1; //naj bo digitalni

const int SERVO_PIN = -1;         //PWM, verjetno
const int SERVO_DEPLOY_ANGLE = 0; //vrednost med 0 - 180
const int SERVO_LOCKED_ANGLE = 180;

//Znajdi se
double unitMatrix[3][3] =
    {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}};

//SETTING END

double pitchMatrix[3][3];
double yawMatrix[3][3];
bool mpuCalibrated = false;

double cas = 0;
int stage = 0;
double pressure;

double altitude;
double ciganQueue[OPERATING_FREQUENCY + 1];

double acc[3];

bool verboseLogging = 1;

//Kota sta v radianih
double pitch;
double yaw;

String FILENAME;

Adafruit_BMP280 barometer(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK);
Adafruit_MPU6050 gyros;
sensors_event_t a, g, temp;
//SocialniDemokrati socialniDemokrati(CS_PIN);
Servo servo;
File f;

bool barometerReferenceSet = false;


void barometerSetup()
{
    if (!barometer.begin())
    {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1)
            ;
    }

    Serial.println("Barometer initialized");
    /* Default settings from datasheet. */
    barometer.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                          Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                          Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                          Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                          Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void gyrosSetup()
{
    // Try to initialize!
    Serial.println("G1");
    if (!gyros.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }

    gyros.setAccelerometerRange(MPU6050_RANGE_16_G);
    gyros.setGyroRange(MPU6050_RANGE_250_DEG);
    gyros.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void SD_Setup(){
    pinMode(SD_CS, OUTPUT);
    if (!SD.begin(SD_CS))
    {
        Serial.println("Card failed");
        // don't do anything more:
        while(1){};
        return;
    }

    String s = "Time,Stage,Air pressure,Altitude,Pitch,Yaw,AcX,AcY,AcZ";

    Serial.println("Card gud");
    f = SD.open(FILENAME, FILE_WRITE);
    f.println(s);
}

double getPitch()
{

    double x = acc[0];
    double y = acc[1];
    double z = acc[2];

    double _pitch = atan(x / sqrt((y * y) + (z * z))); //pitch calculation
    return _pitch;
}

double getYaw()
{
    double x = acc[0];
    double y = acc[1];
    double z = acc[2];
    double _yaw = atan(y / sqrt((x * x) + (z * z))); //roll calculation
    return _yaw;
}

void linearTransormation(double (&m)[3][3], double (&vector)[3])
{
    double v[] = {vector[0], vector[1], vector[2]};

    vector[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2];
    vector[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2];
    vector[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2];
}

void resetMPUCalibration()
{
    pitch = 0;
    yaw = 0;
    calibrateMPU();
}

void calibrateMPU()
{
    mpuCalibrated = true;

    //pitch matrix
    pitchMatrix[0][0] = cos(pitch);
    pitchMatrix[0][1] = 0;
    pitchMatrix[0][2] = -sin(pitch);
    pitchMatrix[1][0] = 0;
    pitchMatrix[1][1] = 1;
    pitchMatrix[1][2] = 0;
    pitchMatrix[2][0] = sin(pitch);
    pitchMatrix[2][1] = 0;
    pitchMatrix[2][2] = cos(pitch);

    //yaw matrix
    yawMatrix[0][0] = 1;
    yawMatrix[0][1] = 0;
    yawMatrix[0][2] = 0;
    yawMatrix[1][0] = 0;
    yawMatrix[1][1] = cos(yaw);
    yawMatrix[1][2] = -sin(yaw);
    yawMatrix[2][0] = 0;
    yawMatrix[2][1] = sin(yaw);
    yawMatrix[2][2] = cos(yaw);
}


void ciganQueueSetup()
{
    for (int i = 0; i < OPERATING_FREQUENCY + 1; i++)
        ciganQueue[i] = 0.0;
}
void ciganPushFront(double value)
{

    if (g)
        digitalWrite(LED_GREEN, HIGH);
    else
        digitalWrite(LED_GREEN, LOW);

    for (int i = OPERATING_FREQUENCY; i > 0; i--)
    {
        ciganQueue[i] = ciganQueue[i - 1];
}

    ciganQueue[0] = value;
}


void ringPiezo()
{
    tone(PIEZO_PIN, 1000); // Send 1KHz sound signal...
    delay(1000);           // ...for 1 sec
    noTone(PIEZO_PIN);     // Stop sound...
    delay(1000);
}



void incrementTime()
{
    //poveca cas za nek interval in ga zaokrozi na 2 decimalki

    double nextTime = cas + TIME_INTERVAL;
    nextTime = floor(nextTime * 100);
    nextTime /= 100;
    cas = nextTime;
}

void updateSensorValues()
{
    //Barometer
    altitude = barometer.readAltitude(_referencePressure);
    pressure = barometer.readPressure();

    //Gyros
    gyros.getEvent(&a, &g, &temp);
    acc[0] = a.acceleration.x;
    acc[1] = a.acceleration.y;
    acc[2] = a.acceleration.z;
    gyro[0] = g.gyro.x;
    gyro[1] = g.gyro.y;
    gyro[2] = g.gyro.z;

    linearTransormation(unitMatrix, acc);
    linearTransormation(unitMatrix, gyro);
    if (mpuCalibrated)
    {
        linearTransormation(pitchMatrix, acc);
        linearTransormation(yawMatrix, acc);
        linearTransormation(pitchMatrix, gyro);
        linearTransormation(yawMatrix, gyro);
    }

    pitch = getPitch();
    yaw = getYaw();

    ciganPushFront(altitude);
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
            Serial.println("Manually setting reference pressure");
            barometerReferenceSet = true;
            _referencePressure = barometer.readPressure() / 100;
        }
        else if (s == "oref")
        {
            Serial.println("Manually setting reference orientation");
            calibrateMPU();
        }
        else if (s == "orefreset")
        {
            Serial.println("Reseting reference orientation");
            calibrateMPU();
        }
        else if (s == "startup")
        {
            Serial.println("Vehicle is now in startup");
            stage = 1;
        }
        else if (s == "stage")
        {
            Serial.println("Staging");
            stage++;
        }
        else if (s == "c")
            deployChutes();
        else if (s == "chutes")
            deployChutes();
        else if (s == "shut")
            verboseLogging = 0;
        else if (s == "speak")
            verboseLogging = 1;
        else if (s == "reboot")
        {
            f.close();
            Serial.println("Rebooting");
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
        int incomingByte = Serial.read();
    }
}

void printMatrix(double (&m)[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Serial.print(m[i][j]);
            Serial.print(" ");
        }
        Serial.println();
    }
}

void writeToSerial()
{
    Serial.print("STAGE: ");
    Serial.println(stageToString[stage]);
    Serial.print("Time: ");
    Serial.println(cas);
    Serial.println();

    Serial.print("Altitude: ");
    Serial.println(altitude);
    Serial.print("Pressure: ");
    Serial.println(pressure);
    Serial.println();

    Serial.print("Acceleration: ");
    Serial.print(acc[0]);
    Serial.print(",");
    Serial.print(acc[1]);
    Serial.print(",");
    Serial.print(acc[2]);
    Serial.println();

    Serial.print("Gyroscope: ");
    Serial.print(gyro[0]);
    Serial.print(",");
    Serial.print(gyro[1]);
    Serial.print(",");
    Serial.print(gyro[2]);
    Serial.println();

    Serial.print("Pitch: ");
    Serial.println(pitch * 180 / M_PI);
    Serial.print("Yaw: ");
    Serial.println(yaw * 180 / M_PI);
    Serial.println();

    /*
    Serial.println("Unit Matrix");
    printMatrix(unitMatrix);
    Serial.println();

    Serial.println("Pitch Matrix");
    printMatrix(pitchMatrix);
    Serial.println();

    Serial.println("Yaw Matrix");
    printMatrix(yawMatrix);
    Serial.println();
    */

    Serial.println("#############");
}

String _appendToString(String s, double i)
{
    return s + String(i) + ",";
}

void writeOnSd()
{
    /*
    socialniDemokrati.logData(
        time,
        stageToString[stage],
        pressure,
        altitude,
        Vvelocity,
        LAvelocity,
        pitch,
        yaw,
        acc[0],
        acc[1],
        acc[2],
        gyro[0],
        gyro[1],
        gyro[2]);*/

    String s;
    _appendToString(s, cas);
    s += stageToString[stage] + ",";
    s = _appendToString(s, pressure);
    s = _appendToString(s, altitude);

    s = _appendToString(s, pitch);
    s = _appendToString(s, yaw);

    s = _appendToString(s, acc[0]);
    s = _appendToString(s, acc[1]);
    s = _appendToString(s, acc[2]);

    f.println(s);
}

void deployChutes()
{
    Serial.println("DEPLOYING CHUTES");
    servo.write(SERVO_DEPLOY_ANGLE);
}

void idle()
{
    toggleLED(0, 0, 1);

    //pocaka, da okoli 3s drzimo button
    if (digitalRead(BUTTON_PIN) == HIGH)
    {
        idleButtonWait++;
        delay(1000);
    }
    else
        idleButtonWait = 0;

    //Prestopi v ARMED
    if (idleButtonWait >= 3)
        stage = 1;
}

void armed()
{
    //Utripa rdeca
    if (int(cas) % 2)
        toggleLED(1, 0, 0);
    else
        toggleLED(0, 0, 0);

    //Nastavi barometer reference point
    if (!barometerReferenceSet)
    {
        barometerReferenceSet = true;
        _referencePressure = barometer.readPressure() / 100;
    }

    if (!mpuCalibrated)
        calibrateMPU();

    updateSensorValues();
    writeOnSd();

    //Prestopi v POWERED FLIGHT
    if (acc[2] >= LIFTOFF_TRESHOLD)
        stage = 2;
}

void poweredFlight()
{
    toggleLED(0, 1, 1); //cyan

    updateSensorValues();
    writeOnSd();

    //Prestopi v UNPOWERED FLIGHT
    if (acc[2] <= BURNOUT_TRESHOLD)
        stage = 3;
}

void unpoweredFlight()
{
    toggleLED(1, 0, 1); //purple

    updateSensorValues();
    writeOnSd();

    //Prestopi v BALLISTIC DESCENT
    //Pogoj: visina je manj kot pred 1s
    
    if (altitude < ciganQueue[OPERATING_FREQUENCY])
        stage = 4;
}

void ballisticDescent()
{
    toggleLED(1, 0, 0); //rdeca

    updateSensorValues();
    writeOnSd();

    //Prestopi v CHTUE DESCENT
    //Pogoj: visina je manj kot nek treshold
    if (altitude <= DEPLOY_ALTITUDE)
    {
        deployChutes();
        stage = 5;
    }
}

void chuteDescent()
{
    toggleLED(1, 1, 0); //rumena

    updateSensorValues();
    writeOnSd();

    //Prestopi v CHTUE DESCENT
    //Pogoj: visina je manj kot nek treshold
    if (altitude <= GROUND_ALTITUDE)
    {
        f.close();
        stage = 6;
    }
}

void landed()
{
    toggleLED(0, 1, 0); // zelena
    ringPiezo();
}

void setup()
{
    Serial.begin(9600);
    Serial.println("DEVICE BOOTED");

    pinSetup();
    ciganQueueSetup();
    toggleLED(0, 0, 0);

    ciganQueueSetup();
    gyrosSetup();
    barometerSetup();
    SD_Setup();

    MCUSR = 0; //za soft reboot

    servo.attach(SERVO_PIN);
    servo.write(SERVO_LOCKED_ANGLE);


    Serial.println("name");
    while (!Serial.available() > 0){
        //caka na input
    }
    String s = Serial.readString();
    s.toLowerCase();
    s.trim();
    FILENAME = s + ".csv";

    Serial.println(FILENAME);
    Serial.println("#################");
}

void loop()
{
    incrementTime();
    updateSensorValues();
    readFromSerial();
    if(verboseLogging) writeToSerial();
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
    delay(DELAY_INTERVAL);
}
