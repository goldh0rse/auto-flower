#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>

extern U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8;

void initDisplay(void);
void draw(void);

#endif
