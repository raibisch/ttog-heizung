#ifndef epdisplay_h
#define epdisplay_h

#include <Wire.h>
#include "ttgo_iodef.h"
#include "io.h"

void display_Init();
void display_Icon32(const uint16_t x, const uint16_t y, const uint8_t bitmap[]);
void display_Clear32(const uint16_t x, const uint16_t y);
void display_Icon16(const uint16_t x, const uint16_t y, const uint8_t bitmap[]);
void display_Clear16(const uint16_t x, const uint16_t y);
void display_Text9pt(const uint16_t x, const uint16_t y, String txt);
void display_Text18pt(uint16_t x, uint16_t y,String txt);
void display_Temp(int x, float temperature);


#endif
