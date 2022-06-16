#ifndef TTGO_HISTORY_H
#define TTGO_HISTORY_H



typedef struct
{
  char year[2];
  char month[2];
  char day[2];
  uint32_t sec_run_heizung;
  uint16_t sec_run_ww;
} struct_history_day;

extern struct_history_day history_day;

typedef struct
{
  struct_history_day history_day[356];
} struct_history_year;

extern uint64_t sec_run_heizung_all;

extern struct_history_year history_year;



typedef struct
{
  char buffer[45]  =   "2009/07/12 00:00:00,0,1,40.0,40.0,40.0,20.0\n";
} struct_history_string;

extern void history_init();
extern bool history_save();
extern uint32_t get_Heizoel_mLiter_Verbrauch();

#endif
