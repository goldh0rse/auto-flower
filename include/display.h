#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "util.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

extern Adafruit_SSD1306 display;

// Functions
void initDisplay(void);
void displayWakeUp(void);
void displaySleep(void);
void displayValues(double room_temp,
                   uint16_t soil_humidity,
                   float soil_temp,
                   float lux);

#endif
