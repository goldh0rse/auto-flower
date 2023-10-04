#include "main.h"

// WiFiClient wifiClient;
// MqttClient mqttClient(wifiClient);
Adafruit_seesaw ss;
LM92 lm92;
ClosedCube_OPT3001 opt3001;

// hw_timer_t *timer = NULL;
// volatile SemaphoreHandle_t timerSemaphore;
volatile double room_temp;
volatile uint16_t soil_humidity;
volatile float soil_temp;

const char topic[] = "home/sensor";

void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

void setup() {
#ifdef DEBUG_MODE
    Serial.begin(9600);
    while (!Serial) delay(10);  // wait until serial port is opened
    delay(1000);
#endif

    // Check environment
    if (!checkEnv()) {
        exit(-1);
    }

    // Activate i2c coupling
    Wire.begin();

    // Initialize OLED
    initDisplay();

    // Connect to peripherals
    connectWiFi(WIFI_SSID, WIFI_PASSWD);
    while (true) delay(1000);
    // connectMQTTClient(mqttClient, MQTT_BROKER, strtoul(MQTT_PORT, NULL, 10));

    lm92.ResultInCelsius = true;
    lm92.enableFaultQueue(true);

    opt3001.begin(OPT3001_ADDRESS);
    if (!configureOPT3001()) {
        exit(-1);
    }

    if (!ss.begin(SS_ADDRESS)) {
        printSerial("ERROR! seesaw not found");
        while (1) delay(1);
    } else {
        printSerial("seesaw started!");
    }

    enableInterruptTimer(&onTimer);
}

void loop() {
    OPT3001 result = opt3001.readResult();
    soil_temp = ss.getTemp();
    soil_humidity = ss.ss_touchRead(0);
    room_temp = lm92.readTemperature();

    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    // mqttClient.poll();

    if (result.error == NO_ERROR) {
        displayValues(room_temp, soil_humidity, soil_humidity, result.lux);
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

        // publishTopic(mqttClient, topic, payload);
    }
}

bool configureOPT3001(void) {
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
