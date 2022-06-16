#ifndef TTGO_IODEV_H
#define TTGO_IODEV_H

// Pin definition HELTEC WIFI-KIT32
//#define HELTEC_WIFI_KIT32
#ifdef HELTEC_WIFI_KIT32
static const uint8_t LED_BUILTIN = 25;
#endif

// Pin definetion of TTGO T5 V1-3
#define BUTTON1     38   // GPIO38
#define BUTTON2     37   // GPIO37
#define BUTTON3     39   // GPIO39

#define IN_ONEWIRE   19    // GPIO19 INPUT
#define IN_BRENNER   26     // Brenner Ein Signal

#define IN_ERROR     27     // Fehler Signal

#define OUT_RELAIS1  22   // GPIO21 OUTPUT
#define OUT_RELAIS2  21   // GPIO22 OUTPUT

#define ONE_WIRE_BUS       19
#define TEMPERATURE_PRECISION 9

#define _EIN true
#define _AUS false

#endif
