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
    Leg(LegServo* hipServo, LegServo* shoulderServo, LegServo* kneeServo, bool invertHip, bool invertKnee, bool reverseLeg);
    LegServo* hip;
    LegServo* shoulder;
    LegServo* knee;
    void setDefault();
    void setAngles(float h, float s, float k);
    bool isReversed();
  private:
    bool _invertHip;
    bool _invertKnee;
    bool _reverseLeg;
};

Leg::Leg(LegServo* hipServo, LegServo* shoulderServo, LegServo* kneeServo, bool invertHip, bool invertKnee, bool reverseLeg){
  hip = hipServo;
  shoulder = shoulderServo;
  knee = kneeServo;
  _invertHip = invertHip;
  _invertKnee = invertKnee;
  _reverseLeg = reverseLeg;
}

void Leg::setDefault(){
  this->hip->setDefault();
  this->shoulder->setDefault();
  this->knee->setDefault();
}

void Leg::setAngles(float h, float s, float k){
  if(this->_invertHip){
    this->hip->setAngle(180-h);
  }
  else{
    this->hip->setAngle(h); 
  }
  if(this->_invertKnee){
    this->shoulder->setAngle(180-s);
    this->knee->setAngle(180-k);
  }
  else{
    this->shoulder->setAngle(s);
    this->knee->setAngle(k);
  }
}

bool Leg::isReversed() {
  return this->_reverseLeg;
}

#endif
