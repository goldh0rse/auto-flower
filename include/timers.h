#ifndef TIMERS_H
#define TIMER_H

// Macros
#define MS_MULTIPLIER 1000000

// Functions
void enableInterruptTimer(void (*fn)(void));

// Variables
hw_timer_t *timer;
SemaphoreHandle_t timerSemaphore;

#endif