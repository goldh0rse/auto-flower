#include "server.h"

void connectWiFi(String ssid, String passwd) {
    printSerial("Attempting to connect to WPA SSID: ", false);
    printSerial(ssid);

    WiFi.begin(ssid, passwd);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        printSerial(".", false);
    };

    printSerial("You're connected to the network");
}

void connectMQTTClient(MqttClient client, const char *host, uint16_t port) {
    printSerial("Attempting to connect to the MQTT broker: ", false);
    printSerial(host);

    if (!client.connect(host, port)) {
        printSerial("MQTT connection failed! Error code = ", false);
        printSerial(client.connectError());

        while (1)
            ;
    }

    printSerial("You're connected to the MQTT broker!");
}

void sendHttpPost(String apiUrl, String payload) {
    HTTPClient http;

    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        String response = http.getString();

        printSerial(httpResponseCode);
        printSerial(response);
    } else {
        // TODO: Error logging
        printSerial("Error on sending POST: ", false);
        printSerial(httpResponseCode);
    }

    http.end();
}

void publishTopic(MqttClient client, String topic, String payload) {
    client.beginMessage(topic);
    client.print(payload);
    client.endMessage();
}