#ifndef MAIN_H
#define MAIN_H

// Include
#include "display.h"
#include "server.h"
#include "util.h"
#include <Arduino.h>
#include <ClosedCube_OPT3001.h>
#include <LM92.h>
#include <SoilSensor.h>
#include <Wire.h>

// Macros
#define OPT3001_ADDRESS 0x45
#define SS_ADDRESS 0x36
#define DEBUG_MODE

// Functions
void printResult(String text, OPT3001 result);
void printError(String text, OPT3001_ErrorCode error);
void configureOPT3001(void);

#endif