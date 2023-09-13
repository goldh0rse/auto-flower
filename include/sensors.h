#ifndef SENSOR_H
#define SENSOR_H

#include <LM92.h>

extern LM92 lm92;

double readTemperature(LM92 &sensor);
void initializeLM92(LM92 &sensor);

#endif
