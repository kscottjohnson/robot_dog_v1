// gathers data about the robot

void getIMUdata() {
  imu.getEvent(&accel, &gyro, &temp);
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
