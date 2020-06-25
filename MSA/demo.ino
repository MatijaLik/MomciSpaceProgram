#include <Barometer.h>
#include <Gyros.h>
#include <SocialniDemokrati.h>

enum Stage
{
    IDLE,
    ARMED,
    POWERED_FLIGHT,
    UNPOWERED_FLIGHT,
    BALLISTIC_DESCENT,
    CHUTE_DESCENT,
    LANDED
};

const int OPERATING_FREQUENCY = 20; //Refresh rate v hercih

const int LED_RED = -1;
const int LED_GREEN = -1;
const int LED_BLUE = -1;

const int PIEZO_PIN = -1;
const int CS_PIN() = -1;
const int BUTTON_PIN = -1;

double time = -1;
Stage stage = Stage.IDLE;
double pressure;
double altitude;
double velocity;

double pitch;
double yaw;

double AcX;
double AcY;
double AcZ;

double GyX;
double GyY;
double GyZ;

Barometer barometer();
Gyros gyros();
SocialniDemokrati socialniDemokrati(CS_PIN);

void pinSetup()
{
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREE, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(PIEZO_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
}

void toggleLED(bool r, bool g, bool b)
{
    if (r)
        digitalWrite(LED_RED, HIGH);
    else
        digitalWrite(LED_RED, LOW);

    if (g)
        digitalWrite(LED_GREEN, HIGH);
    else
        digitalWrite(LED_GREEN, LOW);

    if (b)
        digitalWrite(LED_BLUE, HIGH);
    else
        digitalWrite(LED_BLUE, LOW);
    return;
}

void ringPiezo()
{
    tone(PIEZO_PIN, 1000); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    noTone(PIEZO_PIN);     // Stop sound...
    delay(1000);
}

double getVelocity(){
    //vso sreco
}

void updateSensorValues(){
    //Barometer
    altitude = barometer.getRelAltitude();
    pressure = barometer.getPressure();

    //Gyros
    
}

void setup()
{
    Serial.begin(9600);
    pinSetup();
}

void loop()
{
}