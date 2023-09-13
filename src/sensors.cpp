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
  sensor.setTLow(11);
  sensor.setTHigh(90);
  sensor.setHysteresis(3);

  sensor.enableFaultQueue(true); // optional, make readings more tolerent to
  sensor.ResultInCelsius = true; // change to Celsius mode
}
