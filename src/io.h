#ifndef TTGO_IO_H
#define TTGO_IO_H


#include <SPI.h>
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

// by JG wird nicht gebraucht
//#include <Fonts/FreeMonoBold18pt7b.h>
//#include <Fonts/FreeMonoBold24pt7b.h>
//by JG
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>


//#include <imglib/gridicons_audio.h>
#include "thermo_pix.h"



#include "ttgo_iodef.h"
#include "var_dev.h"

void SetRelaisNetz(uint8_t x);
void SetRelaisVentil(uint8_t x);

#endif
