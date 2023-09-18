#include "server.h"

bool connectWiFi(String ssid, String passwd) {
    printSerial("Connecting to: ", false);
    printSerial(ssid);
    WiFi.begin(ssid, passwd);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        printSerial(".", false);
    }
    printSerial(" CONNECTED");
    return true;
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