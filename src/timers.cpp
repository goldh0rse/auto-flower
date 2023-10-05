#include "localTimers.h"

hw_timer_t *timer = NULL;
SemaphoreHandle_t timerSemaphore;

void enableInterruptTimer(void (*fn)(void)) {
    // Create semaphore to inform us when the timer has fired
    timerSemaphore = xSemaphoreCreateBinary();

    // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for
    // more info).
    timer = timerBegin(0, 8000, true);

    // Attach onTimer function to our timer.
    timerAttachInterrupt(timer, fn, true);

    // Set alarm to call onTimer function every second (value in microseconds).
    // Repeat the alarm (third parameter)
    timerAlarmWrite(timer, 10000 * 60 * 5, true);  // 5 min with 8k Pre-scaler

    // Start an alarm
    timerAlarmEnable(timer);
}