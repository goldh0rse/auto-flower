#include "localTimers.h"

hw_timer_t *sendToTopicTimer = NULL;
hw_timer_t *sensorTimer = NULL;
SemaphoreHandle_t timerSemaphore;

void enableSendTopicInterruptTimer(void (*fn)(void)) {
    // Create semaphore to inform us when the timer has fired
    timerSemaphore = xSemaphoreCreateBinary();

    // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for
    // more info).
    sendToTopicTimer = timerBegin(TOPIC_TIMER_ID, PRESCALER, true);

    // Attach onTimer function to our timer.
    timerAttachInterrupt(sendToTopicTimer, fn, true);

    // Set alarm to call onTimer function every second (value in microseconds).
    // Repeat the alarm (third parameter)
    timerAlarmWrite(sendToTopicTimer,
                    10000 * 60,
                    true);  // 5 min with 8k Pre-scaler

    // Start an alarm
    timerAlarmEnable(sendToTopicTimer);
}

void enableReadSensorsInterruptTimer(void (*fn)(void)) {
    // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for
    // more info).
    sensorTimer = timerBegin(SENSOR_TIMER_ID, PRESCALER, true);

    // Attach onTimer function to our timer.
    timerAttachInterrupt(sensorTimer, fn, true);

    // Set alarm to call onTimer function every second (value in microseconds).
    // Repeat the alarm (third parameter)
    timerAlarmWrite(sensorTimer,
                    10000 * 10,
                    true);  // 10 sec with 8k Pre-scaler

    // Start an alarm
    timerAlarmEnable(sensorTimer);
}