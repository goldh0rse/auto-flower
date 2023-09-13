#include "display.h"

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);

void initDisplay(void) {
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  u8x8.drawString(0, 1, "Hello World!");
  u8x8.setInverseFont(1);
  u8x8.drawString(0, 0, "012345678901234567890123456789");
  u8x8.setInverseFont(0);
}

void draw(void) {

  // Draw pixels
  // oled.drawPixel(11, 44);
  // oled.drawPixel(13, 44);

  // Draw line3
  // oled.drawLine(12, 28, 12, 60);
  // oled.drawLine(12, 60, 128, 60);
  // oled.drawLine(12, 60, 128, 30);

  // Print strings

  // char dateTime[50];
  // sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d",
  //         myRTC.getYear(),
  //         myRTC.getMonth(century),
  //         myRTC.getDate(),
  //         myRTC.getHour(h12Flag, pmFlag),
  //         myRTC.getMinute(),
  //         myRTC.getSecond());
  // // Serial.println(dateTime);
  // oled.drawStr(0, 12, dateTime);

  // char temp[10];
  // sprintf(temp, "%f", lm92.readTemperature());
  // oled.drawStr(0, 24, temp);
  // Serial.print(F("Temperature: "));
  // Serial.print(temp);
  // Serial.print(F(lm92.readTemperature()));

  // Serial.print(myRTC.getYear(), DEC);
  // Serial.print("-");
  // Serial.print(myRTC.getMonth(century), DEC);
  // Serial.print("-");
  // Serial.print(myRTC.getDate(), DEC);
  // Serial.print(" ");
  // Serial.print(myRTC.getHour(h12Flag, pmFlag), DEC); // 24-hr
  // Serial.print(":");
  // Serial.print(myRTC.getMinute(), DEC);
  // Serial.print(":");
  // Serial.println(myRTC.getSecond(), DEC);
}