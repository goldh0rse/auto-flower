#include "server.h"

bool connectWiFi(String ssid, String passwd) {
    // connect to WiFi
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