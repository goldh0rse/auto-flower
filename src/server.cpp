#include "server.h"

void connectWiFi(String ssid, String passwd) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, passwd);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    };

    Serial.println("You're connected to the network");
    Serial.println();
}

void connectMQTTClient(MqttClient client, const char *host, uint16_t port) {
    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(host);

    if (!client.connect(host, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(client.connectError());

        while (1)
            ;
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
}

void sendHttpPost(String apiUrl, String payload) {
    HTTPClient http;

    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        String response = http.getString();

        Serial.println(httpResponseCode);
        Serial.println(response);
    } else {
        // TODO: Error logging
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void publishTopic(MqttClient client, String topic, String payload) {
    client.beginMessage(topic);
    client.print(payload);
    client.endMessage();
}