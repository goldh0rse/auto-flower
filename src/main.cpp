#include "main.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
Adafruit_seesaw ss;
LM92 lm92;
ClosedCube_OPT3001 opt3001;

volatile double room_temp;
volatile uint16_t soil_humidity;
volatile float soil_temp;
OPT3001 opt;

const char topic[] = "home/sensors";

void setup() {
#ifdef DEBUG_MODE
    Serial.begin(9600);
    while (!Serial) delay(10);  // wait until serial port is opened
    delay(1000);
#endif

    // Check environment
    if (!checkEnv()) {
        // Environment wasn't hasn't been setup correctly
        exit(-1);
    }

    // Print the wakeup reason for ESP32
    print_wakeup_reason();

    /*
    First we configure the wake up source
    We set our ESP32 to wake up every 5 seconds
    */
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    printSerial("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                " Seconds");

    // Activate i2c coupling
    Wire.begin();

    // Connect peripherals
    enablePeripherals();

    // Read sensor-data
    readSensors();

    // Send data to MQTT broker
    sendDataToBroker(room_temp, soil_temp, soil_humidity, opt.lux);
    delay(1000);
    printSerial("Going to sleep now");
    Serial.flush();
    esp_deep_sleep_start();
    // We will never reach this space
}

void loop() {}

void enablePeripherals(void) {
    // initDisplay();

    connectWiFi(WIFI_SSID, WIFI_PASSWD);
    connectMQTTClient(mqttClient, MQTT_BROKER, strtoul(MQTT_PORT, NULL, 10));

    configureLM92();

    if (!configureOPT3001()) {
        exit(-1);
    }

    if (!ss.begin(SS_ADDRESS)) {
        printSerial("ERROR! seesaw not found");
        while (1) delay(1);
    } else {
        printSerial("seesaw started!");
    }
}

void configureLM92(void) {
    lm92.ResultInCelsius = true;
    lm92.enableFaultQueue(true);
}

bool configureOPT3001(void) {
    opt3001.begin(OPT3001_ADDRESS);
    OPT3001_Config newConfig;

    newConfig.RangeNumber = B1100;
    newConfig.ConvertionTime = B0;
    newConfig.Latch = B1;
    newConfig.ModeOfConversionOperation = B11;

    OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
    if (errorConfig != NO_ERROR) {
        printError("OPT3001 configuration", errorConfig);
        return false;
    }
    return true;
}

void readSensors(void) {
    printSerial("Reading Sensors");
    opt = opt3001.readResult();
    soil_temp = ss.getTemp();
    soil_humidity = ss.ss_touchRead(0);
    room_temp = lm92.readTemperature();
    printSerial("Read sensors");
}

void sendDataToBroker(double room_temp,
                      float soil_temp,
                      float soil_humidity,
                      float lux) {
    mqttClient.poll();

    printSerial("Sending message to topic: ", false);
    printSerial(topic);

    DynamicJsonDocument doc(256);
    String payload;
    doc["room_temperature"] = room_temp;
    doc["soil_temperature"] = soil_temp;
    doc["soil_humidity"] = soil_humidity;
    doc["light"] = lux;
    serializeJson(doc, payload);
    printSerial(payload);

    publishTopic(mqttClient, topic, payload);
}

bool print_wakeup_reason(void) {
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
            break;
    }
    return false;
}