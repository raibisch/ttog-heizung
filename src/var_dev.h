
#ifndef TTGO_VAR_H
#define TTGO_VAR_H

#define DEV_SENSOR_MAX 4
#define DEV_BRENNERKESSEL  0
#define DEV_WWSPEICHER     1
#define DEV_HEIZUNG        2
#define DEV_AUSSENTEMP     3


// alle Werte in config.txt gespeichert
typedef struct 
{
  uint8_t device_manuell            = 0; 
  uint8_t hour_heizung_nacht_off    = 23;  
  uint8_t hour_heizung_werktag_on    = 6; 
  uint8_t hour_heizung_wochenende_on = 8;  
  uint8_t hour_ww_sommer_off = 12;

  uint8_t temp_winterbetrieb= 10;
  
  uint8_t temp_ww_soll_ein  = 55;
  uint8_t temp_ww_soll_aus  = 50;

  uint64_t oel_verbrauch_sec =0;
  uint16_t mliter_oel_pro_h = 1800; // 0,4Duese, 10bar, 50gradC http://www.martin-havenith.de/Betrieb___Wartung/FluidicsTabelle.pdf
                                          // sec/2=Verbrauch in ml --> sec*3600*1000/2= Verbrauch in l
  uint8_t sensor_ix[4];
}struct_config;

typedef struct
{
 //bool bManuell = false;
 bool bError = false;
 float temp_aussen = 20;
 uint32_t nachlauf = 1000*60*5;  // für Pumpe Nachlauf = 5 Min
 uint64_t millis_start_brenner = 0;
 uint64_t  millis_stop_brenner  = 0;
 uint32_t sec_brenner_on       = 0;
 uint32_t sec_brenner_old_sum   =0;
 uint64_t sec_brenner_on_sum   = 0;
} struct_allg;


typedef struct
{
 bool display_update = true;
 bool active = false;
 float temp_akt = 20;
 float temp_soll =21;
} struct_device;

typedef struct
{
  int next_menuix ;
  String button1;
  String button2;
  String button3;
  long val ;
} struct_menu;

extern struct_config Config_val;
extern struct_allg Device_allg;

extern struct_device Device_WWSpeicher, Device_Heizung, Device_BrennerKessel, Device_Pumpe;
extern struct_menu Menu[5];

extern bool loop_active;
extern int Menu_akt;

extern int min_old;
extern int hour_old;
extern int day_old;
extern int month_old;
extern int year_old;


extern struct_menu Menu[5];
extern struct_config Config_val;
extern struct_allg Device_allg;
extern struct_device Device_WWSpeicher, Device_Heizung, Device_BrennerKessel, Device_Pumpe;

extern bool var_devLoadConfig(); 
extern bool var_devSaveConfig(String s);

#endif
