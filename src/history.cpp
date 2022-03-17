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
   Serial.printf("History-Brenner-Laufzeit:%d\n",Device_allg.sec_brenner_on_sum);
   Device_allg.sec_brenner_old_sum = Device_allg.sec_brenner_on_sum;
}

bool history_save()
{
  hist.putULong64("heizall",Device_allg.sec_brenner_on_sum);

  return true;
}
