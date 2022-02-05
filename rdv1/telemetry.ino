// gathers data about the robot

void getIMUdata() {
  imu.getEvent(&accel, &gyro, &temp);
  cal.calibrate(accel);
  cal.calibrate(gyro);
  accel_x = accel.acceleration.x;
  accel_y = accel.acceleration.y;
  accel_z = accel.acceleration.z;
  gyro_x = gyro.gyro.x;
  gyro_y = gyro.gyro.y;
  gyro_z = gyro.gyro.z;
  Serial.print("Accel: ");
  Serial.print(accel_x);Serial.print(" ");
  Serial.print(accel_y);Serial.print(" ");
  Serial.print(accel_z);
  Serial.println(" m/s^2");
  Serial.print("Gyro: ");
  Serial.print(gyro_x);Serial.print(" ");
  Serial.print(gyro_y);Serial.print(" ");
  Serial.println(gyro_z);
}

void getRemoteTelemetry() {
  robotMsg[0] = robotBatt;
}

uint8_t readBatt() {
  float volts = analogRead(BATT_PIN) * 2.0 * 3.3 / 1024.0; 
  return (volts * volts * -2.9487 + volts * 25.0447 - 52.1712) * 200;
}

uint8_t smooth(uint8_t oldValue, uint8_t newValue, uint8_t factor) {
  return (oldValue * factor + newValue) / (factor + 1);
}

void loadIMUcalibration() {
  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
  }
  
  if (! cal.loadCalibration()) {
    Serial.println("**WARNING** No calibration loaded/found");
  }
  cal.printSavedCalibration();

  Serial.println("Calibrations found: ");
  Serial.print("\tMagnetic Hard Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.mag_hardiron[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("\tMagnetic Soft Offset: ");
  for (int i=0; i<9; i++) {
    Serial.print(cal.mag_softiron[i]); 
    if (i != 8) Serial.print(", ");
  }
  Serial.println();

  Serial.print("\tMagnetic Field Magnitude: ");
  Serial.println(cal.mag_field);

  Serial.print("\tGyro Zero Rate Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.gyro_zerorate[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();

  Serial.print("\tAccel Zero G Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.accel_zerog[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();

}
