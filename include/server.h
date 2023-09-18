#ifndef SERVER_H
#define SERVER_H

#include "util.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

// Functions
bool connectWiFi(String ssid, String passwd);
void sendHttpPost(String apiUrl, String payload);

#endif
