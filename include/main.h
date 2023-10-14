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
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 1 * 60 /* Time ESP32 will go to sleep (in seconds) */

// Functions
void printError(String text, OPT3001_ErrorCode error);
void enablePeripherals(void);
void configureLM92(void);
bool configureOPT3001(void);
void readSensors(void);
void sendDataToBroker(double room_temp,
                      float soil_temp,
                      float soil_humidity,
                      float lux);

#endif