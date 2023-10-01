#include "timers.h"

hw_timer_t *timer = NULL;
SemaphoreHandle_t timerSemaphore;

void enableInterruptTimer(void (*fn)(void)) {
    // Create semaphore to inform us when the timer has fired
    timerSemaphore = xSemaphoreCreateBinary();

    // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for
    // more info).
    timer = timerBegin(0, 80, true);

    // Attach onTimer function to our timer.
    timerAttachInterrupt(timer, fn, true);

    // Set alarm to call onTimer function every second (value in microseconds).
    // Repeat the alarm (third parameter)
    timerAlarmWrite(timer, 1 * MS_MULTIPLIER, true);

    // Start an alarm
    timerAlarmEnable(timer);
}