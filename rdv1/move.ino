// Leg movement and walking algorithms

void moveLeg(int legNum, float x, float y, float z, float roll, float pitch, float yaw) {
  Leg* leg = &legs[legNum]; 
  //Serial.print("Leg "); Serial.println(legNum);

  // roll adjust 
  float rollZAdj = CHASSIS_W / 2.0 * sin(roll * DEG_TO_RAD);
  if(legNum == 0 || legNum == 2){ // right legs
    z += rollZAdj;
  }
  else{
    z -= rollZAdj;
  }

  // pitch adjust 
  float pitchZAdj = CHASSIS_L / 2.0 * sin(pitch * DEG_TO_RAD);
  //Serial.print(pitchZAdj);Serial.print(" ");
  if(legNum == 0 || legNum == 1){ // front legs
    z += pitchZAdj;
  }
  else {
    z -= pitchZAdj;
  }
  //Serial.println(z);

  if(leg->isReversed()) { // flip y and pitch if leg is reversed
    y *= -1;
    pitch *= -1;
  }
  
  // get leg length on X plane
  float legLenX = sqrt(x*x+z*z);

  // get hip rotation
  float hipAngle = 90;
  if(x != 0){ // check for zero x and set angle otherwise
    hipAngle = acos((legLenX*legLenX+x*x-z*z)/(2*x*legLenX))*RAD_TO_DEG;
  }
  hipAngle += roll; // add roll degrees

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

  //Serial.print(s1);Serial.print(" ");Serial.print(s2);Serial.print(" ");Serial.println(pitch);
  float shoulderAngle = s1+s2+pitch;

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
  
  //Serial.println("--------Demo");

  //map left y axis to z hight
  //Serial.print(ly); Serial.print(" ");
  float newZ = map(ly, 0, 255, 36, 75);
  //Serial.println(newZ);

  //map left x axis to y forward/backward
  //Serial.print(lx); Serial.print(" ");
  float newY = map(lx, 0, 255, 15, -15);
  //Serial.println(newY);

  float newR = 0;
  //map right x axis to roll
  //Serial.print(rx); Serial.print(" ");
  newR = map(rx, 0, 255, 30, -30);
  //Serial.println(newR);

  float newX = 0;
  //map right x axis to x adjust
  //Serial.print(rx); Serial.print(" ");
  //newX = map(rx, 0, 255, -10, 10);
  //Serial.println(newX);

  //map right y axis to pitch
  //Serial.print(ry); Serial.print(" ");
  float newP = map(ry, 0, 255, 20, -20);
  //Serial.println(newP);

  moveLeg(FRONT_RIGHT, newX, newY, newZ, newR, newP, 0);
  moveLeg(FRONT_LEFT , newX, newY, newZ, newR, newP, 0);
  moveLeg(BACK_RIGHT , newX, newY, newZ, newR, newP, 0);
  moveLeg(BACK_LEFT  , newX, newY, newZ, newR, newP, 0);
}

// static walk global vars
const int8_t sWalkX[] =       {  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0};
const int8_t sWalkY[] =       {-15,  0, 15, 10,  5,  0, -5,-10, -15,  0, 15, 10,  5,  0, -5,-10};
const int8_t sWalkZ[] =       { 56, 52, 56, 56, 56, 56, 56, 56,  56, 52, 56, 56, 56, 56, 56, 56};
const int8_t sWalkXright[] =  { -6,  0,  6,  4,  2,  0, -2, -4,  -6,  0,  6,  4,  2,  0, -2, -4};
const int8_t sWalkXleft[] =   {  6,  0, -6, -4, -2,  0,  2,  4,   6,  0, -6, -4, -2,  0,  2,  4};
const int8_t sMaxSpeed = 4;
int8_t sWalkSpeed; // cycles per second
int8_t sSteer; // 1=right, 0=none, -1=left
const float sSpinX[4][8] = {{-10.4,  0.0,  9.4,  6.4,  3.3,  0.0, -3.4, -6.8},
                            {  3.4,  0.0, -3.3, -6.4, -9.4,  0.0, 10.4,  6.8},
                            {-10.4, -6.8, -3.4,  0.0,  3.3,  6.4,  9.4,  0.0},
                            {  3.4,  6.8, 10.4,  0.0, -9.4, -6.4, -3.3,  0.0}};
const float sSpinY[4][8] = {{  4.5,  0.0, -6.2, -4.0, -1.9,  0.0,  1.7,  3.2},
                            {  1.7,  0.0, -1.9, -4.0, -6.2,  0.0,  4.5,  3.2},
                            { -4.5, -3.2, -1.7,  0.0,  1.9,  4.0,  6.2,  0.0},
                            { -1.7, -3.2, -4.5,  0.0,  6.2,  4.0,  1.9,  0.0}};
const float sSpinZ[4][8] = {{ 56.0, 52.0, 56.0, 56.0, 56.0, 56.0, 56.0, 56.0},
                            { 56.0, 56.0, 56.0, 56.0, 56.0, 52.0, 56.0, 56.0},
                            { 56.0, 56.0, 56.0, 56.0, 56.0, 56.0, 56.0, 52.0},
                            { 56.0, 56.0, 56.0, 52.0, 56.0, 56.0, 56.0, 56.0}};
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
    //Serial.print(rx); Serial.print(" ");
    sWalkSpeed = map(ly, 0, 255, -1 * sMaxSpeed, sMaxSpeed);
    sSteer = map(rx, 0, 230 , -1, 1);
    //Serial.println(sSteer);
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
      sTicksPerState = 16; // for stationary spin
    }
    //Serial.println(sTicksPerState);
  }

  if(sWalkSpeed == 0){ // stopped    
    if(sSteer == 0){ // no spin
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
    else{ // spinning
      Serial.print("Spin ");Serial.print(sSteer);Serial.print(" State "); Serial.print(sWalkState); Serial.print(" Tick "); Serial.print(sTick);Serial.print(" ");
      for(int l=0; l<4; l++){
        if(sTick + 1 == sTicksPerState){ // if this is the last tick just set the targets
          sCurrentX[l] = sSpinX[l][sWalkState];
          sCurrentY[l] = sSpinY[l][sWalkState];
          sCurrentZ[l] = sSpinZ[l][sWalkState];
          sPrevX[l] = sCurrentX[l];
          sPrevY[l] = sCurrentY[l];
          sPrevZ[l] = sCurrentZ[l];
        }
        else{
          sCurrentX[l] += (sSpinX[l][sWalkState] - sPrevX[l]) / sTicksPerState;
          sCurrentY[l] += (sSpinY[l][sWalkState] - sPrevY[l]) / sTicksPerState;
          sCurrentZ[l] += (sSpinZ[l][sWalkState] - sPrevZ[l]) / sTicksPerState;
        }
        
        moveLeg(l, sCurrentX[l], sCurrentY[l], sCurrentZ[l], 0, 0, 0);
        //Serial.print(sSpinX[l][sWalkState]); Serial.print("/");
        //Serial.print(sCurrentX[l]); Serial.print(" ");
        //Serial.print(sSpinY[l][sWalkState]); Serial.print("/");
        //Serial.print(sCurrentY[l]); Serial.print(" ");
        //Serial.print(sSpinZ[l][sWalkState]); Serial.print("/");
        //Serial.print(sCurrentZ[l]); Serial.println(" ");
      }
      
      if(++sTick == sTicksPerState){ // new state
        sTick = 0;
        sFirstStep = false;
        sWalkState += sSteer;
        if(sWalkState == 8){ // new cycle
          sWalkState = 0;
        }
        else if(sWalkState == -1){ // new cycle
          sWalkState = 7;
        }
      }
    }
  }
  else{ // moving
    Serial.print("Moving State: "); Serial.print(sWalkState); Serial.print(" Tick: "); Serial.print(sTick);
    Serial.print(" Speed: "); Serial.println(sWalkSpeed);
    for(int l=0; l<4; l++){
      int lState = sWalkState + legOrder[l]*2; //offset each leg through the states

      const int8_t* sWalkXdyanmic = sWalkX;
      if(sSteer == 1){ // turning right
        if(l == 0 || l == 1){ // front legs right
          sWalkXdyanmic = sWalkXright;
        }
        else{ // back legs left
          sWalkXdyanmic = sWalkXleft;
        }
      }
      else if(sSteer == -1){ // turning left
        if(l == 0 || l == 1){ // front legs left
          sWalkXdyanmic = sWalkXleft;
        }
        else{ // back legs right
          sWalkXdyanmic = sWalkXright;
        }
      }
      
      if(sTick + 1 == sTicksPerState){ // if this is the last tick just set the targets
        sCurrentX[l] = sWalkXdyanmic[lState];
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
        sCurrentX[l] += (sWalkXdyanmic[lState] - sPrevX[l]) / sTicksPerState;
        sCurrentY[l] += (sWalkY[lState] - sPrevY[l]) / sTicksPerState;
        sCurrentZ[l] += (sWalkZ[lState] - sPrevZ[l]) / sTicksPerState;
        if(sFirstStep && (lState < 5)){
          // hack to make non-lifting leg not go forward
          sCurrentY[l] = 0; 
        }
      }
      //Serial.print(sWalkXdyanmic[lState]); Serial.print("/");
      //Serial.print(sCurrentX[l]); Serial.print(" ");
      //Serial.print(sWalkY[lState]); Serial.print("/");
      //Serial.print(sCurrentY[l]); Serial.print(" ");
      //Serial.print(sWalkZ[lState]); Serial.print("/");
      //Serial.print(sCurrentZ[l]); Serial.println(" ");
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

float ld_x_adj = 0, ld_y_adj = 0;
float ld_flz, ld_frz, ld_blz, ld_brz;
void legDemo(){
  
  Serial.println("--------Leg Demo");

  // adjust for lean
  ld_x_adj = smooth(ld_x_adj, constrain(map(ahrs_r, -8, 8, 10, -10), -12, 12), 3);
  ld_y_adj = smooth(ld_y_adj, constrain(map(ahrs_p, -8, 8, 10, -10), -12, 12), 3);

  ld_flz = smooth(ld_flz, map(ly, 0, 255, 67, 47), 10);
  moveLeg(FRONT_LEFT, ld_x_adj, ld_y_adj, ld_flz, 0, 0, 0);

  ld_frz = smooth(ld_frz, map(ry, 0, 255, 67, 47), 10);
  moveLeg(FRONT_RIGHT, ld_x_adj, ld_y_adj, ld_frz, 0, 0, 0);

  ld_blz = smooth(ld_blz, map(lx, 0, 255, 67, 47), 10);
  moveLeg(BACK_LEFT, ld_x_adj, ld_y_adj, ld_blz, 0, 0, 0);

  ld_brz = smooth(ld_brz, map(rx, 0, 255, 67, 47), 10);
  moveLeg(BACK_RIGHT, ld_x_adj, ld_y_adj, ld_brz, 0, 0, 0);
  
  
}
