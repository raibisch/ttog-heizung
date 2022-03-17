
#include <Arduino.h>
#include "io.h"
#include "epdisplay.h"


// Relais1 Netzspannung --------------
void SetRelaisNetz(uint8_t x)
{
  Serial.print("Netz: ");
   
  Device_Pumpe.display_update = true;
  if (x > 0)
  {
    Serial.println("EIN");
    Device_Pumpe.active = true;   // z.Z werden
    Device_Heizung.active = true; // beide aktiv gesetzt
    digitalWrite(OUT_RELAIS1, 0);
    display_Icon16(40, 78, pumpe_16px);
  }
  else
  {
    Serial.println("AUS");
    Device_Pumpe.active = false;   // zZ. werden
    Device_Heizung.active = false; // beide passiv gsetzt
    digitalWrite(OUT_RELAIS1, 1);
    display_Clear16(40,78);
  }
}

// Relais2 Ventil --------------
void SetRelaisVentil(uint8_t x)
{
  Serial.print("Ventil: ");
  Device_WWSpeicher.display_update = true;
  Device_Heizung.display_update    = true;
  if (x > 0)
  {
    Serial.println("EIN");
    digitalWrite(OUT_RELAIS2, 0);
    Device_WWSpeicher.active = true;
    display_Icon32(78,70, wwspeicher_32px);
    display_Clear32(78+74,70);
  }
  else
  {
    Serial.println("AUS");
    digitalWrite(OUT_RELAIS2, 1);
    Device_WWSpeicher.active = false;
    display_Icon32(78+74,70, heizung_32px);
    display_Clear32(78,70);
  }
}
