#ifndef LegServo_h
#define LegServo_h

class LegServo {
  public:
    LegServo(Adafruit_PWMServoDriver* driver, int num, int defaultPWM, 
      int defaultAngle);
    void setDefault();
    void setAngle(float ang);
  private:
    Adafruit_PWMServoDriver* _driver;
    int _num;
    int _defaultPWM;
    int _defaultAngle;
    void _setPWM(int pwm);
};

LegServo::LegServo(Adafruit_PWMServoDriver* driver, int num, int defaultPWM, 
  int defaultAngle){
  
  _driver = driver;
  _num = num;
  _defaultPWM = defaultPWM;
  _defaultAngle = defaultAngle;
}

void LegServo::_setPWM(int pwm){
  this->_driver->setPWM(this->_num, 0, pwm);
}

void LegServo::setDefault(){
  this->_setPWM(this->_defaultPWM);
}

void LegServo::setAngle(float ang){
  float pwm = (ang - this->_defaultAngle) * 2.2222 + this->_defaultPWM;
  //Serial.print(pwm); Serial.print(" ");
  this->_driver->setPWM(this->_num, 0, pwm);
}

class Leg {
  public:
    Leg(LegServo* hipServo, LegServo* shoulderServo, LegServo* kneeServo, bool inverted);
    LegServo* hip;
    LegServo* shoulder;
    LegServo* knee;
    void setDefault();
    void setAngles(float h, float s, float k);
  private:
    bool _inverted;
};

Leg::Leg(LegServo* hipServo, LegServo* shoulderServo, LegServo* kneeServo, bool inverted){
  hip = hipServo;
  shoulder = shoulderServo;
  knee = kneeServo;
  _inverted = inverted;
}

void Leg::setDefault(){
  this->hip->setDefault();
  this->shoulder->setDefault();
  this->knee->setDefault();
}

void Leg::setAngles(float h, float s, float k){
  if(this->_inverted){
    //Serial.println("Inverting 180-angle");
    this->hip->setAngle(h); // hip servos all face the same way
    this->shoulder->setAngle(180-s);
    this->knee->setAngle(180-k);
  }
  else{
    //Serial.println("Not Inverting");
    this->hip->setAngle(h);
    this->shoulder->setAngle(s);
    this->knee->setAngle(k);
  }
  //Serial.println();
}

#endif

//160 480
/*
driverBack.setPWM(3, 0, 320);
delay(500);
for(int p=320; p>=220; p-=10){
    Serial.println(p);
    driverBack.setPWM(3, 0, p);
    delay(200);
  }
for(int p=230; p<=420; p+=10){
    Serial.println(p);
    driverBack.setPWM(3, 0, p);
    delay(200);
  }
delay(500);
*/
//driverBack.setPWM(3, 0, 320);
//delay(500);
//driverBack.setPWM(3, 0, 480);
//delay(500);

/*
  for(int p=150; p<500; p+=10){
    Serial.println(p);
    driverBack.setPWM(3, 0, p);
    delay(500);
  }
*/
/*
  float testAngle = map(ly, 0, 255, 0, 180);
  Serial.println(testAngle);
  legs[BACK_LEFT].shoulder->setAngle(180);
  legs[BACK_LEFT].knee->setAngle(testAngle);
  legs[BACK_RIGHT].shoulder->setAngle(0);
  legs[BACK_RIGHT].knee->setAngle(testAngle);
*/
