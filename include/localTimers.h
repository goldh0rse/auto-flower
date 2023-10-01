#ifndef LOCAL_TIMERS_H
#define LOCAL_TIMERS_H

#include <Arduino.h>

// Macros
#define MS_MULTIPLIER 1000000

// Functions
void enableInterruptTimer(void (*fn)(void));

// Variables
extern hw_timer_t *timer;
extern SemaphoreHandle_t timerSemaphore;

#endif