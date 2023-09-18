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

void sendHttpPost(String apiHost, String payload) {
    HTTPClient http; // Create an HTTPClient object

    http.begin(apiHost + "/api/data"); // Specify the URL for the request
    http.addHeader("Content-Type",
                   "application/json"); // Specify content-type header

    int httpResponseCode = http.POST(payload); // Send the actual POST request

    if (httpResponseCode > 0) {
        String response = http.getString(); // Get the response to the request

        Serial.println(httpResponseCode); // Print HTTP return code
        Serial.println(response);         // Print request response payload
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end(); // Close connection
}