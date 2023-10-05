#ifndef MAIN_H
#define MAIN_H

// Include
#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <ClosedCube_OPT3001.h>
#include <LM92.h>
#include <SoilSensor.h>
#include <WiFi.h>
#include <Wire.h>

#include "display.h"
#include "localTimers.h"
#include "server.h"
#include "util.h"

// Macros
#define OPT3001_ADDRESS 0x45
#define SS_ADDRESS 0x36
#define DEBUG_MODE

// Functions
void printError(String text, OPT3001_ErrorCode error);
void enablePeripherals(void);
void configureLM92(void);
bool configureOPT3001(void);

#endif