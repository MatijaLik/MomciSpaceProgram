#include <Servo.h>

#include <Barometer.h>
#include <Gyros.h>
#include <SocialniDemokrati.h>

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
const int OPERATING_FREQUENCY = 20; //Refresh rate v hercih
double TIME_INTERVAL = 1.0 / OPERATING_FREQUENCY;
const int DELAY_INTERVAL = 0;

const int LED_RED = -1;   //digitalni
const int LED_GREEN = -1; //digitalni
const int LED_BLUE = -1;  //digitalni

const int PIEZO_PIN = -1;  //mora imeti PWM
const int CS_PIN() = -1;   // googlaj
const int BUTTON_PIN = -1; //naj bo digitalni

const int SERVO_PIN = -1;         //PWM, verjetno
const int SERVO_DEPLOY_ANGLE = 0; //vrednost med 0 - 180
const int SERVO_LOCKED_ANGLE = 180;

const int LIFTOFF_TRESHOLD = -1;//stage shifting
const int BURNOUT_TRESHOLD = -1;

//SETTING END

double time = -1;
int stage = 0;
double pressure;

double altitude;
double ciganQueue[OPERATING_FREQUENCY];

double LAvelocity = 0;
double Vvelocity = 0;

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
Servo servo;

int idleButtonWait = 0;
bool barometerReferenceSet = false;

void pinSetup()
{
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREE, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(PIEZO_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);
}

void ciganQueueSetup()
{
    for (int i = 0; i < OPERATING_FREQUENCY; i++)
        ciganQueue[i] = 0.0;
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
    delay(1000);           // ...for 1 sec
    noTone(PIEZO_PIN);     // Stop sound...
    delay(1000);
}

void ciganPushFront(doube value)
{
    /*
    Uglaunem Arduino ne podpira queue strukture
    Zato pripravimo array, ki se obnasa kot queue.
    Pri push front pa vsaki element rocno prestavimo za eno naprej
    */
    for (int i = OPERATING_FREQUENCY - 1; i > 0)
        ciganQueue[i] = ciganQueue[i - 1];
    ciganQueue[0] = value;
}

void incrementTime()
{
    //poveca cas za nek interval in ga zaokrozi na 2 decimalki

    double nextTime = time + TIME_INTERVAL;
    nextTime = floor(nextTime * 100);
    nextTime /= 100;
    time = nextTime;
}

void updateSensorValues()
{
    //Barometer
    altitude = barometer.getRelAltitude();
    pressure = barometer.getPressure();

    //Gyros
    gyros.refresh();
    AcX = gyros.AcX;
    Acy = gyros.AcY;
    AcZ = gyros.AcZl
              GyX = gyros.GyX;
    GyY = gyros.GyY;
    GyZ = gyros.GyZ;
    pitch = gyros.getPitch();
    roll = gyros.getRoll();

    //velocity je integral AcZ po dt
    LAvelocity += AcZ * TIME_INTERVAL;

    //odvod visine, izpred ene sekunde
    Vvelocity = altitude - ciganQueue[OPERATING_FREQUENCY - 1];
    ciganPushFront(altitude);
}

void writeOnSd()
{
    socialniDemokrati.logData(
        time,
        stageToString[stage],
        pressure,
        altitude,
        Vvelocity,
        LAvelocity,
        pitch,
        yaw,
        AcX,
        AcY,
        AcZ,
        GyX,
        GyY,
        GyZ)
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

void armed(){
    //Utripa rdeca
    if(floor(time)%2) toggleLED(1, 0, 0);
    else toggleLED(0,0,0);

    //Nastavi barometer reference point
    if(!barometerReferenceSet){
        barometerReferenceSet = true;
        barometer.setPressureReference();
    }

    updateSensorValues();
    writeOnSd();

    //Prestopi v POWERED FLIGHT
    if(AcZ >= LIFTOFF_TRESHOLD) stage = 2;
}

void setup()
{
    Serial.begin(9600);
    pinSetup();
    ciganQueueSetup();
    toggleLED(0, 0, 0);

    servo.attach(SERVO_PIN);
    servo.write(SERVO_LOCKED_ANGLE);
}

void loop()
{
    incrementTime();
    switch (stage)
    {
    case 0:
        idle();
        break;
    case 1:
        armed();
        break;
    default:

        landed();
    }
    delayMicroseconds(DELAY_INTERVAL);
}