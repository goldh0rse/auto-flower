#include "main.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
Adafruit_seesaw ss;
LM92 lm92;
ClosedCube_OPT3001 opt3001;

volatile double room_temp;
volatile uint16_t soil_humidity;
volatile float soil_temp;

const char topic[] = "home/sensors";

void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

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

    // Activate i2c coupling
    Wire.begin();

    // Connect peripherals
    enablePeripherals();

    enableInterruptTimer(&onTimer);
}

void loop() {
    OPT3001 result = opt3001.readResult();
    soil_temp = ss.getTemp();
    soil_humidity = ss.ss_touchRead(0);
    room_temp = lm92.readTemperature();

    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    mqttClient.poll();

    if (result.error == NO_ERROR) {
        displayValues(room_temp, soil_temp, soil_humidity, result.lux);
    } else {
        printError("ERROR READING OPT3001:", result.error);
    }

    if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
        Serial.print("Sending message to topic: ");
        Serial.println(topic);

        DynamicJsonDocument doc(256);
        String payload;
        doc["room_temperature"] = room_temp;
        doc["soil_temperature"] = soil_temp;
        doc["soil_humidity"] = soil_humidity;
        doc["light"] = result.lux;
        serializeJson(doc, payload);

        publishTopic(mqttClient, topic, payload);
    }
}

void enablePeripherals(void) {
    initDisplay();

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
