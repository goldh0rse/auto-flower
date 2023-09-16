#include "main.h"

RTC_DATA_ATTR int bootCount = 0;

Adafruit_seesaw ss;
LM92 lm92(1, 0);
ClosedCube_OPT3001 opt3001;

void setup() {
    pinMode(SDA, PULLUP);
    pinMode(SCL, PULLUP);

#ifdef DEBUG_MODE
    Serial.begin(9600);
    while (!Serial)
        delay(10); // wait until serial port is opened
    initDisplay();
#endif

    lm92.ResultInCelsius = true;
    lm92.enableFaultQueue(true);

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Hello!");
    display.display();

    opt3001.begin(OPT3001_ADDRESS);
    printSerial("OPT3001 Manufacturer ID", false);
    printSerial(opt3001.readManufacturerID());
    printSerial("OPT3001 Device ID", false);
    printSerial(opt3001.readDeviceID());

    configureSensor();
    printResult("High-Limit", opt3001.readHighLimit());
    printResult("Low-Limit", opt3001.readLowLimit());
    printSerial("----");

    if (!ss.begin(SS_ADDRESS)) {
        printSerial("ERROR! seesaw not found");
        while (1)
            delay(1);
    } else {
        printSerial("seesaw started!");
    }
}

void loop() {
    delay(2000); // Take some time to open up the Serial Monitor

    printSerial("LM92: ", false);
    printSerial(lm92.readTemperature());
    OPT3001 result = opt3001.readResult();
    printResult("OPT3001", result);

    float tempC = ss.getTemp();
    uint16_t capread = ss.ss_touchRead(0);
    printSerial("Temperature: ", false);
    printSerial(tempC, false);
    printSerial("*C");
    printSerial("Capacitive: ", false);
    printSerial(capread);
    printSerial("----");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("LM92: ");
    display.println(lm92.readTemperature());
    if (result.error == NO_ERROR) {
        display.print("OPT3001: ");
        display.print(result.lux);
        display.println(" lux");
    } else {
        printError("OPT3001", result.error);
    }
    display.print("Soil Temp: ");
    display.println(tempC);
    display.print("Soil Cap: ");
    display.print(capread);

    display.display();
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
        printSerial("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        printSerial("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        printSerial("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        printSerial("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        printSerial("Wakeup caused by ULP program");
        break;
    default:
        printSerial("Wakeup was not caused by deep sleep: ", false);
        printSerial(wakeup_reason);
    }
}

void goToSleep(void) {
    // Increment boot number and print it every reboot
    ++bootCount;
    printSerial("Boot number: " + String(bootCount));

    // Print the wakeup reason for ESP32
    print_wakeup_reason();

    /*
    First we configure the wake up source
    We set our ESP32 to wake up every 5 seconds
    */
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    printSerial("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
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
    // printSerial("Configured all RTC Peripherals to be powered down in
    // sleep");

    /*
    Now that we have setup a wake cause and if needed setup the
    peripherals state in deep sleep, we can now start going to
    deep sleep.
    In the case that no wake up sources were provided but deep
    sleep was started, it will sleep forever unless hardware
    reset occurs.
    */
    printSerial("Going to sleep now");
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
        printSerial("OPT3001 Current Config:");
        printSerial("------------------------------");

        printSerial("Conversion ready (R):", false);
        printSerial(sensorConfig.ConversionReady, HEX);

        printSerial("Conversion time (R/W):", false);
        printSerial(sensorConfig.ConvertionTime, HEX);

        printSerial("Fault count field (R/W):", false);
        printSerial(sensorConfig.FaultCount, HEX);

        printSerial("Flag high field (R-only):", false);
        printSerial(sensorConfig.FlagHigh, HEX);

        printSerial("Flag low field (R-only):", false);
        printSerial(sensorConfig.FlagLow, HEX);

        printSerial("Latch field (R/W):", false);
        printSerial(sensorConfig.Latch, HEX);

        printSerial("Mask exponent field (R/W):", false);
        printSerial(sensorConfig.MaskExponent, HEX);

        printSerial("Mode of conversion operation (R/W):", false);
        printSerial(sensorConfig.ModeOfConversionOperation, HEX);

        printSerial("Polarity field (R/W):", false);
        printSerial(sensorConfig.Polarity, HEX);

        printSerial("Overflow flag (R-only):", false);
        printSerial(sensorConfig.OverflowFlag, HEX);

        printSerial("Range number (R/W):", false);
        printSerial(sensorConfig.RangeNumber, HEX);

        printSerial("------------------------------");
    }
}

void printResult(String text, OPT3001 result) {
    if (result.error == NO_ERROR) {
        printSerial(text, false);
        printSerial(": ", false);
        printSerial(result.lux, false);
        printSerial(" lux");
    } else {
        printError(text, result.error);
    }
}

void printError(String text, OPT3001_ErrorCode error) {
    printSerial(text, false);
    printSerial(": [ERROR] Code #", false);
    printSerial(F(error));
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
            printSerial(address, HEX);
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
