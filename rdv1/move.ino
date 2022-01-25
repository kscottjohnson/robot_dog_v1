void moveLeg(int legNum, float x, float y, float z, float roll, float pitch, float yaw) {
  Leg* leg = &legs[legNum]; 
  //Serial.print("Leg "); Serial.println(legNum);

  if(leg->isReversed()) { // flip y if leg is reversed
    y = -1 * y;
  }

  // roll adjust h' = CHASSIS_W / 2 * sin(roll)
  float rollZAdj = CHASSIS_W / 2.0 * sin(roll * DEG_TO_RAD);
  if(legNum == 0 || legNum == 2){ 
    z += rollZAdj;
  }
  else{
    z -= rollZAdj;
  }

  // get leg length on X plane
  float legLenX = sqrt(x*x+z*z);

  // get hip rotation
  float hipAngle = 90;
  if(x != 0){ // check for zero x and set angle otherwise
    hipAngle = acos((legLenX*legLenX+x*x-z*z)/(2*x*legLenX))*RAD_TO_DEG;
  }
  hipAngle -= roll;

  // get leg length on Y plane
  float legLenY = sqrt(y*y+legLenX*legLenX);

  // get shoulder rotation due to y
  float s1 = 90;
  if(y != 0){
    s1 = acos((legLenY*legLenY+y*y-legLenX*legLenX)/(2*y*legLenY)) * RAD_TO_DEG;
  }

  // shoulder and knee given Y leg length
  float s2 = acos((legLenY*legLenY)/(2*FEMUR*legLenY)) * RAD_TO_DEG;
  float kneeAngle = 180 - (180 - 2 * s2);

  float shoulderAngle = s1+s2;

  //Serial.print("Angles: ");
  //Serial.print(hipAngle); Serial.print(" ");
  //Serial.print(shoulderAngle); Serial.print(" ");
  //Serial.println(kneeAngle); 
  
  leg->setAngles(hipAngle, shoulderAngle, kneeAngle);
}

void centerServos() {
  legs[FRONT_RIGHT].setDefault();
  legs[FRONT_LEFT].setDefault();
  legs[BACK_RIGHT].setDefault();
  legs[BACK_LEFT].setDefault();
}

void moveDemo(){
  
  Serial.println("--------Demo");

  //map left y axis to z hight
  Serial.print(ly); Serial.print(" ");
  float newZ = map(ly, 0, 255, 36, 75);
  Serial.println(newZ);

  //map left x axis to y forward/backward
  Serial.print(lx); Serial.print(" ");
  float newY = map(lx, 0, 255, 15, -15);
  Serial.println(newY);

  //map right x axis to roll
  Serial.print(rx); Serial.print(" ");
  float newR = map(rx, 0, 255, 30, -30);
  Serial.println(newR);

  //map right y axis to pitch
  Serial.print(ry); Serial.print(" ");
  float newP = map(ry, 0, 255, 30, -30);
  Serial.println(newP);

  moveLeg(FRONT_RIGHT, 0, newY, newZ, newR, newP, 0);
  moveLeg(FRONT_LEFT , 0, newY, newZ, newR, newP, 0);
  moveLeg(BACK_RIGHT , 0, newY, newZ, newR, newP, 0);
  moveLeg(BACK_LEFT  , 0, newY, newZ, newR, newP, 0);
}

// static walking variables
//const int8_t sWalkY[] = {-21,  0, 21, 14,  7,  0, -7,-14, -21,  0, 21, 14,  7,  0, -7,-14};
//const int8_t sWalkZ[] = { 56, 52, 56, 56, 56, 56, 56, 56,  56, 52, 56, 56, 56, 56, 56, 56};

const int8_t sWalkX[] = {  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0};
const int8_t sWalkY[] = {-15,  0, 15, 10,  5,  0, -5,-10, -15,  0, 15, 10,  5,  0, -5,-10};
const int8_t sWalkZ[] = { 56, 52, 56, 56, 56, 56, 56, 56,  56, 52, 56, 56, 56, 56, 56, 56};
const int8_t sMaxSpeed = 4;
int8_t sWalkSpeed; // cycles per second
int8_t sTicksPerState;
int8_t sTick = 0;
int8_t sWalkState;
float sCurrentX[4], sCurrentY[4], sCurrentZ[4];
float    sPrevX[4],    sPrevY[4],    sPrevZ[4];
const uint8_t legOrder[4] = {2,0,3,1};
bool sFirstStep = true;
int8_t sDirection = 1; // 1 forward, -1 reverse

void moveStaticWalk() {
  if(sTick == 0){ // update walk speed at start of cycle
    Serial.println("--------Static Walk");
    //Serial.print(ly); Serial.print(" ");
    sWalkSpeed = map(ly, 0, 255, -1 * sMaxSpeed, sMaxSpeed);
    //Serial.print(sWalkSpeed); Serial.print(" ");
    if(sWalkSpeed != 0){
      sTicksPerState =  (float) sMaxSpeed / abs(sWalkSpeed) * 16;
      if(sWalkSpeed > 0){
        sDirection = 1;
      }
      else {
        sDirection = -1;
      }
    }
    else {
      sTicksPerState = 1;
    }
    //Serial.println(sTicksPerState);
  }

  if(sWalkSpeed == 0){ // stopped
    Serial.println("Stopped");
    sWalkState = 0;
    for(int l=0; l<4; l++){
      sCurrentX[l] = 0;
      sCurrentY[l] = 0;
      sCurrentZ[l] = 56;
      sPrevX[l] = 0;
      sPrevY[l] = 0;
      sPrevZ[l] = 56;
      moveLeg(l, 0, 0, 56, 0, 0, 0);
    }
    sTick = 0;
    sFirstStep = true;
  }
  else{ // moving
    Serial.print("Moving State: "); Serial.print(sWalkState); Serial.print(" Tick: "); Serial.print(sTick);
    Serial.print(" Speed: "); Serial.println(sWalkSpeed);
    for(int l=0; l<4; l++){
      int lState = sWalkState + legOrder[l]*2; //offset each leg through the states
      
      if(sTick + 1 == sTicksPerState){ // if this is the last tick just set the targets
        sCurrentX[l] = sWalkX[lState];
        sCurrentY[l] = sWalkY[lState];
        sCurrentZ[l] = sWalkZ[lState];
        sPrevX[l] = sCurrentX[l];
        sPrevY[l] = sCurrentY[l];
        sPrevZ[l] = sCurrentZ[l];
        if(sFirstStep && (lState < 5)){
          // hack to make non-lifting leg not go forward
          sCurrentY[l] = 0; 
          sPrevY[l] = 0;
        }
      }
      else{
        sCurrentX[l] += (sWalkX[lState] - sPrevX[l]) / sTicksPerState;
        sCurrentY[l] += (sWalkY[lState] - sPrevY[l]) / sTicksPerState;
        sCurrentZ[l] += (sWalkZ[lState] - sPrevZ[l]) / sTicksPerState;
        if(sFirstStep && (lState < 5)){
          // hack to make non-lifting leg not go forward
          sCurrentY[l] = 0; 
        }
      }
      Serial.print(sWalkX[lState]); Serial.print("/");
      Serial.print(sCurrentX[l]); Serial.print(" ");
      Serial.print(sWalkY[lState]); Serial.print("/");
      Serial.print(sCurrentY[l]); Serial.print(" ");
      Serial.print(sWalkZ[lState]); Serial.print("/");
      Serial.print(sCurrentZ[l]); Serial.println(" ");
      moveLeg(l, sCurrentX[l], sCurrentY[l], sCurrentZ[l], 0, 0, 0);
    }
    
    if(++sTick == sTicksPerState){ // new state
      sTick = 0;
      sFirstStep = false;
      sWalkState += sDirection;
      if(sWalkState == 8){ // new cycle
        sWalkState = 0;
      }
      else if(sWalkState == -1){ // new cycle
        sWalkState = 7;
      }
    }
  }
}

void moveDynamicWalk() {
  Serial.println("--------Dynamic Walk");
}


void legDemo(){
  
  Serial.println("--------Leg Demo");

  float height = map(ly, 0, 255, 67, 47);
  moveLeg(FRONT_LEFT, 0, 0, height, 0, 0, 0);

  height = map(ry, 0, 255, 67, 47);
  moveLeg(FRONT_RIGHT, 0, 0, height, 0, 0, 0);

  height = map(lx, 0, 255, 67, 47);
  moveLeg(BACK_LEFT, 0, 0, height, 0, 0, 0);

  height = map(rx, 0, 255, 67, 47);
  moveLeg(BACK_RIGHT, 0, 0, height, 0, 0, 0);
  
  
}
