#include "display.h"
#include "seesaw.h"
#include <Arduino.h>
#include <ClosedCube_OPT3001.h>
#include <LM92.h>
#include <Wire.h>

#define OPT3001_ADDRESS 0x45
#define SS_ADDRESS 0x36
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor us
#define TIME_TO_SLEEP 10          // Time ESP32 will go to sleep (in seconds)
#define ANALOGIN 2

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(void);
void goToSleep(void);
void printResult(String text, OPT3001 result);
void printError(String text, OPT3001_ErrorCode error);
void configureSensor(void);

Adafruit_seesaw ss;
LM92 lm92(1, 0);
ClosedCube_OPT3001 opt3001;

void setup() {
    pinMode(SDA, PULLUP);
    pinMode(SCL, PULLUP);

    Serial.begin(9600);
    while (!Serial)
        delay(10); // wait until serial port is opened

    lm92.ResultInCelsius = true;
    lm92.enableFaultQueue(true);

    initDisplay();
    display.clearDisplay();
    display.setTextColor(WHITE);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Hello!");
    display.display();

    opt3001.begin(OPT3001_ADDRESS);
    Serial.print("OPT3001 Manufacturer ID");
    Serial.println(opt3001.readManufacturerID());
    Serial.print("OPT3001 Device ID");
    Serial.println(opt3001.readDeviceID());

    configureSensor();
    printResult("High-Limit", opt3001.readHighLimit());
    printResult("Low-Limit", opt3001.readLowLimit());
    Serial.println("----");

    if (!ss.begin(SS_ADDRESS)) {
        Serial.println("ERROR! seesaw not found");
        while (1)
            delay(1);
    } else {
        Serial.print("seesaw started!");
    }
}

void loop() {
    delay(5000); // Take some time to open up the Serial Monitor

    Serial.print("LM92: ");
    Serial.println(lm92.readTemperature());
    OPT3001 result = opt3001.readResult();
    printResult("OPT3001", result);

    float tempC = ss.getTemp();
    uint16_t capread = ss.ss_touchRead(0);
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("*C");
    Serial.print("Capacitive: ");
    Serial.println(capread);
    Serial.println("----");

    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println(" !");

            nDevices++;
        }
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.println("done\n");
    }
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Serial.println("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        Serial.println("Wakeup caused by ULP program");
        break;
    default:
        Serial.printf("Wakeup was not caused by deep sleep: %d\n",
                      wakeup_reason);
    }
}

void goToSleep(void) {
    // Increment boot number and print it every reboot
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));

    // Print the wakeup reason for ESP32
    print_wakeup_reason();

    /*
    First we configure the wake up source
    We set our ESP32 to wake up every 5 seconds
    */
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                   " Seconds");

    /*
    Next we decide what all peripherals to shut down/keep on
    By default, ESP32 will automatically power down the peripherals
    not needed by the wakeup source, but if you want to be a poweruser
    this is for you. Read in detail at the API docs
    http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
    Left the line commented as an example of how to configure peripherals.
    The line below turns off all RTC peripherals in deep sleep.
    */
    // esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // Serial.println("Configured all RTC Peripherals to be powered down in
    // sleep");

    /*
    Now that we have setup a wake cause and if needed setup the
    peripherals state in deep sleep, we can now start going to
    deep sleep.
    In the case that no wake up sources were provided but deep
    sleep was started, it will sleep forever unless hardware
    reset occurs.
    */
    Serial.println("Going to sleep now");
    Serial.flush();
    esp_deep_sleep_start();
}

void configureSensor() {
    OPT3001_Config newConfig;

    newConfig.RangeNumber = B1100;
    newConfig.ConvertionTime = B0;
    newConfig.Latch = B1;
    newConfig.ModeOfConversionOperation = B11;

    OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
    if (errorConfig != NO_ERROR)
        printError("OPT3001 configuration", errorConfig);
    else {
        OPT3001_Config sensorConfig = opt3001.readConfig();
        Serial.println("OPT3001 Current Config:");
        Serial.println("------------------------------");

        Serial.print("Conversion ready (R):");
        Serial.println(sensorConfig.ConversionReady, HEX);

        Serial.print("Conversion time (R/W):");
        Serial.println(sensorConfig.ConvertionTime, HEX);

        Serial.print("Fault count field (R/W):");
        Serial.println(sensorConfig.FaultCount, HEX);

        Serial.print("Flag high field (R-only):");
        Serial.println(sensorConfig.FlagHigh, HEX);

        Serial.print("Flag low field (R-only):");
        Serial.println(sensorConfig.FlagLow, HEX);

        Serial.print("Latch field (R/W):");
        Serial.println(sensorConfig.Latch, HEX);

        Serial.print("Mask exponent field (R/W):");
        Serial.println(sensorConfig.MaskExponent, HEX);

        Serial.print("Mode of conversion operation (R/W):");
        Serial.println(sensorConfig.ModeOfConversionOperation, HEX);

        Serial.print("Polarity field (R/W):");
        Serial.println(sensorConfig.Polarity, HEX);

        Serial.print("Overflow flag (R-only):");
        Serial.println(sensorConfig.OverflowFlag, HEX);

        Serial.print("Range number (R/W):");
        Serial.println(sensorConfig.RangeNumber, HEX);

        Serial.println("------------------------------");
    }
}

void printResult(String text, OPT3001 result) {
    if (result.error == NO_ERROR) {
        Serial.print(text);
        Serial.print(": ");
        Serial.print(result.lux);
        Serial.println(" lux");
    } else {
        printError(text, result.error);
    }
}

void printError(String text, OPT3001_ErrorCode error) {
    Serial.print(text);
    Serial.print(": [ERROR] Code #");
    Serial.println(error);
}
