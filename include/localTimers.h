#ifndef LOCAL_TIMERS_H
#define LOCAL_TIMERS_H

#include <Arduino.h>

// Functions
void enableInterruptTimer(void (*fn)(void));

// Variables
extern hw_timer_t *timer;
extern SemaphoreHandle_t timerSemaphore;

#endif