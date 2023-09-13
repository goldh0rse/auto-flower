#include "sensors.h"

LM92 lm92(0, 0); // address 1, 0

double readTemperature(LM92 &sensor) {
  double res = sensor.readTemperature();
  if (res == NULL) {
    // Throw error?
  }
  return res;
}

void initializeLM92(LM92 &sensor) {
  lm92.setTLow(11);
  lm92.setTHigh(90);
  lm92.setHysteresis(3);

  //   Serial.println(lm92.getManufacturerID());
  //   Serial.println(lm92.readConfigurationRegister());
  //   Serial.println(lm92.readHysteresis());
  //   Serial.println(lm92.readTLow());
  //   Serial.println(lm92.readTHigh());
  //   Serial.println(lm92.readTCritical());

  lm92.enableFaultQueue(true); // optional, make readings more tolerent to
  lm92.ResultInCelsius = true; // change to Celsius mode
}
