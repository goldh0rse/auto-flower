#ifndef TIMERS_H
#define TIMER_H

#include "main.h"

// Macros
#define MS_MULTIPLIER 1000000

// Functions
void enableInterruptTimer(void);

// Variables
hw_timer_t *timer;
SemaphoreHandle_t timerSemaphore;

#endif