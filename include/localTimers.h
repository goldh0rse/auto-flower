#ifndef LOCAL_TIMERS_H
#define LOCAL_TIMERS_H

#include <Arduino.h>

#define TOPIC_TIMER_ID 0
#define SENSOR_TIMER_ID 1
#define PRESCALER 8000

// Functions
void enableSendTopicInterruptTimer(void (*fn)(void));
void enableReadSensorsInterruptTimer(void (*fn)(void));

// Variables
extern hw_timer_t *timer;
extern SemaphoreHandle_t timerSemaphore;

#endif