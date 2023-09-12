#include <Arduino.h>
#include <SPI.h>  // For serial monitor
#include <Wire.h> // For I2C
#include <U8g2lib.h>
// #include <DS3231.h>     // For RTC
// #include <LM92.h> // For LM92

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);
// DS3231 myRTC;

// LM92 lm92(0, 0); // address 1, 0

// bool century = false;
// bool h12Flag;
// bool pmFlag;

void draw(void);

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  // lm92.setTLow(11);
  // lm92.setTHigh(90);
  // lm92.setHysteresis(3);

  // lm92.setTLow(11);
  // lm92.setTHigh(90);
  // lm92.setHysteresis(3);

  // Serial.println(lm92.getManufacturerID());
  // Serial.println(lm92.readConfigurationRegister());
  // Serial.println(lm92.readHysteresis());
  // Serial.println(lm92.readTLow());
  // Serial.println(lm92.readTHigh());
  // Serial.println(lm92.readTCritical());

  // lm92.enableFaultQueue(true); // optional, make readings more tolerent to inteference
  // lm92.ResultInCelsius = true; // change to Celsius mode

  // Serial.println(lm92.readTemperature());

  // // Set font to Helvetica size 10
  // oled.setFont(u8g_font_helvB08);

  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop()
{
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 1, "Hello World!");
  u8x8.setInverseFont(1);
  u8x8.drawString(0, 0, "012345678901234567890123456789");
  u8x8.setInverseFont(0);
  // u8x8.drawString(0,8,"Line 8");
  // u8x8.drawString(0,9,"Line 9");
  delay(2000);
  // oled.firstPage();
  // do
  // {
  //   draw();
  // } while (oled.nextPage());
}

void draw(void)
{

  // Draw pixels
  // oled.drawPixel(11, 44);
  // oled.drawPixel(13, 44);

  // Draw line3
  // oled.drawLine(12, 28, 12, 60);
  // oled.drawLine(12, 60, 128, 60);
  // oled.drawLine(12, 60, 128, 30);

  // Print strings
  delay(500);

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