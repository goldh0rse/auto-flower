#include "main.h"

Adafruit_seesaw ss;
LM92 lm92;
ClosedCube_OPT3001 opt3001;

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
volatile double room_temp;
volatile uint16_t capread;
volatile float soil_temp;
volatile bool showScreen;
byte interruptPin = 2;

void ARDUINO_ISR_ATTR onTimer() {
    // xSemaphoreGiveFromISR(timerSemaphore, NULL);

    showScreen = false;
    timerAlarmDisable(timer);
}

// External interrupt functin for starting up OLED display
void ext_interrupt() {
    timerAlarmEnable(timer);
    showScreen = true;
}

void setup() {
    // Activate i2c coupling
    Wire.begin();

    // Activate Oled
    pinMode(interruptPin, INPUT_PULLUP);

#ifdef DEBUG_MODE
    Serial.begin(9600);
    while (!Serial) delay(10);  // wait until serial port is opened
#endif
    delay(1000);
    scanI2CDevices();
    // Initialize OLED
    initDisplay();

#ifdef WIFI_SSID
    printSerial("SSID: ", false);
    printSerial(WIFI_SSID);
#else
    // Log error
    printSerial("SSID not set, exiting program.");
    exit(-1);
#endif

#ifdef REST_API
    printSerial("Rest API Host: ", false);
    printSerial(REST_API);
#else
    // Log error
    printSerial("No API host provided");
    exit(-1);
#endif

#ifdef WIFI_PASSWD
    printSerial("Password: ", false);
    // printSerial(WIFI_PASSWD);
    Serial.println(WIFI_PASSWD);
#else
    printSerial("Password is not set, exiting prgram.");
    exit(-1);
#endif

    connectWiFi(WIFI_SSID, WIFI_PASSWD);

    lm92.ResultInCelsius = true;
    lm92.enableFaultQueue(true);

    opt3001.begin(OPT3001_ADDRESS);
    // Serial.print("OPT3001 Manufacturer ID");
    // Serial.println(opt3001.readManufacturerID());
    // Serial.print("OPT3001 Device ID");
    // Serial.println(opt3001.readDeviceID());
    configureOPT3001();

    if (!ss.begin(SS_ADDRESS)) {
        printSerial("ERROR! seesaw not found");
        while (1) delay(1);
    } else {
        printSerial("seesaw started!");
    }

    attachInterrupt(digitalPinToInterrupt(interruptPin),
                    ext_interrupt,
                    FALLING);
    // Create semaphore to inform us when the timer has fired
    // timerSemaphore = xSemaphoreCreateBinary();

    // Use 1st timer of 4 (counted from zero).
    // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for
    // more info).
    timer = timerBegin(0, 80, true);

    // Attach onTimer function to our timer.
    timerAttachInterrupt(timer, &onTimer, true);

    // Set alarm to call onTimer function every second (value in microseconds).
    // Repeat the alarm (third parameter)
    timerAlarmWrite(timer, 1000000, true);

    // Start an alarm
    // timerAlarmEnable(timer);
    showScreen = false;
}

void loop() {
    OPT3001 result = opt3001.readResult();
    soil_temp = ss.getTemp();
    capread = ss.ss_touchRead(0);
    room_temp = lm92.readTemperature();

    if (showScreen) {
        display.clearDisplay();
        display.setTextColor(WHITE);
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
        display.println(soil_temp);
        display.print("Soil Cap: ");
        display.println(capread);
        display.display();
    } else {
        display.flush();
        display.clearDisplay();
    }

    // if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    //     // scanI2CDevices();
    //     // // Create JSON document
    //     // DynamicJsonDocument doc(256);
    //     // String payload;
    //     // doc["room_temperature"] = room_temp;

    //     // serializeJson(doc, payload);  // Serialize JSON document to
    //     // String doc.clear();                  // Clear
    //     // // Send HTTP POST request
    //     // sendHttpPost(REST_API + "/api/v1/temperature", payload);

    //     // doc["soil_temperature"] = soil_temp;
    //     // doc["moisture"] = capread;
    //     // serializeJson(doc, payload);  // Serialize JSON document to
    //     // String doc.clear();                  // Clear
    //     // // Send HTTP POST request
    //     // sendHttpPost(REST_API + "/api/v1/soil", payload);

    //     // doc["lux"] = result.lux;
    //     // serializeJson(doc, payload);  // Serialize JSON document to
    //     // String
    //     // // Send HTTP POST request
    //     // sendHttpPost(REST_API + "/api/v1/light", payload);

    //     Serial.print("Temp: ");
    //     Serial.println(room_temp);
    //     Serial.print("Lux: ");
    //     Serial.println(result.lux);
    //     display.clearDisplay();
    //     display.setTextColor(WHITE);
    //     display.setCursor(0, 0);
    //     display.print("LM92: ");
    //     display.println(lm92.readTemperature());
    //     if (result.error == NO_ERROR) {
    //         display.print("OPT3001: ");
    //         display.print(result.lux);
    //         display.println(" lux");
    //     } else {
    //         printError("OPT3001", result.error);
    //     }
    //     display.print("Soil Temp: ");
    //     display.println(soil_temp);
    //     display.print("Soil Cap: ");
    //     display.println(capread);
    //     display.display();
    // }
}

void configureOPT3001() {
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
