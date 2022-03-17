
// Fuer Test ohne Display
//=======================
//#ifndef NO_DISPLAY
//#define NO_DISPLAY
//#endif

// fuer HELTEC Wifi Kit32 
#ifndef OLED_DISPLAY 
#define OLED_DISPLAY 
#endif

#include "epdisplay.h"

#ifdef OLED_DISPLAY
  #include <U8x8lib.h>
#endif

#ifdef OLED_DISPLAY
  // OLED init
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
#else
// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\espressif\esp32\variants\lolin32\pins_arduino.h
GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4
#endif

void display_Init()
{

#ifdef NO_DISPLAY
  return;
  UpdateHistory();
  
#endif

#ifdef OLED_DISPLAY
  u8x8.begin();
  //u8x8.setFont(u8x8_font_courR18_2x3_f);
  u8x8.setFont(u8x8_font_8x13_1x2_f);

  Serial.println("Init ok!");
  //u8x8.setFlipMode(2);
  u8x8.drawString(1, 1, "INIT OK");
  delay(1000);
  u8x8.clearDisplay();
#else
  display.init(); // 
  display.setRotation(1); // landscape, keys at botton
  delay(100);
  display.fillScreen(GxEPD_WHITE);
  display.update();
  delay(100);
  display.fillScreen(GxEPD_WHITE);
  display.update();
#endif
}

void display_Icon32(const uint16_t x, const uint16_t y, const uint8_t bitmap[])
{
#ifdef NO_DISPLAY
  return;
#endif

#ifdef OLED_DISPLAY
  return;
#else
  //display.fillRect(x,y,32,32, GxEPD_BLACK);
  display.drawBitmap(bitmap, x, y, 32, 32, GxEPD_WHITE);
  display.updateWindow(x,y,32,32);
  //delay(3000);
#endif
}


/////////////////////////////////////////////////////////
void display_Clear32(const uint16_t x, const uint16_t y)
{
#ifdef NO_DISPLAY 
  return;
#endif
#ifdef OLED_DISPLAY
  //u8x8.clearDisplay();
#else
  display.fillRect(x,y,32,32, GxEPD_WHITE);
  display.updateWindow(x,y,32,32);
#endif
}

///////////////////////////////////////////////////////////////////////////////
void display_Icon16(const uint16_t x, const uint16_t y, const uint8_t bitmap[])
{
#ifdef NO_DISPLAY
  return;
#endif

#ifdef OLED_DISPLAY
  return;
#else
  //display.fillRect(x,y,32,32, GxEPD_BLACK);
  display.drawBitmap(bitmap, x, y, 16, 16, GxEPD_WHITE);
  display.updateWindow(x,y,16,16);
  //delay(3000);
#endif
}

void display_Clear16(const uint16_t x, const uint16_t y)
{
#ifdef NO_DISPLAY
  return;
#endif 
#ifdef OLED_DISPLAY
  return;
#else
  //display.fillRect(x,y,32,32, GxEPD_BLACK);
  display.fillRect(x, y, 16, 16, GxEPD_WHITE);
  display.updateWindow(x,y,16,16);
  //delay(3000);
#endif
}

//////////////////////////////////////////////////////////////////////
void display_Text9pt(const uint16_t x, const uint16_t y, String txt)
{
#ifdef NO_DISPLAY
  return;
#endif 
#ifdef OLED_DISPLAY
   if (x < 16)
   u8x8.drawString(x,1, txt.c_str());
#else
  //const char* name = "FreeMonoBold9pt7b";
  const GFXfont* f = &FreeMono9pt7b;

  uint16_t box_x = x;
  uint16_t box_y = y;
  uint16_t box_w = 1+ txt.length() * 11;
  uint16_t box_h = 14;
  uint16_t cursor_y = box_y + box_h -4;

  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);

  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);

  display.setCursor(box_x, cursor_y);
  display.print(txt);
  display.updateWindow(box_x, box_y, box_w, box_h, true);
#endif
}

void display_Text18pt(uint16_t x, uint16_t y,String txt)
{
#ifdef NO_DISPLAY
  return;
#endif
#ifdef OLED_DISPLAY
    if (y== 34)
    {y=3;  
     if (x < 10)
     { x = x -15 ; y=y+2;}
    }
    else
    {
    y = 1;
    }
    u8x8.drawString(x,y, txt.c_str());
  
   
#else
  //const char* name = "FreeMonoBold9pt7b";
  const GFXfont* f = &FreeSansBold18pt7b;

  uint16_t box_x = x;
  uint16_t box_y = y;
  uint16_t box_w = 1+ txt.length() * 17;
  uint16_t box_h = 32;
  uint16_t cursor_y = box_y + box_h -5;

  display.setFont(f);
  display.setTextColor(GxEPD_WHITE);

  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);

  display.setCursor(box_x+2, cursor_y);
  display.print(txt);
  display.updateWindow(box_x, box_y, box_w, box_h, true);
#endif
}


void display_Temp(int x,float temperature)
{
  //Serial.print("display_Temp:");
  //Serial.println(String(temperature, 1));
  display_Text18pt(x, 34, String(temperature,1));
}

/*
void display_Temp_old(float temperature)
{
  String temperatureString = String(temperature,1); // eine Kommastelle
  //const char* name = "FreeSansBold24pt7b";
  const GFXfont* f = &FreeSansBold24pt7b;

  uint16_t box_x = 1;
  uint16_t box_y = 36;
  uint16_t box_w = 93;
  uint16_t box_h = 48;
  uint16_t cursor_y = box_y + box_h-7;

  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);

  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(box_x, cursor_y);
  display.print(temperatureString);
  display.updateWindow(box_x, box_y, box_w, box_h, true);
}
*/
