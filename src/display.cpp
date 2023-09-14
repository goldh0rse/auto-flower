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