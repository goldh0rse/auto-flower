#include "server.h"

void connectWiFi(String ssid, String passwd) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_SSID);
    while (WiFi.begin(WIFI_SSID, WIFI_PASSWD) != WL_CONNECTED) {
        // failed, retry
        Serial.print(".");
        delay(1000);
    }

    Serial.println("You're connected to the network");
    Serial.println();
}

void connectMQTTClient(MqttClient client, String broker, int port) {
    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);

    if (!client.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

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