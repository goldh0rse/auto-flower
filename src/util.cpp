#include "util.h"
#include "main.h"

void printSerial(const char *text, bool newLine) {
#ifdef DEBUG_MODE
    if (newLine) {
        Serial.println(text);
    } else {
        Serial.print(text);
    }
#endif
}

void printSerial(const String &text, bool newLine) {
#ifdef DEBUG_MODE
    if (newLine) {
        Serial.println(text);
    } else {
        Serial.print(text);
    }
#endif
}

void printSerial(double variable, bool newLine) {
#ifdef DEBUG_MODE
    if (newLine) {
        Serial.println(variable);
    } else {
        Serial.print(variable);
    }
#endif
}

void printSerial(int variable, int format, bool newline) {
#ifdef DEBUG_MODE
    if (newline) {
        Serial.println(variable, format);
    } else {
        Serial.print(variable, format);
    }
#endif
}

void printSerial(unsigned int variable, int format, bool newline) {
#ifdef DEBUG_MODE
    if (newline) {
        Serial.println(variable, format);
    } else {
        Serial.print(variable, format);
    }
#endif
}

void scanI2CDevices(void) {
    byte error, address;
    int nDevices;

    printSerial("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            printSerial("I2C device found at address 0x", false);
            if (address < 16) {
                printSerial("0", false);
            }
            printSerial(address, HEX, false);
            printSerial(" !");

            nDevices++;
        }
    }
    if (nDevices == 0) {
        printSerial("No I2C devices found");
    } else {
        printSerial("done");
    }
}

void writeEEPROMByte(int address, byte data) {
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);
    Wire.write((int)(address >> 8));   // MSB
    Wire.write((int)(address & 0xFF)); // LSB
    Wire.write(data);
    Wire.endTransmission();
    delay(5); // Give EEPROM time to complete the write operation
}

byte readEEPROMByte(int address) {
    byte data = 0x00;
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);
    Wire.write((int)(address >> 8));   // MSB
    Wire.write((int)(address & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
    if (Wire.available()) {
        data = Wire.read();
    }
    return data;
}

void writeErrorLog(int logIndex, const String &error) {
    if (logIndex >= MAX_LOG_ENTRIES) {
        return; // Don't exceed max log entries
    }
    int baseAddress = logIndex * PAGE_SIZE;
    for (int i = 0; i < error.length(); ++i) {
        writeEEPROMByte(baseAddress + i, error[i]);
    }
}

String readErrorLog(int logIndex) {
    if (logIndex >= MAX_LOG_ENTRIES) {
        return ""; // Don't exceed max log entries
    }
    int baseAddress = logIndex * PAGE_SIZE;
    String error = "";
    char c;
    for (int i = 0; i < PAGE_SIZE; ++i) {
        c = readEEPROMByte(baseAddress + i);
        if (c == '\0') {
            break; // Null terminator, end of string
        }
        error += c;
    }
    return error;
}