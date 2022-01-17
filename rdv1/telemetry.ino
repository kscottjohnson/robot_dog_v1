void getTelemetry() {
  robotMsg[0] = robotBatt;
}

uint8_t readBatt() {
  float volts = analogRead(BATT_PIN) * 2.0 * 3.3 / 1024.0; 
  return (volts * volts * -2.9487 + volts * 25.0447 - 52.1712) * 200;
}

uint8_t smooth(uint8_t oldValue, uint8_t newValue, uint8_t factor) {
  return (oldValue * factor + newValue) / (factor + 1);
}
