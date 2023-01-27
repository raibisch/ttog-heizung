#include <Arduino.h>
#include <Timezone.h>
#include <TimeLib.h>

#include <Preferences.h>
Preferences storage;

#include "ttgo_iodef.h"
#include "io.h"
#include "history.h"

Preferences hist;

void history_init()
{
   hist.begin("history", false); // use history
   Device_allg.sec_brenner_on_sum = hist.getULong64("heizall",0);
   Serial.printf("Heizoel-Verbrauch (l): %4.2f\n",get_Heizoel_mLiter_Verbrauch()/(float)1000);
   Device_allg.sec_brenner_old_sum = Device_allg.sec_brenner_on_sum;
}

bool history_save()
{
  hist.putULong64("heizall",Device_allg.sec_brenner_on_sum);

  // ... hier koennten nnoch weitere Werte gespeichert werden ...
  return true;
}
 
uint32_t get_Heizoel_mLiter_Verbrauch()
{
  return (uint32_t)((uint64_t)(Device_allg.sec_brenner_on_sum * Config_val.mliter_oel_pro_h/10) / (uint32_t)360);
}

bool set_Heizoel_mLiter_Verbrauch(uint32_t verbrauch)
{
  Serial.print("func: set_Heizoel_mLiter_Verbrauch:");
  Serial.println(verbrauch);
  Device_allg.sec_brenner_on_sum = ((uint64_t)verbrauch * 3600) / Config_val.mliter_oel_pro_h;
  Device_allg.sec_brenner_old_sum = Device_allg.sec_brenner_on_sum;
  hist.putLong64("heizall",Device_allg.sec_brenner_on_sum);
  return true;
}