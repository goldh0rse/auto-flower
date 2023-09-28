#include "main.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
Adafruit_seesaw ss;
LM92 lm92;
ClosedCube_OPT3001 opt3001;

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
volatile double room_temp;
volatile uint16_t soil_humidity;
volatile float soil_temp;

const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topic[] = "arduino/simple";

void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

void setup() {
#ifdef DEBUG_MODE
    Serial.begin(9600);
    while (!Serial) delay(10);  // wait until serial port is opened
#endif
    delay(1000);

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
    connectMQTTClient(mqttClient, MQTT_BROKER, MQTT_PORT.toInt());

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

    // Create semaphore to inform us when the timer has fired
    timerSemaphore = xSemaphoreCreateBinary();

    // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for
    // more info).
    timer = timerBegin(0, 80, true);

    // Attach onTimer function to our timer.
    timerAttachInterrupt(timer, &onTimer, true);

    // Set alarm to call onTimer function every second (value in microseconds).
    // Repeat the alarm (third parameter)
    timerAlarmWrite(timer, 1000000, true);

    // Start an alarm
    timerAlarmEnable(timer);
}

void loop() {
    OPT3001 result = opt3001.readResult();
    soil_temp = ss.getTemp();
    soil_humidity = ss.ss_touchRead(0);
    room_temp = lm92.readTemperature();

    // call poll() regularly to allow the library to send MQTT keep alives which
    // avoids being disconnected by the broker
    mqttClient.poll();

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
    display.println(soil_humidity);
    display.display();

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
        mqttClient.beginMessage(topic);
        mqttClient.endMessage();
    }
}

void configureOPT3001() {
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

void printError(String text, OPT3001_ErrorCode error) {
    printSerial(text, false);
    printSerial(": [ERROR] Code #", false);
    printSerial(F(error));
}

bool checkEnv(void) {
#ifndef WIFI_SSID
    // Log error
    printSerial("SSID not set, exiting program.");
    return false;
#endif

#ifndef REST_API
    // Log error
    printSerial("No API host provided");
    return false;
#endif

#ifndef WIFI_PASSWD
    // Log error
    printSerial("Password is not set, exiting prgram.");
    return false;
#endif

#ifndef MQTT_BROKER
    // Log error
    printSerial("MQTT Host is not set, exiting program.");
    return false;
#endif

#ifndef MQTT_PORT
    // Log error
    printSerial("MQTT Port is not set, exiting porgram");
    return false;
#endif
    return true;
}