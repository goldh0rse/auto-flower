#ifndef RTC_H
#define RTC_H

#include <DS3231.h>

extern DS3231 ds3231;

void initializeDS3231(void);
float getRTCTemperature(void);

#endif
