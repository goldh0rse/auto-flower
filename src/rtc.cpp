#include "rtc.h"

void initializeDS3231(void) {
  byte alarmDay = 0;
  byte alarmHour = 0;
  byte alarmMinute = 0;
  byte alarmSecond = 0;
  byte alarmBits = 0b00001111; // Alarm 1 every second
  bool alarmDayIsDay = false;
  bool alarmH12 = false;
  bool alarmPM = false;

  // Set alarm 1 to fire at one-second intervals
  ds3231.turnOffAlarm(1);
  ds3231.setA1Time(alarmDay,
                   alarmHour,
                   alarmMinute,
                   alarmSecond,
                   alarmBits,
                   alarmDayIsDay,
                   alarmH12,
                   alarmPM);
  // enable Alarm 1 interrupts
  ds3231.turnOnAlarm(1);
  // clear Alarm 1 flag
  ds3231.checkIfAlarm(1);
}

float getRTCTemperature(void) {
  float temp = ds3231.getTemperature();
  if (temp > -9999) {
    // it may be OK to use the returned value
    return temp;
  } else {
    // the value returned is not valid
  }
}