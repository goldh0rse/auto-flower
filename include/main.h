#ifndef MAIN_H
#define MAIN_H

// Include
#include "display.h"
#include "util.h"
#include <Arduino.h>
#include <ClosedCube_OPT3001.h>
#include <LM92.h>
#include <SoilSensor.h>
#include <Wire.h>

// Macros
#define OPT3001_ADDRESS 0x45
#define SS_ADDRESS 0x36
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor us
#define TIME_TO_SLEEP 10          // Time ESP32 will go to sleep (in seconds)
#define ANALOGIN 2
#define DEBUG_MODE

// Functions
void print_wakeup_reason(void);
void goToSleep(void);
void printResult(String text, OPT3001 result);
void printError(String text, OPT3001_ErrorCode error);
void configureOPT3001(void);
void scanI2CDevices(void);

#endif