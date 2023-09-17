#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>
#include <Wire.h>

#define EEPROM_I2C_ADDRESS 0x50 // Replace with your EEPROM's I2C address
#define PAGE_SIZE 128           // The page size for 24AA1026 is 128 bytes
#define MAX_LOG_ENTRIES 100     // The maximum number of log entries to store

// Function declarations
void printSerial(const char *text, bool newLine = true);
void printSerial(const String &text, bool newline = true);
void printSerial(double variable, bool newLine = true);
void printSerial(int variable, int format = DEC, bool newLine = true);
void printSerial(unsigned int variable, int format = DEC, bool newLine = true);
void scanI2CDevices(void);
void writeEEPROMByte(int address, byte data);
byte readEEPROMByte(int address);
void writeErrorLog(int logIndex, const String &error);
String readErrorLog(int logIndex);

#endif