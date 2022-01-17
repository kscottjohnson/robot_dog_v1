#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <bluefruit.h>
#include <Adafruit_PWMServoDriver.h>
#include "LegServo.h"

// Servos
Adafruit_PWMServoDriver driverBack = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver driverFront = Adafruit_PWMServoDriver(0x41);
#define SERVO_FREQ 50

LegServo servos[12] = {  // 90 degrees 220 to 420; 
  LegServo(&driverFront, 0, 325, 90), // Front Right
  LegServo(&driverFront, 1, 253, 45),
  LegServo(&driverFront, 2, 310, 90),
  LegServo(&driverFront, 4, 330, 90), // Front Left
  LegServo(&driverFront, 5, 370, 135),
  LegServo(&driverFront, 6, 310, 90),
  LegServo(&driverBack,  0, 305, 90), // Back Right
  LegServo(&driverBack,  1, 263, 45),
  LegServo(&driverBack,  2, 320, 90),
  LegServo(&driverBack,  4, 325, 90), // Back Left
  LegServo(&driverBack,  5, 387, 135),
  LegServo(&driverBack,  6, 330, 90)
};

Leg legs[4] = {
  Leg(&servos[0], &servos[1], &servos[2], true), // Front Right
  Leg(&servos[3], &servos[4], &servos[5], false), // Front Left
  Leg(&servos[6], &servos[7], &servos[8], true), // Back Right
  Leg(&servos[9], &servos[10], &servos[11], false) // Back Left
};

#define FRONT_RIGHT 0
#define FRONT_LEFT 1
#define BACK_RIGHT 2
#define BACK_LEFT 3

// Bluetooth
BLEUart bleuart;
uint8_t controlMsg[6] = {128,128,128,128,0,0};
uint8_t robotMsg[1] = {0};

// LCD
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define BATT_PIN A6

uint8_t mode = 0; // 0 demo, 1 static walking, 2 dynamic walking

unsigned long currentMs;
unsigned long prevMs;
unsigned long connectedMs;

uint8_t rx, ry, lx, ly;
bool buttonA, buttonB, buttonC, buttonR, buttonL, buttonAR, buttonBR, buttonCR;

uint8_t robotBatt;

float hipAngle[4];
float shoulderAngle[4];
float kneeAngle[4];

#define FEMUR 40
#define CLOCK_CYCLE 5

void setup() {
  Serial.begin(115200);
  Serial.println("rdv1");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  driverFront.begin();
  driverBack.begin();
  driverFront.setPWMFreq(SERVO_FREQ);
  driverBack.setPWMFreq(SERVO_FREQ);
  
  //setupDisplay();

  delay(500);
  centerServos();
  
  startBleAdv();

  //robotBatt = readBatt();
  
  prevMs = millis();
}

void loop() {
  currentMs = millis();
  if((currentMs - prevMs) < CLOCK_CYCLE) return;
  prevMs = currentMs;

  if(! Bluefruit.Periph.connected()){
    Serial.println("Not Connected");
    centerServos();
    return;
  }
  if((currentMs - connectedMs) < 1000){ // wait a second after connecting
    return;
  }
  Serial.print("Mode "); Serial.print(mode);
  if(mode == 0) moveDemo();
  else if(mode == 1) moveStaticWalk();
  else if(mode == 2) moveDynamicWalk();

  //robotBatt = smooth(robotBatt, readBatt(), 10);
  //getTelemetry();
  //refreshDisplay();
  //sendTelemetry();
}
