#include "display.h"
#include "rtc.h"
#include "sensors.h"
#include <Arduino.h>
#include <SPI.h>  // For serial monitor
#include <Wire.h> // For I2C

// myRTC interrupt pin
#define CLINT 2
// Interrupt signaling byte
volatile byte tick = 1;

void setup() {
  // Begin Serial communication
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println();
  Serial.println("Starting Serial");

  Wire.begin();

  initializeLM92(lm92);
  initializeDS3231();
  initDisplay();

  // attach clock interrupt
  pinMode(CLINT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLINT), isr_TickTock, FALLING);

  // Use builtin LED to blink
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // static variable to keep track of LED on/off state
  static byte state = false;

  // if alarm went of, do alarm stuff
  if (tick) {
    tick = 0;
    state = ~state;
    digitalWrite(LED_BUILTIN, state);

    // optional serial output
    Serial.print("Turning LED ");
    Serial.println((state ? "ON" : "OFF"));

    // Clear Alarm 1 flag
    ds3231.checkIfAlarm(1);
  }
  double temp = readTemperature(lm92);
  Serial.println(temp);
}

void isr_TickTock() {
  // interrupt signals to loop
  tick = 1;
  return;
}