#include "display.h"
#define DISPLAY_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initDisplay(void) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    display.setRotation(2);
}

void displayValues(double room_temp,
                   uint16_t soil_humidity,
                   float soil_temp,
                   float lux) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("LM92: ");
    display.println(room_temp);
    display.print("OPT3001: ");
    display.println(lux);
    display.print("Soil Temp: ");
    display.println(soil_temp);
    display.print("Soil Cap: ");
    display.println(soil_humidity);
    display.display();
}