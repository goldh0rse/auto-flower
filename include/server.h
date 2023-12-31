#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "util.h"

// Functions
void connectWiFi(String ssid, String passwd);
void connectMQTTClient(MqttClient client, const char *host, uint16_t port);
void sendHttpPost(String apiUrl, String payload);
void publishTopic(MqttClient client, String topic, String payload);

#endif
