#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <bluefruit.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>
#include <PID_v1.h>
#include "LegServo.h"

// Servos
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver(0x40);
#define SERVO_FREQ 50

LegServo servos[12] = {  // 90 degrees 220 to 420; 
  LegServo(&driver,  0, 300, 90), // Front Right
  LegServo(&driver,  1, 243, 45),
  LegServo(&driver,  2, 273, 90),
  LegServo(&driver,  8, 298, 90), // Front Left
  LegServo(&driver,  9, 333, 135),
  LegServo(&driver, 10, 300, 90),
  LegServo(&driver,  4, 306, 90), // Back Right
  LegServo(&driver,  5, 360, 135),
  LegServo(&driver,  6, 300, 90),
  LegServo(&driver, 12, 310, 90), // Back Left
  LegServo(&driver, 13, 242, 45),
  LegServo(&driver, 14, 320, 90)
};

Leg legs[4] = {
  Leg(&servos[0], &servos[1], &servos[2], false, true, false), // Front Right
  Leg(&servos[3], &servos[4], &servos[5], false, false, false), // Front Left
  Leg(&servos[6], &servos[7], &servos[8], true, false, true), // Back Right
  Leg(&servos[9], &servos[10], &servos[11], true, true, true) // Back Left
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

// IMU
Adafruit_Sensor_Calibration_SDFat cal;
Adafruit_LSM6DS33 imu;
Adafruit_LIS3MDL mdl;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
sensors_event_t mag; 
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;
float mag_x, mag_y, mag_z;

// filter
Adafruit_NXPSensorFusion filter;
#define FILTER_UPDATE_RATE_HZ 100
float ahrs_r, ahrs_p, ahrs_y;

// PID x adjustment
const double pid1_p = 1, pid1_i = 0, pid1_d = 0;
double pid1_in, pid1_out, pid1_set;
PID pid1(&pid1_in, &pid1_out, &pid1_set, pid1_p, pid1_i, pid1_d, DIRECT);

#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define BATT_PIN A6

uint8_t mode = 0; // 0 demo, 1 static walking, 2 dynamic walking

unsigned long currentMs;
unsigned long prevMs;
unsigned long connectedMs;
unsigned long filterMs;

uint8_t rx, ry, lx, ly;
bool buttonA, buttonB, buttonC, buttonR, buttonL, buttonAR, buttonBR, buttonCR;

uint8_t robotBatt;

float hipAngle[4];
float shoulderAngle[4];
float kneeAngle[4];

#define CHASSIS_L 114
#define CHASSIS_W 62
#define FEMUR 40
#define CLOCK_CYCLE 10

void setup() {
  Serial.begin(115200);
  while (!Serial) yield();
  Serial.println("rdv1");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  setupIMU();

  filter.begin(FILTER_UPDATE_RATE_HZ);

  setupPID();

  // start servos
  driver.begin();
  driver.setPWMFreq(SERVO_FREQ);
  
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

  getIMUdata();

  if((currentMs - filterMs) > (1000 / FILTER_UPDATE_RATE_HZ)){
    updateFilter();
    filterMs = currentMs;
  }
  
  //Serial.print("Mode "); Serial.print(mode);
  if(mode == 0) moveDemo();
  else if(mode == 1) moveStaticWalk();
  else if(mode == 2) legDemo();

  //robotBatt = smooth(robotBatt, readBatt(), 10);
  //getRemoteTelemetry();
  //refreshDisplay();
  //sendRemoteTelemetry();
}
