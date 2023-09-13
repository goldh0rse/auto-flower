#include "display.h"
#include "sensors.h"
#include <Arduino.h>
#include <SPI.h> // For serial monitor
#include <U8g2lib.h>
#include <Wire.h> // For I2C

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  double temp = readTemperature(lm92);
  Serial.println(temp);
}
